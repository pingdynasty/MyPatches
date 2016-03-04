#ifndef __SuperSawOscPatch_hpp__
#define __SuperSawOscPatch_hpp__

#include "StompBox.h"
#include "SuperSaw.hpp"
#include "VoltsPerOctave.h"

class SuperSawOscPatch : public Patch {
private:
  SuperSaw saw;
  VoltsPerOctave hz;
public:
  SuperSawOscPatch() {
    saw.setSampleRate(getSampleRate());
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Detune");
    registerParameter(PARAMETER_C, "Mix");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer){
    float tune = getParameterValue(PARAMETER_A)*8.0 - 4.0;
    float detune = getParameterValue(PARAMETER_B);
    float mix = getParameterValue(PARAMETER_C);
    float gain = getParameterValue(PARAMETER_D);
    gain = gain*gain*1.2;
    gain *= 0.2+mix*0.8;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    saw.setMix(mix);
    saw.setDetune(detune);
    hz.setTune(tune);
    float frequency = hz.getFrequency(left[0]);
    saw.setFrequency(frequency);
    saw.getSamples(left);
    for(int i=0; i<buffer.getSize(); ++i){
      float amp = hz.sampleToVolts(right[i]);
      amp = gain + amp*amp*0.5;
      left[i] *= amp;
      right[i] = -left[i];
    }
  }
};

#endif // __SuperSawOscPatch_hpp__
