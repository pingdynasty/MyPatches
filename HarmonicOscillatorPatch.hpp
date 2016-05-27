#ifndef __HarmonicOscillatorPatch_hpp__
#define __HarmonicOscillatorPatch_hpp__

#include "StompBox.h"
#include "HarmonicOscillator.hpp"
#include "WavetableOscillator.h"

class HarmonicOscillatorPatch : public Patch {
private:
  static const int samples = 512;
  static const int controls = 4;
  HarmonicOscillator<samples, controls> harms;
  WavetableOscillator* osc;
  FloatArray wavetable;
  int count;
public:
HarmonicOscillatorPatch(){
    wavetable = FloatArray::create(samples);
    osc = new WavetableOscillator(getSampleRate(), wavetable);
    osc->setFrequency(440);
    for(int i=0; i<controls; ++i)
      harms.setControl(i, 0.5*127);
    harms.calculate(wavetable, samples);    
  }
  void processAudio(AudioBuffer &buffer) {
    float a = getParameterValue(PARAMETER_A);
    float b = getParameterValue(PARAMETER_B);
    float c = getParameterValue(PARAMETER_C);
    float d = getParameterValue(PARAMETER_D);
    // if(++count > a*5000){
    //   count = 0;
      harms.setControl(0, a*127);
      harms.setControl(1, b*127);
      harms.setControl(2, c*127);
      harms.setControl(3, d*127);
      harms.calculate(wavetable, samples);
    // }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    osc->setFrequency(d*2000);
    osc->getSamples(left);
  }
};

#endif   // __HarmonicOscillatorPatch_hpp__
