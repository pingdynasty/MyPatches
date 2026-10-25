#ifndef __StateVariableFilterPatch_hpp__
#define __StateVariableFilterPatch_hpp__

#include "Patch.h"
#include "StateVariableFilter.h"

class StateVariableFilterPatch : public Patch {
private:
  MultiStateVariableFilter* filter;
public:
  StateVariableFilterPatch(){
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Resonance");
    registerParameter(PARAMETER_C, "Gain");
    registerParameter(PARAMETER_D, "Mode");
    filter = MultiStateVariableFilter::create(getSampleRate(), getNumberOfChannels());
  }
  ~StateVariableFilterPatch(){
    MultiStateVariableFilter::destroy(filter);
  }
  void processAudio(AudioBuffer &buffer){
    float fc = getParameterValue(PARAMETER_A)*12000+20;
    float q = getParameterValue(PARAMETER_B)*8+0.1;
    float gain = getParameterValue(PARAMETER_C)*60 - 30;
    int mode = getParameterValue(PARAMETER_D)*5;
    switch(mode){
    case 0:
      filter->setLowPass(fc, q);
      break;
    case 1:
      filter->setHighShelf(fc, q, gain);
      break;
    case 2:
      filter->setBell(fc, q, gain); // or setBandPass(fc, q);
      break;
    case 3:
      filter->setLowShelf(fc, q, gain);
      break;
    case 4:
      filter->setHighPass(fc, q);
      break;
    }
    filter->process(buffer, buffer);
  }
};

#endif // __StateVariableFilterPatch_hpp__
