#ifndef __Oscillators_hpp__
#define __Oscillators_hpp__

#include "Oscillator.h"
#include "SineOscillator.h"
#include "ChirpOscillator.h"
// #include "SquareWaveOscillator.h"

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
  float getFrequency(){
    return incr[0]*fs;
  }   
  void setPhase(float ph){
    phase[0] = ph/(2*M_PI);
  }
  float getPhase(){
    // return phase 0 to 2*pi
    return phase[0]*2*M_PI;
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
  float generate(float fm){
    phase[1] += fm/fs;
    return generate();
  }
  float generate(){
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

#endif // __Oscillators_hpp__
