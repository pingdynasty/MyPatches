#ifndef __MorphPatch_hpp__
#define __MorphPatch_hpp__

#include "OpenWareLibrary.h"
#include "MorphingOscillator.h"

// #include "WitchFX.hpp"
// #include "WitchPatch.hpp"

class MorphPatch : public Patch {
private:
  MorphingOscillator* morph;
public:
  MorphPatch(){
    float rate = getSampleRate();
    morph = MorphingOscillator::create(5);
    morph->setOscillator(0, AgnesiOscillator::create(rate));
    morph->setOscillator(1, SineOscillator::create(rate));
    morph->setOscillator(2, TriangleOscillator::create(rate));
    morph->setOscillator(3, RampOscillator::create(rate));
    morph->setOscillator(4, InvertedSquareOscillator::create(rate));
  }
  ~MorphPatch(){
    for(size_t i=0; i<morph->getNumberOfOscillators(); ++i)
      delete morph->getOscillator(i);
    // AgnesiOscillator::destroy((AgnesiOscillator*)morph->getOscillator(0));
    // SineOscillator::destroy((SineOscillator*)morph->getOscillator(1));
    // RampOscillator::destroy((RampOscillator*)morph->getOscillator(2));
    // RampOscillator<RAMP_SHAPE>::destroy((RampOscillator<RAMP_SHAPE>*)morph->getOscillator(2));
    MorphingOscillator::destroy(morph);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      setParameterValue(PARAMETER_D, 0);
      break;
    case BUTTON_2:
      setParameterValue(PARAMETER_D, 1/3);
      break;
    case BUTTON_3:
      setParameterValue(PARAMETER_D, 2/3);
      break;
    case BUTTON_4:
      setParameterValue(PARAMETER_D, 0.999);
      break;
    }
  }
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    morph->select(getParameterValue(PARAMETER_D));
    morph->setFrequency(getParameterValue(PARAMETER_A)*440+4);
    morph->generate(left);
  }
};

#endif   // __MorphPatch_hpp__
