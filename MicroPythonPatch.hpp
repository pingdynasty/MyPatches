#ifndef __MicroPythonPatch_hpp__
#define __MicroPythonPatch_hpp__

/**
   cd micropython/ports/owl
   make CROSS=1 clean all
   cp build/libmicropython-cm4.a ~/devel/OwlProgram/MyPatches/
   make CROSS=0 clean all
   cp build/libmicropython-i386.a ~/devel/OwlProgram/MyPatches/
 */
#include "Patch.h"
#include "ServiceCall.h"
#include "message.h"

// #define NO_QSTR

#define HEAPSIZE (6*1024*1024) // bigger heap is slower
// #define HEAPSIZE (256*1024)

extern "C" {

#define MP_CONFIGFILE "ports/owl/mpconfigport.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"

#ifndef ARM_CORTEX
#include <unistd.h>
#endif

#if defined ARM_CORTEX && MICROPY_ENABLE_GC != 1
  extern char _eprogram;
  char *__brkval = (char *)&_eprogram;
  void * _sbrk(int incr){
    char *prev = __brkval;
    __brkval += incr;
    return prev;
  }
#endif

  volatile size_t msg_len = 0;
  volatile size_t msg_idx = 0;
  const char* volatile msg_str = NULL;

  // Receive single character
  int mp_hal_stdin_rx_chr(void) {
    // blocking?
    char c = 0;
    while(msg_idx >= msg_len); // spin
    // if(msg_idx < msg_len)
    c = msg_str[msg_idx++];
    return c;
  }

  // Send string of given length
  void mp_hal_stdout_tx_strn(const char* str, mp_uint_t len) {
#ifdef ARM_CORTEX
    // if(len > 2) // ignore cr lf
    if(len > 0 && str[0] != 13) // ignore cr lf
      debugMessage(str);
#else
    write(STDOUT_FILENO, str, len);
#endif
  }

  void NORETURN __fatal_error(const char *msg) {
    error(PROGRAM_ERROR_STATUS, msg);
    for(;;);
  }

  // Handle uncaught exceptions (should never be reached in a correct C implementation).
  void nlr_jump_fail(void *val) {
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    for(;;);
    // __fatal_error("uPy exc");
  }

  void abort(void) {
    __fatal_error("uPy fatal");
  }
  
};

class MicroPythonIterator {
private:
public:
  mp_obj_t iterator_obj = NULL;
  void fill(FloatArray output){
    if(iterator_obj != NULL){
      // mp_buffer_info_t bufinfo;
      // mp_get_buffer(iterator_obj, &bufinfo, MP_BUFFER_READ);
      // mp_get_buffer_raise(iterator_obj, &bufinfo, MP_BUFFER_READ);

      const mp_obj_type_t* type = mp_obj_get_type(iterator_obj);
      if(type == &mp_type_bool || type == &mp_type_int){
	int dcvalue = mp_obj_get_int(iterator_obj);
	float val = dcvalue == 0 ? 0.0f : dcvalue > 0 ? 0.99f : -0.99f;
	output.setAll(val);
      }else if(type == &mp_type_float){
	float dcvalue = mp_obj_get_float(iterator_obj);
	output.setAll(dcvalue);
      // }else if(type == &mp_type_list){ // could be a list
      // }else if(type == &mp_type_array){
      // 	mp_obj_list_t* list = MP_OBJ_TO_PTR(iterator_obj);
      // 	list->len
      // 	  list->items
      // }else if(type == &mp_type_polymorph_iter){
      // }else if(type == &mp_type_range){
      // }else if(type == &mp_type_range_it){
      // }else if(type == &mp_type_set_it){
      }else if(type->getiter != NULL){ // check if __iter__ is available
	mp_obj_iter_buf_t iter_buf;
	mp_obj_t iterable = mp_getiter(iterator_obj, &iter_buf);
	for(int i=0; i<output.getSize(); ++i){
	  mp_obj_t item = mp_iternext(iterable); // could be a list
	  if(item == MP_OBJ_STOP_ITERATION){
	    iterator_obj = NULL;
	    return;
	  }
	  if(mp_obj_is_float(item)){
	    mp_float_t value = mp_obj_float_get(item);
	    // float value = mp_obj_get_float(item);
	    output[i] = value;
	  }else{
	    output[i] = 0;
	  }	    
	}
      }
    }
  }
};

static MicroPythonIterator ch0;
static MicroPythonIterator ch1;

extern "C" {
  static void (*delay_callback)(size_t) = NULL;
  void doDelay(size_t ms){
    if(delay_callback == NULL){
      void* args[] = {(void*)SYSTEM_FUNCTION_SLEEP, (void*)&delay_callback};
      int ret = getProgramVector()->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, args, 2);
      if(ret != OWL_SERVICE_OK)
	delay_callback = NULL;
    }
    if(delay_callback != NULL)
      delay_callback(ms);
  }

  void doSetOutput(uint8_t ch, mp_obj_t iterator){
    if(ch == 0){
      ch0.iterator_obj = iterator;
    }else if(ch == 1){
      ch1.iterator_obj = iterator;
    }
  }
}

class MicroPythonPatch : public Patch {
private:
#if MICROPY_ENABLE_GC
  char heap[HEAPSIZE];
#endif
public:
#if defined ARM_CORTEX && MICROPY_ENABLE_GC != 1
  char heap[HEAPSIZE];
  MicroPythonPatch() {
    __brkval = heap;
#else
  MicroPythonPatch() {
#endif
    debugMessage("sr/bs/ch", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());
    // void* args[] = {(void*)SYSTEM_FUNCTION_MSG, (void*)&onMessageCallback};
    // int ret = getProgramVector()->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, args, 2);
    // debugMessage("reg msg", ret);
    mpmain();
  }

  mp_obj_t execute_from_str(const char *str) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
      qstr src_name = 1/*MP_QSTR_*/;
      mp_lexer_t *lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
      mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
      mp_obj_t module_fun = mp_compile(&pt, src_name, false);
      mp_call_function_0(module_fun);
      nlr_pop();
      return 0;
    } else {
      // uncaught exception
      return (mp_obj_t)nlr.ret_val;
    }
  }

  int mpmain() {
    // Initialized stack limit
    // mp_stack_set_limit(40000 * (sizeof(void *) / 4));
    mp_stack_ctrl_init();
    
#if MICROPY_ENABLE_GC
    // Initialize heap
    gc_init(heap, heap + sizeof(heap));
#endif
    
    // Initialize interpreter
    mp_init();

    // mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), 0);
    // mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    const char str[] = 
      // "print('Hello world of easy embedding!')\n"
      "import owl\n" 
      "import math\n" 
      // "def sin_osc(freq, duration = 1, sample_rate = 48000):\n"
      // "  ph = 0\n"
      // "  samples = 0\n"
      // "  inc = (2 * math.pi * freq) / sample_rate\n"
      // "  while samples < duration*sample_rate:\n"
      // "    yield math.sin(ph)\n"
      // "    ph = (ph + inc) % (2*math.pi)\n"
      // "    samples += 1\n"
      // "osc0 = sin_osc(440, 0.5)\n"
      // "osc1 = sin_osc(880, 0.5)\n"
      // "owl.setOutput(0, osc0)\n"
      // "owl.setOutput(1, osc1)"
      ;
    mp_obj_t ret = execute_from_str(str);
    if(ret)
      debugMessage("Err", (int)ret);

    return 0;
  }

  void processAudio(AudioBuffer &buffer) {
    ch0.fill(buffer.getSamples(0));
    ch1.fill(buffer.getSamples(1));
  }

  void processMessage(const char* str, size_t len){
    mp_obj_t ret = execute_from_str(str);
    if(ret){
      if(mp_obj_is_str(ret)){
	const char* msg = mp_obj_str_get_str(ret);
	debugMessage(msg);
      }else{
	// mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(ret));
	const char* msg = mp_obj_get_type_str(ret);
	debugMessage(msg);
      }
    }
  }
  
};

#endif   // __MicroPythonPatch_hpp__
