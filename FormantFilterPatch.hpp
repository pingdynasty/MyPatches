#ifndef __FormantFilterPatch_hpp__
#define __FormantFilterPatch_hpp__

#include "StompBox.h"
#include "BiquadFilter.h"
#include <math.h>

class FormantFilterPatch : public Patch {
private:
  StereoBiquadFilter* filter1;
  StereoBiquadFilter* filter2;
  StereoBiquadFilter* filter3;
  StereoBiquadFilter* filter4;
  const float qmul;
  const float minf, maxf;
  const float lp;
public:
  FormantFilterPatch() 
    // Cutoffs in Hz. These filters used to be created with create(1), i.e. a
    // sample rate of 1, so the old v/(sr/2) values meant v*2 Hz: 200/(sr/2) was
    // 400 Hz, and q = f*(sr/2)/80 with f in units of sr is q = f_hz/160.
    : qmul(1.0f/160),
      minf(400),
      maxf(2800),
      lp(14000) {
    registerParameter(PARAMETER_A, "Formant 1");
    registerParameter(PARAMETER_B, "Formant 2");
    registerParameter(PARAMETER_C, "Formant 3");
    registerParameter(PARAMETER_D, "Gain");
    filter1 = StereoBiquadFilter::create(getSampleRate(), 1);
    filter2 = StereoBiquadFilter::create(getSampleRate(), 1);
    filter3 = StereoBiquadFilter::create(getSampleRate(), 1);
    filter4 = StereoBiquadFilter::create(getSampleRate(), 1);
  }    

  ~FormantFilterPatch(){
    StereoBiquadFilter::destroy(filter1);
    StereoBiquadFilter::destroy(filter2);
    StereoBiquadFilter::destroy(filter3);
    StereoBiquadFilter::destroy(filter4);
  }

  void processAudio(AudioBuffer &buffer){
    float f1 = getParameterValue(PARAMETER_A)*(maxf-minf)+minf;
    float f2 = getParameterValue(PARAMETER_B)*(maxf-minf)+minf;
    float f3 = getParameterValue(PARAMETER_C)*(maxf-minf)+minf;
    float gain = getParameterValue(PARAMETER_D)*2;
    filter1->setBandPass(f1, f1*qmul);
    filter2->setBandPass(f2, f2*qmul);
    filter3->setBandPass(f3, f3*qmul);
    filter4->setLowShelf(lp, gain);
    filter1->process(buffer, buffer);
    filter2->process(buffer, buffer);
    filter3->process(buffer, buffer);
    filter4->process(buffer, buffer);
  }
};

#endif // __FormantFilterPatch_hpp__
