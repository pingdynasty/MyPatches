#ifndef __Drum_hpp__
#define __Drum_hpp__

#include "Envelope.h"
#include "Oscillator.h"

class Drum : public Oscillator {
public:
  virtual void setDecay(float amount){}
  virtual void setAccent(float accent){}
};

#endif // __Drum_hpp__

