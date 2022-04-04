#ifndef __BassDrum_hpp__
#define __BassDrum_hpp__

#include "Envelope.h"
#include "BiquadFilter.h"
#include "NoiseGenerator.h"
#include "ChirpOscillator.h"
#include "SineOscillator.h"
#include "Drum.hpp"
#include "Oscillators.hpp"
#include "ExponentialDecayEnvelope.h"

class ImpulseOscillator : public SignalGenerator {
private:
  bool triggered = false;
public:
  void trigger(){
    triggered = false;
  }
  float generate(){
    if(triggered)
      return 0.0f;
    triggered = true;
    return 1.0f;
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
  PinkNoiseGenerator* noise;
  BiquadFilter* filter;
  float freq;
  float decay;
  float snare;
  float balance;
  const float fs;
  float gain;
  float accent;
public:
  BassDrumVoice(float sr) : fs(sr), gain(0.0), accent(0.0) {
    snare = 0;
    balance = 0.2;
    sine = SineOscillator::create(sr);
    chirp = ChirpOscillator::create(sr);
    impulse = new ImpulseOscillator();
    env1 = ExponentialDecayEnvelope::create(sr);
    env2 = ExponentialDecayEnvelope::create(sr);
    noise = PinkNoiseGenerator::create();
    filter = BiquadFilter::create(sr, 1);
    filter->setLowPass(0.6, FilterStage::BUTTERWORTH_Q);
  }  
  BassDrumVoice(float sr, float freq, float snap) : fs(sr), gain(0.0), accent(0.0) {
    sine = SineOscillator::create(sr);
    chirp = ChirpOscillator::create(sr);
    impulse = new ImpulseOscillator();
    env1 = ExponentialDecayEnvelope::create(sr);
    env2 = ExponentialDecayEnvelope::create(sr);
    noise = PinkNoiseGenerator::create();
    filter = BiquadFilter::create(sr, 1);
    filter->setLowPass(0.6, FilterStage::BUTTERWORTH_Q);
    setFrequency(freq);
    setSnap(snap);
  }
  ~BassDrumVoice(){
    SineOscillator::destroy(sine);
    ChirpOscillator::destroy(chirp);
    delete impulse;
    ExponentialDecayEnvelope::destroy(env1);
    ExponentialDecayEnvelope::destroy(env2);
    PinkNoiseGenerator::destroy(noise);
    BiquadFilter::destroy(filter);
  }
  void setFrequency(float f){
    freq = f;
    // filter->setLowPass(f*2/(fs*2), FilterStage::BUTTERWORTH_Q);
  }
  float getFrequency(){
    return freq;
  }   
  void setPhase(float ph){
    sine->setPhase(ph);
  }
  float getPhase(){
    return sine->getPhase();
  }
  void reset(){
    sine->reset();
    chirp->reset();
  }
  void setDecay(float d){
    decay = d;
  }
  void setSnap(float s){
    snare = s;
    balance = s*0.5;
    filter->setLowPass(0.25+balance, FilterStage::BUTTERWORTH_Q);
  }
  void setAccent(float amount){
    accent = amount;
  }
  void trigger(bool state, int delay){
    sine->setFrequency(freq);
    chirp->setFrequency(freq*2);
    env1->setDecay(decay);
    env2->setDecay(decay*snare*0.5);
    chirp->setRate(-decay);
    env1->trigger();
    env2->trigger();
    chirp->trigger();
    impulse->trigger();
    gain = accent * 0.6 + 0.7;
  }
  using Oscillator::generate;
  float generate(float fm){
    return generate();
  }
  float generate(){
    float vca1 = sine->generate();
    vca1 += chirp->generate();
    vca1 *= env1->generate();
    float vca2 = 0.0f;
    vca2 += impulse->generate();
    // vca2 += filter->process(noise->generate());
    // vca2 *= env2->generate();
    vca2 += noise->generate();
    vca2 = filter->process(vca2);
    vca2 *= env2->generate();    
    float sample = vca1*(1.0-balance) + vca2*balance;
    return sample * gain;
  }
};

#endif // __BassDrum_hpp__
