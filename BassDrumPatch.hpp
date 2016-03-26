#ifndef __BassDrumPatch_hpp__
#define __BassDrumPatch_hpp__

#include "StompBox.h"
#include "BiquadFilter.h"
#include "Envelope.h"
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

class ChirpOscillator : public Oscillator {
private:
  const float fs;
  float phase;
public:
  float incr;
  ChirpOscillator(float sr)
    : fs(sr), phase(0.0f), incr(1.0f){}
  void setAscending(float freq){
    incr = 1+freq*2*M_PI/fs;
  }
  void setDescending(float freq){
    incr = 1+freq*2*M_PI/fs;
  }
  float getNextSample(){
    float sample = sinf(phase);
    phase *= incr;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }
};

class ExponentialDecayEnvelope {
private:
  float value;
  float incr;
public:
  void setDecay(float s){
    incr = s;
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
  // SineOscillator* sine;
  ChirpOscillator* chirp;
  ExponentialDecayEnvelope* env;
  float freq;
  int pos;
public:
  DrumVoice(float sr){
    // env = new AdsrEnvelope(sr);
    // env = FloatArray::create(1024);
    // for(int i=0; i<env.getSize(); ++i)
    //   env[i] = expf(-M_E*i/env.getSize());
    // sine = new SineOscillator(sr);
    chirp = new ChirpOscillator(sr);
    env = new ExponentialDecayEnvelope();
  }  
  void setFrequency(float f){
    freq = f;
  }
  void setDecay(float d){
    env->setDecay(d);
  }
  void trigger(){
    pos = 0;
    env->trigger();
    chirp->setDescending(freq);
  }
  float getNextSample(){
    return chirp->getNextSample()*env->getNextSample();
  }
};

class BassDrumPatch : public Patch {
private:
  DrumVoice* kick;
  bool buttonstate = false;
public:
  BassDrumPatch(){
    registerParameter(PARAMETER_A, "Tone");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "");
    registerParameter(PARAMETER_D, "Level");
    kick = new DrumVoice(getSampleRate());
  }
  ~BassDrumPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float a = getParameterValue(PARAMETER_A);
    float b = getParameterValue(PARAMETER_B);
    float c = getParameterValue(PARAMETER_C);
    float d = getParameterValue(PARAMETER_D)*2;
    kick->setDecay(b*0.2+0.79);
    kick->setFrequency(a*4000+10);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    if(isButtonPressed(PUSHBUTTON) != buttonstate){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate)
	kick->trigger();
    }
    kick->getSamples(left);
    left.multiply(d);
  }
};

#endif   // __BassDrumPatch_hpp__
