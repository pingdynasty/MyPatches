#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "StompBox.h"
#include "Oscillators.hpp"
#include "PolyBlepOscillator.h"
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
  float getNextSample(){
    phase -= incr;
    if(phase <= 0.0){
      phase += 1.0;
      formant1->reset();
      formant2->reset();
    }
    float s1 = formant1->getNextSample();
    float s2 = formant2->getNextSample();
    return (s1*s1 + s2*s2)*phase*0.5;
  }
};

class VosimPatch : public Patch {
  SineOscillator* sine1;
  SineOscillator* sine2;
  VosimOscillator* osc;
  VoltsPerOctave hz;
public:
  VosimPatch(){
    sine1 = new SineOscillator(getSampleRate());
    sine2 = new SineOscillator(getSampleRate());
    osc = new VosimOscillator(getSampleRate(), sine1, sine2);
    registerParameter(PARAMETER_A, "Fundamental");
    registerParameter(PARAMETER_B, "Formant");
    registerParameter(PARAMETER_B, "Offset");
    registerParameter(PARAMETER_D, "Gain");
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
    sine1->setFrequency(rfreq);
    sine2->setFrequency(rfreq*f2);
    osc->getSamples(left);
    left.multiply(gain);
    right.copyFrom(left);
  }
};

#endif   // __VosimPatch_hpp__
