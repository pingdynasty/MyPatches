#ifndef __BassDrumPatch_hpp__
#define __BassDrumPatch_hpp__

#include "StompBox.h"
#include "BassDrum.hpp"

class BassDrumPatch : public Patch {
private:
  BassDrumVoice* drum[2];
  bool buttonstate = false;
public:
  BassDrumPatch(){
    registerParameter(PARAMETER_A, "Tone");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Snare");
    registerParameter(PARAMETER_D, "Level");
    drum[0] = new BassDrumVoice(getSampleRate());
    drum[1] = new BassDrumVoice(getSampleRate());
    drum[0]->setSnap(0.2);
  }
  ~BassDrumPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float tone = 20*powf(2, getParameterValue(PARAMETER_A)*4);
    float decay = getParameterValue(PARAMETER_B)*100;
    float c = getParameterValue(PARAMETER_C);
    float level = getParameterValue(PARAMETER_D)*2;
    drum[0]->setDecay(decay);
    drum[0]->setFrequency(tone);
    drum[1]->setDecay(decay*2);
    drum[1]->setFrequency(tone*2);
    drum[1]->setSnap(c);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    if(isButtonPressed(PUSHBUTTON) != buttonstate){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate){
	drum[0]->trigger();
	drum[1]->trigger();
      }
    }
    drum[0]->getSamples(left);
    drum[1]->getSamples(right);
    left.multiply(level);
    right.multiply(level);
  }
};

#endif   // __BassDrumPatch_hpp__
