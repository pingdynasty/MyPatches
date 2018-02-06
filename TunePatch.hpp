#ifndef __TunePatch_hpp__
#define __TunePatch_hpp__

#include "StompBox.h"
// #include "Oscillators.hpp"

class TunePatch : public Patch {
private:
  SmoothFloat min, max, avg;
public:
  TunePatch() {
  }

  void reset();

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
    }else if(bid == PUSHBUTTON){
      avg = min = max = 0.0;
    }else{
    }
  }

  void processAudio(AudioBuffer &buffer) {
    FloatArray data = buffer.getSamples(LEFT_CHANNEL);
    min = data.getMinValue();
    max = data.getMaxValue();
    avg = data.getMean();
    debugMessage("min/max/avg", min*1000, max*1000, avg*1000);
  }
};

#endif   // __TunePatch_hpp__
