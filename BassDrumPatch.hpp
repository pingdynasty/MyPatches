#ifndef __BassDrumPatch_hpp__
#define __BassDrumPatch_hpp__

#include "StompBox.h"
#include "BiquadFilter.h"
#include "Envelope.h"
#include "Oscillator.h"
#include "NoiseOscillator.h"

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
      rate = 1.0f - (1/(1-r))/fs;
    else
      rate = 1.0f + r/fs;
  }
  void setDecay(float d){
    setRate(-1.0f/d);
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

class ExponentialDecayEnvelope {
private:
  float value;
  float incr;
public:
  void setDecay(float s){
    incr = s*0.005+0.995;
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
  
class DrumVoice : public Oscillator {
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
  float fs;
public:
  DrumVoice(float sr){
    fs = sr;
    // env = new AdsrEnvelope(sr);
    // env = FloatArray::create(1024);
    // for(int i=0; i<env.getSize(); ++i)
    //   env[i] = expf(-M_E*i/env.getSize());
    sine = new SineOscillator(sr);
    chirp = new ChirpOscillator(sr);
    impulse = new ImpulseOscillator();
    env1 = new ExponentialDecayEnvelope();
    env2 = new ExponentialDecayEnvelope();
    noise = new PinkNoiseOscillator();
    filter = BiquadFilter::create(1);
  }  
  void setFrequency(float f){
    freq = f;
    filter->setLowPass(f*2/(fs*2), FilterStage::BUTTERWORTH_Q);
  }
  void setDecay(float d){
    decay = d;
  }
  void trigger(){
    sine->setFrequency(freq);
    chirp->setFrequency(freq);
    env1->setDecay(decay);
    env2->setDecay(decay/2);
    chirp->setDecay(decay);
    env1->trigger();
    env2->trigger();
    chirp->trigger();
    impulse->trigger();
  }
  float balance;
  float getNextSample(){
    float vca1 = sine->getNextSample();
    vca1 += chirp->getNextSample();
    vca1 *= env1->getNextSample();

    float vca2 = impulse->getNextSample();
    vca2 += filter->process(noise->getNextSample());
    vca2 *= env2->getNextSample();

    float sample = vca1*balance + vca2*(1.0-balance);
    return sample;
  }
};

class BassDrumPatch : public Patch {
private:
  DrumVoice* kick;
  bool buttonstate = false;
  ChirpOscillator* chirp;
public:
  BassDrumPatch(){
    registerParameter(PARAMETER_A, "Tone");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "");
    registerParameter(PARAMETER_D, "Level");
    kick = new DrumVoice(getSampleRate());
    chirp = new ChirpOscillator(getSampleRate());
  }
  ~BassDrumPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float a = getParameterValue(PARAMETER_A);
    float b = getParameterValue(PARAMETER_B);
    float c = getParameterValue(PARAMETER_C);
    float d = getParameterValue(PARAMETER_D)*2;
    float e = getParameterValue(PARAMETER_E);
    kick->setDecay(b);
    kick->setFrequency(a*600+10);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    kick->balance = c;
    if(isButtonPressed(PUSHBUTTON) != buttonstate){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate){
	kick->trigger();
	chirp->setDecay(b);
	chirp->setFrequency(a*2000);
	chirp->trigger();
      }
    }
    kick->getSamples(left);
    left.multiply(d);
    chirp->getSamples(right);
    right.multiply(e);
  }
};

#endif   // __BassDrumPatch_hpp__
