#ifndef __BuzzPatch_hpp__
#define __BuzzPatch_hpp__

#include "Patch.h"
#include "PolyBlepOscillator.h"

class BuzzPatch : public Patch {
private:
  PolyBlepOscillator osc1, osc2;
public:
  BuzzPatch(){
    registerParameter(PARAMETER_A, "Freq 1");
    registerParameter(PARAMETER_B, "Wave 1");
    registerParameter(PARAMETER_C, "Freq 2");
    registerParameter(PARAMETER_D, "Wave 2");
    osc1.setSampleRate(getSampleRate());
    osc2.setSampleRate(getSampleRate());
  }
  void processAudio(AudioBuffer &buffer) {
    float freq1 = getParameterValue(PARAMETER_A)*(250-108)+108;
    float shape1 = getParameterValue(PARAMETER_B)*1.8;
    float freq2 = getParameterValue(PARAMETER_C)*(250-108)+108;
    float shape2 = getParameterValue(PARAMETER_D)*1.8;
    osc1.setFrequency(freq1);
    osc2.setFrequency(freq2);
    if(shape1 > 1.0){
      osc1.setShape(1.0);
      osc1.setPulseWidth(shape1/2.0);
    }else{
      osc1.setShape(shape1);
    }
    if(shape2 > 1.0){
      osc2.setShape(1.0);
      osc2.setPulseWidth(shape2/2.0);
    }else{
      osc2.setShape(shape2);
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    osc1.getSamples(left);
    osc2.getSamples(right);
  }
};

#endif   // __BuzzPatch_hpp__
