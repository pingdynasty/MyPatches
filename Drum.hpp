#ifndef __Drum_hpp__
#define __Drum_hpp__

#include "Envelope.h"
#include "Oscillator.h"

class Drum : public Oscillator {
public:
  virtual void setDecay(float amount){}
  virtual void setAccent(float accent){}
  virtual void trigger(bool state, int delay){}
  virtual void gate(bool state, int delay){}
  virtual void trigger(){
    trigger(true, 0);
  }
};

#endif // __Drum_hpp__

