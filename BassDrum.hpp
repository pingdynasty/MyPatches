#ifndef __BassDrum_hpp__
#define __BassDrum_hpp__

#include "Envelope.h"
#include "BiquadFilter.h"
#include "NoiseOscillator.h"
#include "Oscillators.hpp"
#include "Drum.hpp"

class ExponentialDecayEnvelope : public Envelope {
private:
  const float fs;
  float value;
  float incr;
public:
  ExponentialDecayEnvelope(float sr)
    : fs(sr){}
  void setRate(float r){
    if(r < 0)
      incr = 1.0f - 100*(1/(1-r))/fs;
    else
      incr = 1.0f + 100*r/fs;
  }
  void setDecay(float d){
    setRate(-(d+1/fs));
  }
  void trigger(){
    value = 1.0;
  }
  float getNextSample(){
    float sample = value;
    value *= incr;
    return sample;
  }
};
  
class BassDrumVoice : public Drum {
private:
  // AdsrEnvelope* env;
  // FloatArray env;
  SineOscillator* sine;
  ChirpOscillator* chirp;
  ImpulseOscillator* impulse;
  ExponentialDecayEnvelope* env1;
  ExponentialDecayEnvelope* env2;
  Oscillator* noise;
  BiquadFilter* filter;
  float freq;
  float decay;
  float snare;
  float balance;
  const float fs;
public:
  BassDrumVoice(float sr) : fs(sr) {
    // env = new AdsrEnvelope(sr);
    // env = FloatArray::create(1024);
    // for(int i=0; i<env.getSize(); ++i)
    //   env[i] = expf(-M_E*i/env.getSize());
    snare = 0;
    balance = 0.2;
    sine = new SineOscillator(sr);
    chirp = new ChirpOscillator(sr);
    impulse = new ImpulseOscillator();
    env1 = new ExponentialDecayEnvelope(sr);
    env2 = new ExponentialDecayEnvelope(sr);
    noise = new PinkNoiseOscillator();
    filter = BiquadFilter::create(1);
    filter->setLowPass(0.6, FilterStage::BUTTERWORTH_Q);
  }  
  void setFrequency(float f){
    freq = f;
    // filter->setLowPass(f*2/(fs*2), FilterStage::BUTTERWORTH_Q);
  }
  void setDecay(float d){
    decay = d*60;
  }
  void setSnap(float s){
    snare = s;
    balance = s*0.5;
    filter->setLowPass(0.25+balance, FilterStage::BUTTERWORTH_Q);
  }
  void trigger(bool state, int delay){
    sine->setFrequency(freq);
    chirp->setFrequency(freq*2);
    env1->setDecay(decay);
    env2->setDecay(decay*snare*0.5);
    chirp->setDecay(decay);
    env1->trigger();
    env2->trigger();
    chirp->trigger();
    impulse->trigger();
  }
  float getNextSample(){
    float vca1 = sine->getNextSample();
    vca1 += chirp->getNextSample();
    vca1 *= env1->getNextSample();

    float vca2 = 0.0f;
    vca2 += impulse->getNextSample();
    // vca2 += filter->process(noise->getNextSample());
    // vca2 *= env2->getNextSample();
    vca2 += noise->getNextSample();
    vca2 = filter->process(vca2);
    vca2 *= env2->getNextSample();
    
    float sample = vca1*(1.0-balance) + vca2*balance;
    return sample;
  }
};

#endif // __BassDrum_hpp__
