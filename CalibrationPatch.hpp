#ifndef __CalibrationPatch_hpp__
#define __CalibrationPatch_hpp__

#include "Patch.h"
#include "SmoothValue.h"
// #include "Oscillators.hpp"
#include "VoltsPerOctave.h"

class CalibrationPatch : public Patch {
private:
  SmoothFloat lavg;
  SmoothFloat ravg;
  enum OperationMode {
		      IDLE,
		      CALIBRATE_HIGH,
		      CALIBRATE_LOW,
		      TEST_HIGH,
		      TEST_LOW
  };
  OperationMode mode;
public:
  CalibrationPatch() {
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      mode = value ? CALIBRATE_HIGH : IDLE;
      sendMidi(MidiMessage::note(0, 60, value ? 63 : 0));
      break;
    case BUTTON_B:
      mode = value ? CALIBRATE_LOW : IDLE;
      sendMidi(MidiMessage::note(0, 62, value ? 63 : 0));
      break;
    }
  }

  void processMidi(MidiMessage msg){
    if(msg.isNoteOn()){
      setButton(PUSHBUTTON, 4095);
    }else if(msg.isNoteOff()){
      setButton(PUSHBUTTON, 0);
    }else if(msg.isControlChange()){
      switch(msg.getControllerNumber()){
      case 27:
	mode = msg.getControllerValue() ? CALIBRATE_HIGH : IDLE;
	setButton(BUTTON_A, msg.getControllerValue() ? 4095 : 0);
	break;
      case 28:
	mode = msg.getControllerValue() ? CALIBRATE_LOW : IDLE;
	setButton(BUTTON_B, msg.getControllerValue() ? 4095 : 0);
	break;
      case 29:
	mode = msg.getControllerValue() ? TEST_HIGH : IDLE;
	break;
      case 30:
	mode = msg.getControllerValue() ? TEST_LOW : IDLE;
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

    switch(mode){
    case CALIBRATE_HIGH:
      left.setAll(0.5);
      debugMessage("Sample High:", lavg*1000);
      break;
    case CALIBRATE_LOW:
      left.setAll(-0.5);
      debugMessage("Sample Low:", lavg*1000);
      break;
    case TEST_HIGH:
      {
	VoltsPerOctave vin(true);
	VoltsPerOctave vout(false);
	debugMessage("Test High:", vin.sampleToVolts(lavg));
	left.setAll(vout.voltsToSample(5));
      }
      break;
    case TEST_LOW:
      {
	VoltsPerOctave vin(true);
	VoltsPerOctave vout(false);
	debugMessage("Test Low:", vin.sampleToVolts(lavg));
	left.setAll(vout.voltsToSample(-5));
      }
      break;
    }
  }
};

#endif   // __CalibrationPatch_hpp__
