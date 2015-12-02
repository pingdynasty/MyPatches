#ifndef __TrackerPatch_hpp__
#define __TrackerPatch_hpp__

#include "StompBox.h"
#include "PitchDetector.hpp"
#include "SmoothFloat.hpp"
#include "WavetableOscillator.h"

/**
 */
class TrackerPatch : public Patch {
private:
  WavetableOscillator osc;
  ZeroCrossingPitchDetector zcc;
  SmoothFloat freq;
  SmoothFloat gain;
public:
  TrackerPatch() :
    osc(getSampleRate()), 
    zcc(getSampleRate(), getBlockSize()) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_C, "Portamento");
    registerParameter(PARAMETER_D, "Dry/Wet");
    zcc.setLowPassCutoff(40);
    zcc.setHighPassCutoff(8000);
    freq.resetValue(440.0);
    freq.lambda = 0.9;
    gain.lambda = 0.9;
  }
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    zcc.process(left);
    float tune = getParameterValue(PARAMETER_A)*1.5+0.5; // up/down 1 octave
    float portamento = getParameterValue(PARAMETER_C)*0.1+0.9;
    freq.lambda = portamento;
    freq = zcc.getFrequency()*tune;
    float rms = left.getRms();
    if(rms > 0.1)
      gain = rms*1.66;
    else
      gain = 0.0;
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    debugMessage("freq/gain: ", freq, gain);
    //    if(gain > 0.1 && freq > 20.00 && freq < 12000.0)
    osc.setFrequency(freq);
    for(int n = 0; n<buffer.getSize(); n++){
      float s = gain*osc.getNextSample();
      left[n] = left[n]*dry + s*wet;
      //      right[n] = left[n];
    }
  }
};

#endif   // __TrackerPatch_hpp__
