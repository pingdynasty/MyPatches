#ifndef __KarplusStrongOscillator_hpp__
#define __KarplusStrongOscillator_hpp__

#include "Oscillator.h"
#include "NoiseOscillator.h"
#include "CircularBuffer.hpp"

class KarplusStrongOscillator : public Oscillator {
private:
  const float fs;
  Oscillator* noise;
  CircularBuffer* pluck; // output y(n)
  int phase;
  int numSamps;
  float decay; // filter gain/string tension/decay factor
public:
  KarplusStrongOscillator(float sr, Oscillator* ns, CircularBuffer* pbf) 
    : fs(sr), noise(ns), pluck(pbf), phase(0), decay(0.485) {}

  void setFrequency(float f){
    numSamps = (int)(fs/f);
  }

  /* set how hard the string is plucked and how fast it decays, t from 0.0 to 1.0 */
  void setDecay(float t){
    decay = t*(0.51-0.45)+0.45;
  }

  void trigger(){
    phase = 0;
  }

  float getNextSample(){
    float sample = 0.0f;
    if(phase > (numSamps +  1)){
      // if we have filled up our delay line, y(n) = gain * (y(n-N) + y( n-(N+1) ))
      sample = decay * ( pluck->read(numSamps) + pluck->read(numSamps + 1));
    }else{
      // computing the first N samples, y(n) = x(n)
      sample = noise->getNextSample();
    }
    pluck->write(sample);
    phase++;
    return sample;
  }

  static void destroy(KarplusStrongOscillator* kp){
    // GaussianNoiseOscillator::destroy(kp->noise);
    // oops: static polymorphism needed! todo.
    CircularBuffer::destroy(kp->pluck);
    delete kp;
  }

  static KarplusStrongOscillator* create(float sr, int size){
    KarplusStrongOscillator* kp = 
      new KarplusStrongOscillator(sr, 
				  new WhiteNoiseOscillator(),
				  // new PinkNoiseOscillator(),
				  // new BrownNoiseOscillator(),
				  // GaussianNoiseOscillator::create(size),
				  CircularBuffer::create(size));
    return kp;
  }
};

#endif // __KarplusStrongOscillator_hpp__
