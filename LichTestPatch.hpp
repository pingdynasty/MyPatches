#ifndef __LichTestPatch_hpp__
#define __LichTestPatch_hpp__

#include "Patch.h"
#include "SmoothValue.h"
// #include "Oscillators.hpp"
#include "VoltsPerOctave.h"

class LichTestPatch : public Patch {
private:
  VoltsPerOctave hz;
public:
  LichTestPatch() {
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
      // setButton(PUSHBUTTON, value);
      break;
    case PUSHBUTTON:
    case BUTTON_A:
      sendMidi(MidiMessage::note(0, 60, value ? 63 : 0));
      setButton(BUTTON_B, value);
      break;
    case BUTTON_B:
      sendMidi(MidiMessage::note(0, 62, value ? 63 : 0));
      setButton(BUTTON_A, value);
      break;
    }
    static int count = 0;
    debugMessage("button event", bid, value, ++count);
    sendMidi(MidiMessage::note(0, 66, value ? 63 : 0));      
  }

  SmoothFloat lavg;
  SmoothFloat ravg;
  
  void processAudio(AudioBuffer &buffer) {

    // debugMessage("a/b/c", getParameterValue(PARAMETER_A), getParameterValue(PARAMETER_B), getParameterValue(PARAMETER_C));
    // debugMessage("c/d/e", getParameterValue(PARAMETER_C), getParameterValue(PARAMETER_D), getParameterValue(PARAMETER_E));

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    lavg = left.getMean();
    ravg = right.getMean();
    debugMessage("x1000/V", lavg*1000, hz.sampleToVolts(lavg));
    // debugMessage("AVG L/R", lavg*1000, ravg*1000);
    // debugMessage("AVG L/R", (int)roundf(lavg*1000), (int)roundf(ravg*1000));
    // debugMessage("AVG L/R", (int)roundf(lavg*65535), (int)roundf(ravg*65535));
      
    setParameterValue(PARAMETER_F, getParameterValue(PARAMETER_A));
    setParameterValue(PARAMETER_G, getParameterValue(PARAMETER_B));

    if(isButtonPressed(BUTTON_A))
      left.setAll(0.5);
    else if(isButtonPressed(BUTTON_B))
      left.setAll(-0.5);
    // buffer.getSamples(0).setAll(getParameterValue(PARAMETER_C)*2-1);
    // buffer.getSamples(1).setAll(getParameterValue(PARAMETER_D)*2-1);
  }
};

#endif   // __LichTestPatch_hpp__
