#ifndef __Drum_hpp__
#define __Drum_hpp__

class Drum : public Oscillator {
public:
  virtual void trigger(){
    trigger(true, 0);
  }
  virtual void trigger(bool state){
    trigger(state, 0);
  }
  virtual void trigger(bool state, int triggerDelay){}
  virtual void gate(bool state){
    gate(state, 0);
  }
  virtual void gate(bool state, int gateDelay){}
};

#endif // __Drum_hpp__

