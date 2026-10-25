#ifndef __KarplusStrongOscillator_hpp__
#define __KarplusStrongOscillator_hpp__

#include "FloatArray.h"
#include "NoiseGenerator.h"
#include "CircularBuffer.hpp"

/**
 * Karplus-Strong plucked-string oscillator. Not an Oscillator subclass:
 * it is excited by a trigger() and does not support frequency modulation.
 */
class KarplusStrongOscillator {
private:
  const float fs;
  SignalGenerator* noise;
  CircularBuffer* pluck; // output y(n)
  int phase;
  int numSamps;
  float decay; // filter gain/string tension/decay factor
public:
  KarplusStrongOscillator(float sr, SignalGenerator* ns, CircularBuffer* pbf) 
    : fs(sr), noise(ns), pluck(pbf), phase(0), numSamps(0), decay(0.485) {}

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

  float generate(){
    float sample = 0.0f;
    if(phase > (numSamps +  1)){
      // if we have filled up our delay line, y(n) = gain * (y(n-N) + y( n-(N+1) ))
      sample = decay * ( pluck->read(numSamps) + pluck->read(numSamps + 1));
    }else{
      // computing the first N samples, y(n) = x(n)
      sample = noise->generate();
    }
    pluck->write(sample);
    phase++;
    return sample;
  }

  float getNextSample(){
    return generate();
  }

  void getSamples(FloatArray output){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = generate();
  }

  static void destroy(KarplusStrongOscillator* kp){
    WhiteNoiseGenerator::destroy((WhiteNoiseGenerator*)kp->noise);
    CircularBuffer::destroy(kp->pluck);
    delete kp;
  }

  static KarplusStrongOscillator* create(float sr, int size){
    KarplusStrongOscillator* kp = 
      new KarplusStrongOscillator(sr, 
				  WhiteNoiseGenerator::create(),
				  CircularBuffer::create(size));
    return kp;
  }
};

#endif // __KarplusStrongOscillator_hpp__
