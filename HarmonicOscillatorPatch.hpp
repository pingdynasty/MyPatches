#ifndef __HarmonicOscillatorPatch_hpp__
#define __HarmonicOscillatorPatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"
#include "HarmonicOscillator.hpp"
#include "WavetableOscillator.h"
#include "BiquadFilter.h"
#include "Window.h"

class HarmonicOscillatorPatch : public Patch {
private:
  static const int samples = 512;
  static const int controls = 5;
  HarmonicOscillator<samples, controls> harms;
  WavetableOscillator* osc;
  FloatArray wavetable;
  VoltsPerOctave hz;
public:
  HarmonicOscillatorPatch(){
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Rate");
    registerParameter(PARAMETER_C, "H2");
    registerParameter(PARAMETER_D, "H3");
    registerParameter(PARAMETER_E, "H4");
    wavetable = FloatArray::create(samples);
    osc = new WavetableOscillator(getSampleRate(), wavetable);
    osc->setFrequency(440);
    harms.setGlauberState(0.5);
    harms.calculate(wavetable, samples);
  }
  ~HarmonicOscillatorPatch(){
    delete osc;
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*6.0 - 4.0;
    float b = getParameterValue(PARAMETER_B);
    float c = getParameterValue(PARAMETER_C);
    float d = getParameterValue(PARAMETER_D);
    float e = getParameterValue(PARAMETER_E);
    harms.setTimeStep(b*b);
    harms.setControl(1, c);
    harms.setControl(2, d);
    harms.setControl(3, e);
    harms.normalizeAmplitudes();
    harms.calculateNormalized(wavetable, samples);
    harms.increment();
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune);
    float freq = hz.getFrequency(left[0]);
    osc->setFrequency(freq);
    osc->generate(left);
    right.copyFrom(left);
  }
};

#endif   // __HarmonicOscillatorPatch_hpp__
