#ifndef __Drum_hpp__
#define __Drum_hpp__

class Drum : public Oscillator, public Envelope {
public:
  virtual void setDecay(float amount){}
  virtual void setAccent(float accent){}
};

#endif // __Drum_hpp__

