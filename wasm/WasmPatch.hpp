#ifndef __WasmPatch_hpp__
#define __WasmPatch_hpp__

#include "wasm3.h"
#include "m3_env.h"
#include "fib32_tail.wasm.h"

#include "Patch.h"

#define FATAL(x, y) error(PROGRAM_ERROR_STATUS, x)

class WasmPatch : public Patch {
private:
  IM3Environment env;
  IM3Runtime runtime;
  IM3Module module;
  IM3Function function;
public:
  WasmPatch() {
    // registerParameter(PARAMETER_A, "one>");
    // debugMessage("sr/bs/ch", (int)getSampleRate(), getBlockSize(), getChannels());

    M3Result result = m3Err_none;
    uint8_t* wasm = (uint8_t*)fib32_tail_wasm;
    size_t fsize = fib32_tail_wasm_len;
    // puts("Loading WebAssembly...");
    env = m3_NewEnvironment ();
    if (!env) FATAL("m3_NewEnvironment", "failed");
    runtime = m3_NewRuntime (env, 1024, NULL);
    if (!runtime) FATAL("m3_NewRuntime", "failed");
    result = m3_ParseModule (env, &module, wasm, fsize);
    if (result) FATAL("m3_ParseModule", result);
    result = m3_LoadModule (runtime, module);
    if (result) FATAL("m3_LoadModule", result);
    result = m3_FindFunction (&function, runtime, "fib");
    if (result) FATAL("m3_FindFunction", result);
    // run_wasm(24);
  }

  void processAudio(AudioBuffer &buffer) {
    float a = getParameterValue(PARAMETER_A);
    int arg = a*30;
    M3Result result = m3Err_none;
    result = m3_CallV (function, arg);
    if (result) FATAL("m3_Call", result);
    long value = *(uint64_t*)(runtime->stack);
    debugMessage("Result", arg, value);

    // debugMessage("a/b/c", a, b, c);
  }
};

#endif   // __WasmPatch_hpp__
