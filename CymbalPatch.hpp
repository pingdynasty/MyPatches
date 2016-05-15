#ifndef __CymbalPatch_hpp__
#define __CymbalPatch_hpp__

#include "StompBox.h"
#include "Cymbal.hpp"

class CymbalPatch : public Patch {
private:
  CymbalVoice* cymbal[2];
  bool buttonstate = false;
public:
  CymbalPatch(){
    registerParameter(PARAMETER_A, "Tone");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Filter");
    registerParameter(PARAMETER_D, "FM");
    cymbal[0] = new CymbalVoice(getSampleRate());
    cymbal[1] = new CymbalVoice(getSampleRate());
  }
  ~CymbalPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float tone = 60*powf(2, getParameterValue(PARAMETER_A)*6);
    float decay = getParameterValue(PARAMETER_B);
    float fc = getParameterValue(PARAMETER_C)*0.498+0.01;
    float fm = getParameterValue(PARAMETER_D);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    cymbal[0]->setFrequency(tone);
    cymbal[0]->setFilter(fc);
    cymbal[0]->setDecay(decay);
    cymbal[0]->setFmAmount(fm);
    cymbal[1]->setFrequency(tone);
    cymbal[1]->setFilter(fc);
    cymbal[1]->setDecay(decay/2);
    cymbal[1]->setFmAmount(fm);
    if(buttonstate != isButtonPressed(PUSHBUTTON)){
      buttonstate = isButtonPressed(PUSHBUTTON);
      cymbal[0]->gate(buttonstate, getSamplesSinceButtonPressed(PUSHBUTTON));
      cymbal[1]->trigger(buttonstate, getSamplesSinceButtonPressed(PUSHBUTTON));
    }
    cymbal[0]->getSamples(left);
    cymbal[1]->getSamples(right);
  }
};

#endif   // __CymbalPatch_hpp__
