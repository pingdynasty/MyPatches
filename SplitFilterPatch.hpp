#ifndef __SplitFilterPatch_hpp__
#define __SplitFilterPatch_hpp__

#include "StompBox.h"

class SplitFilterPatch : public Patch {
private:
  BiquadFilter* leftFilter;
  BiquadFilter* rightFilter;
public:
  SplitFilterPatch(){
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Resonance");
    registerParameter(PARAMETER_C, "Spread");
    registerParameter(PARAMETER_D, "Gain");
    leftFilter = BiquadFilter::create(1);
    rightFilter = BiquadFilter::create(1);
  }
  void processAudio(AudioBuffer &buffer){
    float fc = getParameterValue(PARAMETER_A);
    // fc = 0.25*powf(2, fc);
    float q = getParameterValue(PARAMETER_B);
    float spread = getParameterValue(PARAMETER_C)*0.004 - 0.002;
    spread = 0;
    float gain = getParameterValue(PARAMETER_D)*2.0;
    leftFilter->setHighPass(fc+spread, q);
    rightFilter->setLowPass(fc-spread, q);
    int size = buffer.getSize();
    float buf[size];
    FloatArray left = buffer.getSamples(0);
    FloatArray right = buffer.getSamples(1);
    leftFilter->process(left, buf, size);
    left.copyFrom(buf, size);
    rightFilter->process(right, buf, size);
    right.copyFrom(buf, size);
    left.scale(gain);
    right.scale(gain);
  }
};

#endif // __SplitFilterPatch_hpp__
