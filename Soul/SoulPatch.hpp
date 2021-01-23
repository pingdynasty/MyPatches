#ifndef __SoulPatch_hpp__
#define __SoulPatch_hpp__

#include "Patch.h"

#undef min
#undef max
#undef sin
#undef cos
#undef exp
#undef sqrt
#undef pow
#undef log
#undef log10

#undef __FAST_MATH__ /* set by gcc option -ffast-math */

// #include "DiodeClipper.hpp"
// #define SOULPATCH Diode
// #include "SineSynth.hpp"
// #define SOULPATCH SineSynth
#include "Reverb.hpp"
#define SOULPATCH Reverb

namespace std {void __throw_bad_function_call() { while(1); }; }


class SoulPatch : public Patch {
private:
  SOULPATCH soulpatch;
  SOULPATCH::RenderContext<float> ctx;
  std::vector<SOULPATCH::Parameter> params;
public:
  SoulPatch():
    ctx({{}, {}, nullptr, (uint32_t)getBlockSize()}) {
    soulpatch.init(getSampleRate(), 0);
    params = soulpatch.createParameterList();
    for(int i=0; i<params.size(); ++i){
      registerParameter(PatchParameterId(PARAMETER_A+i), params[i].properties.name);
    }
  }
  void processAudio(AudioBuffer &buffer){
    for(int i=0; i<params.size(); ++i)
      params[i].setValue(PatchParameterId(PARAMETER_A+i));
    for(int i=0; i<ctx.inputChannels.size(); ++i)
      ctx.inputChannels[i] = buffer.getSamples(i);
    for(int i=0; i<ctx.outputChannels.size(); ++i)
      ctx.outputChannels[i] = buffer.getSamples(i);
    soulpatch.render(ctx);
  }
};

#endif // __SoulPatch_hpp__
