#ifndef __Oscillators_hpp__
#define __Oscillators_hpp__

#include "Oscillator.h"
#include "SineOscillator.h"
#include "ChirpOscillator.h"

class SquareOscillator : public Oscillator {
private:
  const float fs;
  float phase;
  float incr;
  float sample;
public:
  SquareOscillator(float sr)
    : fs(sr), phase(0.0f), incr(0.0f), sample(-0.5) {}
  void setFrequency(float freq){
    incr = freq/fs;
  }
  void reset(){
    phase = 0.0;
    sample = -0.5;
  }
  float getNextSample(){
    phase += incr;
    if(phase >= 1.0){
      phase -= 1.0;
      sample *= -1;
    }
    return sample;
  }
};

class SquareFMOscillator : public Oscillator {
private:
  const float fs;
  float phase[2];
  float incr[2];
  float index;
  float sample[2];
public:
  SquareFMOscillator(float sr)
    : fs(sr), index(0.0) {
    incr[0] = 0.0;
    incr[1] = 0.0;
    sample[0] = -1.0;
    sample[1] = -1.0;
    reset();
  }
  void reset(){
    phase[0] = 0.0;
    phase[1] = 0.0;
  }
  void setFrequency(float freq){
    incr[0] = freq/fs;
  }
  void setGain(float value){
    sample[0] = copysignf(value, sample[0]);
  }
  void setModulatorFrequency(float freq){
    incr[1] = freq/fs;
  }
  void setModulatorGain(float value){
    sample[1] = copysignf(value, sample[1]);
  }
  float getNextSample(){
    phase[0] += incr[0] + sample[1];
    phase[1] += incr[1];
    if(phase[0] >= 1.0){
      phase[0] -= 1.0;
      sample[0] *= -1;
    }
    if(phase[1] >= 1.0){
      phase[1] -= 1.0;
      sample[1] *= -1;
    }
    return sample[0];
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

#endif // __Oscillators_hpp__
