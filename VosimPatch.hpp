#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "StompBox.h"
#include "SineOscillator.h"
#include "VoltsPerOctave.h"

class VosimOscillator : public Oscillator {
private:
  const float fs;
  SineOscillator* formant1;
  SineOscillator* formant2;
  float phase;
  float incr;
public:
  VosimOscillator(float samplerate, SineOscillator* osc2, SineOscillator* osc1) :
    fs(samplerate), formant1(osc1) , formant2(osc2) {}
  void setFrequency(float freq){
    incr = freq/fs;
  }
  void reset(){
    phase = 0.0f;
  }
  void setPhase(float ph){
    phase = 1-ph/(2*M_PI);
  }
  float getPhase(){
    return (1-phase)*2*M_PI;
  }
  void setFormant1(float frequency){
    formant1->setFrequency(frequency);
  }
  void setFormant2(float frequency){
    formant2->setFrequency(frequency);
  }
  using Oscillator::generate;
  float generate(float fm){
    phase -= incr+fm;
    if(phase <= 0.0){
      phase += 1.0;
      formant1->reset();
      formant2->reset();
    }
    float s1 = formant1->generate();
    float s2 = formant2->generate();
    return (s1*s1 + s2*s2)*phase*0.5;
  }
  float generate(){
    phase -= incr;
    if(phase <= 0.0){
      phase += 1.0;
      formant1->reset();
      formant2->reset();
    }
    float s1 = formant1->generate();
    float s2 = formant2->generate();
    return (s1*s1 + s2*s2)*phase*0.5;
  }
  static VosimOscillator* create(float sr){
    SineOscillator* f1 = SineOscillator::create(sr);
    SineOscillator* f2 = SineOscillator::create(sr);
    return new VosimOscillator(sr, f1, f2);
  }
  static void destroy(VosimOscillator* obj){
    SineOscillator::destroy(obj->formant1);
    SineOscillator::destroy(obj->formant2);
    delete obj;
  }
};

class VosimPatch : public Patch {
  SineOscillator* sine1;
  SineOscillator* sine2;
  VosimOscillator* osc;
  VoltsPerOctave hz;
public:
  VosimPatch(){
    osc = VosimOscillator::create(getSampleRate());
    registerParameter(PARAMETER_A, "Fundamental");
    registerParameter(PARAMETER_B, "Formant");
    registerParameter(PARAMETER_C, "Detune");
    registerParameter(PARAMETER_D, "Gain");
  }
  ~VosimPatch(){
    VosimOscillator::destroy(osc);
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*10.0 - 6.0;
    float f1 = getParameterValue(PARAMETER_B)*8.0 - 4.0;
    float f2 = getParameterValue(PARAMETER_C)+1.0;
    float gain = getParameterValue(PARAMETER_D)*2;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(LEFT_CHANNEL);
    hz.setTune(tune);
    float lfreq = hz.getFrequency(left[0]);
    osc->setFrequency(lfreq);
    hz.setTune(f1);
    float rfreq = hz.getFrequency(right[0]);
    osc->setFormant1(rfreq);
    osc->setFormant2(rfreq*f2);
    osc->generate(left);
    left.multiply(gain);
    right.copyFrom(left);
  }
};

#endif   // __VosimPatch_hpp__
