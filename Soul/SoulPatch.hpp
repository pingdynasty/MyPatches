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
// #include "Reverb.hpp"
// #define SOULPATCH Reverb
#include "Delay.hpp"
#define SOULPATCH Delay

namespace std {void __throw_bad_function_call() { while(1); }; }


class SoulPatch : public Patch {
private:
  SOULPATCH soulpatch;
  std::vector<SOULPATCH::Parameter> params;
  AudioBuffer* outputBuffer;
public:
  SoulPatch(){
    soulpatch.init(getSampleRate(), 0);
    params = soulpatch.createParameterList();
    for(int i=0; i<params.size(); ++i){
      registerParameter(PatchParameterId(PARAMETER_A+i), params[i].properties.name);
    }
    ASSERT(getNumberOfChannels() >= SOULPATCH::numAudioInputChannels, "Too many input channels in SOUL patch");
    ASSERT(getNumberOfChannels() >= SOULPATCH::numAudioOutputChannels, "Too many output channels in SOUL patch");
    outputBuffer = AudioBuffer::create(SOULPATCH::numAudioOutputChannels, getBlockSize());
  }
  void processAudio(AudioBuffer &buffer){
    SOULPATCH::RenderContext<float> ctx;
    for(int i=0; i<params.size(); ++i){
      float min = params[i].properties.minValue;
      float max = params[i].properties.maxValue;
      float value = getParameterValue(PatchParameterId(PARAMETER_A+i));      
      params[i].setValue(value * (max-min) + min);
    }
    ctx.incomingMIDI.numMessages = 0;
    for(int i=0; i<ctx.inputChannels.size(); ++i)
      ctx.inputChannels[i] = buffer.getSamples(i);
    for(int i=0; i<ctx.outputChannels.size(); ++i){
      outputBuffer->clear();
      ctx.outputChannels[i] = outputBuffer->getSamples(i);
      // ctx.outputChannels[i] = buffer.getSamples(i); // in-place
    }
    // debugMessage("xruns", (int)soulpatch.getNumXRuns());
    ctx.numFrames = buffer.getSize();
    soulpatch.render(ctx);
    for(int i=0; i<ctx.outputChannels.size(); ++i){
      buffer.getSamples(i).copyFrom(outputBuffer->getSamples(i));
    }
  }
};

#endif // __SoulPatch_hpp__
