#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "StompBox.h"
#include "Oscillators.hpp"
#include "PolyBlepOscillator.h"
#include "VoltsPerOctave.h"

class VosimOscillator : public Oscillator {
private:
  const float fs;
  SineOscillator* formant;
  // PolyBlepOscillator saw;
  float phase;
  float incr;
public:
  VosimOscillator(float samplerate, SineOscillator* osc) :
    fs(samplerate), formant(osc) {}
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
      formant->reset();
    }
    float s = formant->getNextSample();
    return s*s*phase;
  }
};

class VosimPatch : public Patch {
  SineOscillator* sine;
  VosimOscillator* osc;
  VoltsPerOctave hz;
public:
  VosimPatch(){
    sine = new SineOscillator(getSampleRate());
    osc = new VosimOscillator(getSampleRate(), sine);
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Formant");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*10.0 - 6.0;
    float formant = getParameterValue(PARAMETER_B)*10.0 - 6.0;
    float gain = getParameterValue(PARAMETER_D)*2;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(LEFT_CHANNEL);
    hz.setTune(tune);
    float lfreq = hz.getFrequency(left[0]);
    osc->setFrequency(lfreq);
    hz.setTune(formant);
    float rfreq = hz.getFrequency(right[0]);
    sine->setFrequency(rfreq);
    osc->getSamples(left);
    left.multiply(gain);
  }
};

#endif   // __VosimPatch_hpp__
