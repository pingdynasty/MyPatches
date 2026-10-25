#ifndef __LichTestPatch_hpp__
#define __LichTestPatch_hpp__

#include "Patch.h"
#include "SmoothValue.h"
// #include "Oscillators.hpp"
#include "VoltsPerOctave.h"

class LichTestPatch : public Patch {
private:
  VoltsPerOctave hz;
  SmoothFloat lavg;
  SmoothFloat ravg;
public:
  LichTestPatch() {
    registerParameter(PARAMETER_A, "CV A");
    registerParameter(PARAMETER_B, "CV B");
    registerParameter(PARAMETER_F, "CV F>");
    registerParameter(PARAMETER_G, "CV G>");
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
      // setButton(PUSHBUTTON, value);
      break;
    case PUSHBUTTON:
    case BUTTON_A:
      sendMidi(MidiMessage::note(0, 60, value ? 63 : 0));
      break;
    case BUTTON_B:
      sendMidi(MidiMessage::note(0, 62, value ? 63 : 0));
      break;
    case BUTTON_C:
      setButton(BUTTON_E, value ? 4095 : 0);
      break;
    case BUTTON_D:
      setButton(BUTTON_F, value ? 4095 : 0);
      break;
    }
    static int count = 0;
    // debugMessage("button event", bid, value, ++count);
    // sendMidi(MidiMessage::note(0, 66, value ? 63 : 0));      
  }

  void processMidi(MidiMessage msg){
    if(msg.isNoteOn())
      debugMessage("note on", msg.getNote(), msg.getVelocity());
  }
  
  void processAudio(AudioBuffer &buffer) {
    if(isButtonPressed(BUTTON_B)){
      debugMessage("c/d/e", getParameterValue(PARAMETER_C), getParameterValue(PARAMETER_D), getParameterValue(PARAMETER_E));
      return;
    }else if(isButtonPressed(BUTTON_A)){
      debugMessage("a/b/c", getParameterValue(PARAMETER_A), getParameterValue(PARAMETER_B), getParameterValue(PARAMETER_C));
      return;
    }else{
      float a = getParameterValue(PARAMETER_B)*4096;
      static float amin, amax;
      static SmoothFloat avg;
      avg = a;
      if(isButtonPressed(BUTTON_A)){
	amin = a;
	amax = a;
      }
      if(a < amin)
	amin = a;
      if(a > amax)
	amax = a;
      // debugMessage("a/b/c", getParameterValue(PARAMETER_A)*4096, getParameterValue(PARAMETER_B)*4096, getParameterValue(PARAMETER_E)*4096);
      debugMessage("min/max/b", amin, amax, a);
    }
    
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    lavg = left.getMean();
    ravg = right.getMean();
    // debugMessage("AVG L/R", lavg*1000, ravg*1000);
    // debugMessage("AVG L/R", (int)roundf(lavg*1000), (int)roundf(ravg*1000));
    // debugMessage("AVG L/R", (int)roundf(lavg*65535), (int)roundf(ravg*65535));
      
    // setParameterValue(PARAMETER_F, 0.5);
    // setParameterValue(PARAMETER_G, 0.5);
    setParameterValue(PARAMETER_F, getParameterValue(PARAMETER_A));
    setParameterValue(PARAMETER_G, getParameterValue(PARAMETER_B));
    
    if(isButtonPressed(BUTTON_A)){
      left.setAll(0.5);
      debugMessage("500", lavg*1000, hz.sampleToVolts(lavg));
    }else if(isButtonPressed(BUTTON_B)){
      left.setAll(-0.5);
      debugMessage("-500", lavg*1000, hz.sampleToVolts(lavg));
    }
    // buffer.getSamples(0).setAll(getParameterValue(PARAMETER_C)*2-1);
    // buffer.getSamples(1).setAll(getParameterValue(PARAMETER_D)*2-1);
  }
};

#endif   // __LichTestPatch_hpp__
