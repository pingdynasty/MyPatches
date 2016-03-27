#ifndef __Oscillators_hpp__
#define __Oscillators_hpp__

#include "Oscillator.h"

class SineOscillator : public Oscillator {
private:
  const float fs;
  float phase;
  float incr;
public:
  SineOscillator(float sr)
    : fs(sr), phase(0.0f), incr(0.0f){}
  void setFrequency(float freq){
    incr = freq*2*M_PI/fs;
  }
  void setPhase(float ph){
    phase = ph;
    while(phase >= 2*M_PI)
      phase -= 2*M_PI;
  }
  float getPhase(){
    return phase;
  }
  float getNextSample(){
    float sample = sinf(phase);
    phase += incr;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }
};

class ImpulseOscillator : public Oscillator {
private:
  bool triggered = false;
public:
  void trigger(){
    triggered = false;
  }
  float getNextSample(){
    if(triggered)
      return 0.0f;
    triggered = true;
    return 1.0f;
  }
};

class ChirpOscillator : public Oscillator {
private:
  const float fs;
  float phase;
  float incr;
public:
  float rate;
  ChirpOscillator(float sr)
    : fs(sr), phase(0.0f), incr(1.0f){}
  void setFrequency(float freq){
    incr = freq*2*M_PI/fs;
  }
  void setRate(float r){
    // should be: rate 0: 1 (constant), rate -1: 1-0.5/sr (halved in a second), rate 1: 1+1/sr (doubled in a second)
    if(r < 0)
      rate = 1.0f - 10*(1/(1-r))/fs;
    else
      rate = 1.0f + 10*(1/(1+r))/fs;
  }
  void setDecay(float d){
    setRate(-(d+1/fs));
  }
  void trigger(){
    phase = 0.0f;
  }
  float getNextSample(){
    float sample = sinf(phase);
    phase += incr;
    incr *= rate;
    // phase %= 2*M_PI;
    // if(phase >= 2*M_PI)
    //   phase -= 2*M_PI;
    return sample;
  }
};

#endif // __Oscillators_hpp__
