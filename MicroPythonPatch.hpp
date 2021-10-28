#ifndef __MicroPythonPatch_hpp__
#define __MicroPythonPatch_hpp__

/**
   cd micropython/ports/owl
   make CROSS=1 clean all
   cp build/libmicropython-cm4.a ~/devel/OwlProgram/MyPatches/
   make CROSS=0 clean all
   cp build/libmicropython-i386.a ~/devel/OwlProgram/MyPatches/
 */

#define USE_MP_SCREEN
#ifdef USE_MP_SCREEN
#include "MonochromeScreenPatch.h"
#else
#include "Patch.h"
#endif
#include "ServiceCall.h"
#include "message.h"

// #define NO_QSTR

// #define HEAPSIZE (6*1024*1024)
#define HEAPSIZE (256*1024)

extern "C" {

  static const char* mp_message = NULL;
  void output_string(const char* str){
    if(str != NULL && str[0] != 0 && str[0] != 13){ // ignore cr lf
      debugMessage(str);
      mp_message = str;
    }
  }
	     
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
    output_string(str);
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

// class MicroPythonSetter {
// private:
// public:
//   mp_obj_t setter_obj = NULL;
//   void fill(FloatArray output){
//     if(setter_obj != NULL){
//       const mp_obj_type_t* type = mp_obj_get_type(setter_obj);
//       if(type == &mp_type_bool || type == &mp_type_int){
// 	int dcvalue = mp_obj_get_int(setter_obj);
// 	float val = dcvalue == 0 ? 0.0f : dcvalue > 0 ? 0.99f : -0.99f;
// 	output.setAll(val);
//       }else{
// 	float dcvalue = mp_obj_get_float(setter_obj);
// 	output.setAll(dcvalue);
//       }
//     }
//   }
// };

class AbstractIterator {
public:
  virtual void fill(FloatArray output) = 0;
  virtual float next() = 0;
  float getFloatValue(mp_obj_t item){
    if(mp_obj_is_float(item)){
      mp_float_t value = mp_obj_float_get(item);
      // float value = mp_obj_get_float(item);
      return value;
    }else if(mp_obj_is_integer(item)){
      return mp_obj_get_int(item);
    }else{
      return 0;
    }
  }
};

class ConstantIterator : public AbstractIterator {
private:
public:
  float dcvalue = 0;
  ConstantIterator(float value): dcvalue(value){}
  void fill(FloatArray output){
    output.setAll(dcvalue);
  }
  float next(){
    return dcvalue;
  }
};

class IterableIterator : public AbstractIterator {
public:
  // typedef void *mp_obj_t;
  // typedef struct _mp_obj_iter_buf_t {
  //     mp_obj_base_t base;
  //     mp_obj_t buf[3];
  // } mp_obj_iter_buf_t;
  mp_obj_iter_buf_t iter_buf;
  mp_obj_t iterable;
  IterableIterator(mp_obj_t iterator_obj) {
    iterable = mp_getiter(iterator_obj, &iter_buf);    
  }
  void fill(FloatArray output){
    if(iterable != NULL){
      for(size_t i=0; i<output.getSize(); ++i){
	mp_obj_t item = mp_iternext(iterable);
	if(item == MP_OBJ_STOP_ITERATION){
	  iterable = NULL;
	  return;
	}else{
	  output[i] = getFloatValue(item);
	}
      }
    }
  }
  float next(){
    if(iterable != NULL){
      mp_obj_t item = mp_iternext(iterable);
      if(item == MP_OBJ_STOP_ITERATION){
	iterable = NULL;
      }else{
	return getFloatValue(item);
      }
    }
    return 0;
  }
};

// class ListIterator : public AbstractIterator {
// public:
    // mp_obj_tuple_get(iterator_obj, len, items)
// };

AbstractIterator* createIterator(mp_obj_t iterator_obj){
  const mp_obj_type_t* type = mp_obj_get_type(iterator_obj);
  if(type == &mp_type_bool || type == &mp_type_int){
    float dcvalue = mp_obj_get_int(iterator_obj);
    dcvalue = clamp(dcvalue, -0.99f, 0.99f);
    return new ConstantIterator(dcvalue);
  }else if(type == &mp_type_float){
    float dcvalue = mp_obj_get_float(iterator_obj);
    dcvalue = clamp(dcvalue, -0.99f, 0.99f);
    return new ConstantIterator(dcvalue);
    // void mp_obj_get_array(mp_obj_t o, size_t *len, mp_obj_t **items);

    // if (mp_obj_is_type(o, &mp_type_tuple)) {
    //     mp_obj_tuple_get(o, len, items);
    // } else if (mp_obj_is_type(o, &mp_type_list)) {
    //     mp_obj_list_get(o, len, items);

    // }else if(type == &mp_type_tuple){
    // }else if(type == &mp_type_list){ // could be a list
      // return new ListIterator(iterator_obj);

    // mp_obj_tuple_get(o, len, items);
    // }else if(type == &mp_type_array){

    // 	mp_obj_list_t* list = MP_OBJ_TO_PTR(iterator_obj);
    // 	list->len
    // 	  list->items
    // }else if(type == &mp_type_polymorph_iter){
    // }else if(type == &mp_type_range){
    // }else if(type == &mp_type_range_it){
    // }else if(type == &mp_type_set_it){
  // }else if(type->getiter != NULL){ // check if __iter__ is available
  }
    return new IterableIterator(iterator_obj);
  // }
  // return NULL;
}


#define MP_NOF_CHANNELS 2
#define MP_NOF_PARAMETERS 8

static AbstractIterator* outputs[MP_NOF_CHANNELS] = {};
static AbstractIterator* parameters[MP_NOF_PARAMETERS] = {};

#ifdef USE_MP_SCREEN
static MonochromeScreenBuffer* mp_screen = NULL;
#endif

extern "C" {
  void doSetOutputIterator(uint8_t ch, mp_obj_t iterator){
    if(ch < MP_NOF_CHANNELS)
      outputs[ch] = createIterator(iterator);
  }
  void doSetParameterIterator(uint8_t id, mp_obj_t iterator){
    if(id < MP_NOF_PARAMETERS)
      parameters[id] = createIterator(iterator);
  }
  void doScreenPrint(int x, int y, const char* text){
#ifdef USE_MP_SCREEN
    if(mp_screen != NULL)
      mp_screen->print(x, y, text);
#endif
  }
  void doScreenDraw(int x0, int y0, int x1, int y1, int c){
#ifdef USE_MP_SCREEN
    if(mp_screen != NULL)
      mp_screen->drawLine(x0, y0, x1, y1, c);
#endif
  }
  void doScreenClear(){
#ifdef USE_MP_SCREEN
    if(mp_screen != NULL)
      mp_screen->clear();
    mp_message = NULL;
#endif
  }
}

#ifdef USE_MP_SCREEN
class MicroPythonPatch : public MonochromeScreenPatch {
#else
class MicroPythonPatch : public Patch {
#endif
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
#ifdef USE_MP_SCREEN
    mp_screen = new MonochromeScreenBuffer(getScreenWidth(), getScreenHeight());
    mp_screen->setBuffer(new uint8_t[getScreenWidth()*getScreenHeight()/8]);
#endif
    mpmain();
  }

  ~MicroPythonPatch() {
    for(size_t pid=0; pid<MP_NOF_PARAMETERS; ++pid)
      delete parameters[pid];
    for(size_t ch=0; ch<MP_NOF_CHANNELS; ++ch)
      delete outputs[ch];
#ifdef USE_MP_SCREEN
    delete[] mp_screen->getBuffer();
    delete mp_screen;
#endif
  }

#ifdef USE_MP_SCREEN
  void processScreen(MonochromeScreenBuffer& screen){
    screen.copyFrom(*mp_screen);
    if(mp_message != NULL){
      screen.setTextSize(1);
      screen.print(0, 12, mp_message);
    }
  }
#endif

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
      "import owl\n" 
      "import math\n"
      "print('hello embedded world!')\n"
      // "def sin_osc(freq, duration = 1, sample_rate = 48000):\n"
      // "  ph = 0\n"
      // "  samples = 0\n"
      // "  inc = (2 * math.pi * freq) / sample_rate\n"
      // "  while samples < duration*sample_rate:\n"
      // "    yield math.sin(ph)\n"
      // "    ph = (ph + inc) % (2*math.pi)\n"
      // "    samples += 1\n"
      // "osc0 = sin_osc(440, 2.5)\n"
      // "osc1 = sin_osc(880, 2.5)\n"
      // "owl.setOutput(0, osc0)\n"
      // "owl.setOutput(1, osc1)"
      ;
    mp_obj_t ret = execute_from_str(str);
    if(ret)
      output_string("error");
    return 0;
  }

  void processAudio(AudioBuffer &buffer) {
    for(size_t pid=0; pid<MP_NOF_PARAMETERS; ++pid){
      if(parameters[pid] != NULL)
	setParameterValue((PatchParameterId)pid, parameters[pid]->next());
    }
    buffer.clear();
    for(size_t ch=0; ch<MP_NOF_CHANNELS; ++ch){
      if(outputs[ch] != NULL)
	outputs[ch]->fill(buffer.getSamples(ch));
    }
  }

  void processMessage(const char* str, size_t len){
    mp_obj_t ret = execute_from_str(str);
    if(ret){
      const char* msg;
      if(mp_obj_is_str(ret)){
	msg = mp_obj_str_get_str(ret);
      }else{
	// mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(ret));
	msg = mp_obj_get_type_str(ret);
      }
      output_string(msg);
    }
  }
  
};

#endif   // __MicroPythonPatch_hpp__
