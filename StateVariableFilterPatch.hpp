#ifndef __StateVariableFilterPatch_hpp__
#define __StateVariableFilterPatch_hpp__

#include "Patch.h"
#include "StateVariableFilter.h"

class StateVariableFilterPatch : public Patch {
private:
  StateVariableFilter filter;
public:
  StateVariableFilterPatch(){
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Resonance");
    registerParameter(PARAMETER_C, "Gain");
    registerParameter(PARAMETER_D, "Mode");
  }
  void processAudio(AudioBuffer &buffer){
    float fc = getParameterValue(PARAMETER_A)*0.5;
    float q = getParameterValue(PARAMETER_B)*8+0.1;
    float gain = getParameterValue(PARAMETER_C)*60 - 30;
    int mode = getParameterValue(PARAMETER_D)*5;
    switch(mode){
    case 0:
      filter.setLowShelf(fc, q, gain);
      break;
    case 1:
      filter.setLowPass(fc, q);
      break;
    case 2:
      filter.setBandPass(fc, q);
      break;
    case 3:
      filter.setHighPass(fc, q);
      break;
    case 4:
      filter.setHighShelf(fc, q, gain);
      break;
    // case NOTCH_FILTER_MODE:
    //   filter.setNotch(fc, q);
    //   break;
    // case PEAK_FILTER_MODE:
    //   filter.setPeak(fc, q, gain);
    //   break;
    }
    FloatArray samples = buffer.getSamples(0);
    filter.process(samples, samples);
  }
};

#endif // __StateVariableFilterPatch_hpp__
