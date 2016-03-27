#ifndef __ChirpPatch_hpp__
#define __ChirpPatch_hpp__

#include "StompBox.h"
#include "Oscillators.hpp"

class ChirpPatch : public Patch {
private:
  ChirpOscillator* chirp[2];
  bool buttonstate = false;
public:
  ChirpPatch(){
    registerParameter(PARAMETER_A, "Tone");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Rate");
    registerParameter(PARAMETER_D, "Level");
    chirp[0] = new ChirpOscillator(getSampleRate());
    chirp[1] = new ChirpOscillator(getSampleRate());
  }
  ~ChirpPatch(){
    delete chirp[0];
    delete chirp[1];
  }
  void processAudio(AudioBuffer& buffer){
    float freq = 20*powf(2, getParameterValue(PARAMETER_A)*6);
    float decay = getParameterValue(PARAMETER_B)*10;
    float rate = 10-getParameterValue(PARAMETER_C)*20;
    float level = getParameterValue(PARAMETER_D);
    if(isButtonPressed(PUSHBUTTON) != buttonstate){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate){
	chirp[0]->setFrequency(freq);
	chirp[0]->setDecay(decay);
	chirp[0]->trigger();
	chirp[1]->setFrequency(freq);
	chirp[1]->setRate(rate);
	chirp[1]->trigger();
      }
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    chirp[0]->getSamples(left);
    chirp[1]->getSamples(right);
    left.multiply(level);
    right.multiply(level);
  }
};

#endif   // __ChirpPatch_hpp__
