#ifndef __FormantFilterPatch_hpp__
#define __FormantFilterPatch_hpp__

#include "StompBox.h"
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
    : qmul((getSampleRate()/2)/80),
      minf(200/(getSampleRate()/2)),
      maxf(1400/(getSampleRate()/2)),
      lp(3500*2/(getSampleRate()/2)) {
    registerParameter(PARAMETER_A, "Formant 1");
    registerParameter(PARAMETER_B, "Formant 2");
    registerParameter(PARAMETER_C, "Formant 3");
    registerParameter(PARAMETER_D, "Gain");
    filter1 = StereoBiquadFilter::create(1);
    filter2 = StereoBiquadFilter::create(1);
    filter3 = StereoBiquadFilter::create(1);
    filter4 = StereoBiquadFilter::create(1);
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
    filter1->process(buffer);
    filter2->process(buffer);
    filter3->process(buffer);
    filter4->process(buffer);
  }
};

#endif // __FormantFilterPatch_hpp__
