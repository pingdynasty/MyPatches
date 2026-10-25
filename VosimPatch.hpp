#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "OpenWareLibrary.h"

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
