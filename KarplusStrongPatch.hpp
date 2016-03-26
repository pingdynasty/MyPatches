#ifndef __KarplusStrongPatch_hpp__
#define __KarplusStrongPatch_hpp__

#include "StompBox.h"
#include "Oscillator.h"
#include "CircularBuffer.hpp"

#include "NoiseOscillator.h"

class KarplusStrongOscillator : public Oscillator {
private:
  const float fs;
  Oscillator* noise;
  CircularBuffer* pluck; // output y(n)
  int phase;
  int numSamps;
public:
  int duration;
  float gain; // filter gain/string tension/decay factor
public:
  KarplusStrongOscillator(float sr, Oscillator* ns, CircularBuffer* pbf) 
    : fs(sr), noise(ns), pluck(pbf) {
    // initalize phase
    phase = 0;
  }

  void setFrequency(float f){
    numSamps = (int)(fs/f);
  }

  void trigger(){
    phase = 0;
  }

  float getNextSample(){
    float sample = 0.0f;
    if(phase > (numSamps +  1)){
      // if we have filled up our delay line, y(n) = gain * (y(n-N) + y( n-(N+1) ))
      sample = gain * ( pluck->read(numSamps) + pluck->read(numSamps + 1));
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

class KarplusStrongPatch : public Patch {
private:
  KarplusStrongOscillator* osc;
  int maxDuration;
  bool buttonstate = false;
public:
  KarplusStrongPatch(){
    registerParameter(PARAMETER_A, "Freq");
    registerParameter(PARAMETER_C, "Tension");
    registerParameter(PARAMETER_D, "Duration");
    osc = KarplusStrongOscillator::create(getSampleRate(), 1024);
    maxDuration = getSampleRate()*3; // 3 seconds
  }

  void processAudio(AudioBuffer &buffer){
    float freq = getParameterValue(PARAMETER_A)*6;
    float gain = getParameterValue(PARAMETER_C)*(0.51-0.46)+0.46;
    int duration = getParameterValue(PARAMETER_D)*maxDuration;    
    freq = 110.f * powf(2, freq);

    osc->setFrequency(freq);
    osc->gain = gain;
    osc->duration = duration;
    if(buttonstate != isButtonPressed(PUSHBUTTON)){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate) // rising edge
	osc->trigger();
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    osc->getSamples(left);
  }
};

#endif // __KarplusStrongPatch_hpp__
