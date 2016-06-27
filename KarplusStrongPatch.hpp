#ifndef __KarplusStrongPatch_hpp__
#define __KarplusStrongPatch_hpp__

#include "StompBox.h"
#include "KarplusStrongOscillator.hpp"

class KarplusStrongPatch : public Patch {
private:
  KarplusStrongOscillator* osc;
  int maxDuration;
  bool buttonstate = false;
public:
  KarplusStrongPatch(){
    registerParameter(PARAMETER_A, "Freq");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Freq");
    registerParameter(PARAMETER_D, "Decay");
    osc = KarplusStrongOscillator::create(getSampleRate(), 1024);
    maxDuration = getSampleRate()*3; // 3 seconds
  }

  void processAudio(AudioBuffer &buffer){
    float freq = getParameterValue(PARAMETER_A)*6;
    float decay = getParameterValue(PARAMETER_C);
    freq = 110.f * powf(2, freq);

    osc->setFrequency(freq);
    osc->setDecay(decay);
    if(buttonstate != isButtonPressed(PUSHBUTTON)){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate) // rising edge
	osc->trigger();
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    osc->getSamples(left);
  }
};

#endif // __KarplusStrongPatch_hpp__
