#ifndef __CalibrationPatch_hpp__
#define __CalibrationPatch_hpp__

#include "Patch.h"
#include "SmoothValue.h"
// #include "Oscillators.hpp"
#include "VoltsPerOctave.h"

class CalibrationPatch : public Patch {
private:
  VoltsPerOctave hz;
  SmoothFloat lavg;
  SmoothFloat ravg;
  bool button1 = false;
  bool button2 = false;
public:
  CalibrationPatch() {
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      sendMidi(MidiMessage::note(0, 60, value ? 63 : 0));
      break;
    case BUTTON_B:
      sendMidi(MidiMessage::note(0, 62, value ? 63 : 0));
      break;
    }
  }

  void processMidi(MidiMessage msg){
    if(msg.isNoteOn()){
      debugMessage("note on", msg.getNote(), msg.getVelocity());
      setButton(PUSHBUTTON, 4095);
    }else if(msg.isNoteOff()){
      setButton(PUSHBUTTON, 0);
    }else if(msg.isControlChange()){
      switch(msg.getControllerNumber()){
      case 27:
	setButton(BUTTON_A, msg.getControllerValue());
	button1 = msg.getControllerValue();
	break;
      case 28:
	setButton(BUTTON_B, msg.getControllerValue());
	button2 = msg.getControllerValue();
	break;
      }
    }
  }
  
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    lavg = left.getMean();
    ravg = right.getMean();
 
    setParameterValue(PARAMETER_F, getParameterValue(PARAMETER_A));
    setParameterValue(PARAMETER_G, getParameterValue(PARAMETER_B));
 
    if(isButtonPressed(BUTTON_A) || button1){
      left.setAll(0.5);
      // debugMessage("500", lavg*1000, hz.sampleToVolts(lavg));
      debugMessage("High", lavg*1000);
    }else if(isButtonPressed(BUTTON_B) || button2){
      left.setAll(-0.5);
      // debugMessage("-500", lavg*1000, hz.sampleToVolts(lavg));
      debugMessage("Low", lavg*1000);
    }else{
    // buffer.getSamples(0).setAll(getParameterValue(PARAMETER_C)*2-1);
    // buffer.getSamples(1).setAll(getParameterValue(PARAMETER_D)*2-1);
    }
  }
};

#endif   // __CalibrationPatch_hpp__
