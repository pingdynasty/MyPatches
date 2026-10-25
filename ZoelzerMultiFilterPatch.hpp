#ifndef __ZoelzerMultiFilterPatch_hpp__
#define __ZoelzerMultiFilterPatch_hpp__

#include "Patch.h"
#include "BiquadFilter.h"
/**
http://www.musicdsp.org/showArchiveComment.php?ArchiveID=37
http://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/
Type : biquad IIR
References : Udo Zoelzer: Digital Audio Signal Processing (John Wiley & Sons, ISBN 0 471 97226 6), Chris Townsend, Nigel Redmon
*/

// #define ZOELZER_FILTER_STAGES         2 // 4-pole, 24dB filter
#define ZOELZER_FILTER_STAGES         4 // 8-pole, 48dB filter
#define ZOELZER_LOWPASS_FILTER_MODE   0
#define ZOELZER_HIGHSHELF_FILTER_MODE 1
#define ZOELZER_PEAK_FILTER_MODE      2
#define ZOELZER_LOWSHELF_FILTER_MODE  3
#define ZOELZER_HIGHPASS_FILTER_MODE  4
#define ZOELZER_NOTCH_FILTER_MODE     5
#define ZOELZER_BANDPASS_FILTER_MODE  6
#define ZOELZER_MODES                 5 // ignore notch and bandpass
class ZoelzerMultiFilterPatch : public Patch {
private:
  MultiBiquadFilter* filter;
public:
  ZoelzerMultiFilterPatch(){
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Resonance");
    registerParameter(PARAMETER_C, "Gain");
    registerParameter(PARAMETER_D, "Mode");
    filter = MultiBiquadFilter::create(getSampleRate(), getNumberOfChannels(), ZOELZER_FILTER_STAGES);
  }
  ~ZoelzerMultiFilterPatch(){
    MultiBiquadFilter::destroy(filter);
  }
  void processAudio(AudioBuffer &buffer){
    int mode = getParameterValue(PARAMETER_D)*ZOELZER_MODES;
    float fc = getParameterValue(PARAMETER_A)*12000+400; // exp control?
    float q = getParameterValue(PARAMETER_B)*4+0.1;
    float gain = getParameterValue(PARAMETER_C)*36-18
    switch(mode){
    case ZOELZER_LOWPASS_FILTER_MODE:
      filter->setLowPass(fc, q);
      break;
    case ZOELZER_HIGHPASS_FILTER_MODE:
      filter->setHighPass(fc, q);
      break;      
    case ZOELZER_BANDPASS_FILTER_MODE:
      filter->setBandPass(fc, q);
      break;      
    case ZOELZER_NOTCH_FILTER_MODE:
      filter->setNotch(fc, q);
      break;      
    case ZOELZER_PEAK_FILTER_MODE:
      filter->setPeak(fc, q, gain);
      break;      
    case ZOELZER_LOWSHELF_FILTER_MODE:
      filter->setLowShelf(fc, gain);
      break;
    case ZOELZER_HIGHSHELF_FILTER_MODE:
      filter->setHighShelf(fc, gain);
      break;
    }
    filter->process(buffer, buffer);
  }
};

#endif // __ZoelzerMultiFilterPatch_hpp__
