#ifndef __SynthVoice_hpp__
#define __SynthVoice_hpp__

#include "Envelope.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"

class SynthVoice {
private:
  PolyBlepOscillator osc;
  BiquadFilter* filter;
  LinearAdsrEnvelope env;
  SmoothFloat fc, q;
  float gain;
public:
  SynthVoice(float sr, BiquadFilter* f) : 
    osc(sr), filter(f), env(sr), fc(0.25), q(0.77), gain(1.0f) {    
    env.setSustain(1.0);
    env.setDecay(0.0);
    env.setRelease(0.0);
  }
  static SynthVoice* create(float sr){
    // 8-pole filter 48dB
    return new SynthVoice(sr, BiquadFilter::create(4));
  }
  static void destroy(SynthVoice* voice){
    BiquadFilter::destroy(voice->filter);
    delete voice;
  }
  void setFrequency(float freq){
    osc.setFrequency(freq);
  }
  void setFilter(float freq, float res){
    fc = min(0.45f, max(0.05f, freq));
    fc = freq;
    q = res;
  }
  void setWaveshape(float shape){
    float pw = 0.5f;
    if(shape > 1.0f){
      pw += 0.49f*(shape-1.0f); // pw 0.5 to 0.99
      shape = 1.0f; // square wave
    }
    osc.setShape(shape);
    osc.setPulseWidth(pw);
  }
  void setEnvelope(float attack, float release){
    env.setAttack(attack);
    env.setRelease(release);    
  }
  void setGain(float value){
    gain = value;
  }
  void setGate(bool state, int delay){
    env.gate(state, delay);
  }
  void generate(FloatArray samples){
    filter->setLowPass(fc, q);
    osc.generate(samples);
    filter->process(samples);
    samples.multiply(gain*(0.8-q*0.2)); // gain compensation for high q
    env.process(samples, samples);
  }
  float getEnvelopeLevel(){
    return env.getLevel();
  }
};

#endif   // __SynthVoice_hpp__
