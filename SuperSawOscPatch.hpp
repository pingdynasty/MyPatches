#ifndef __SuperSawOscPatch_hpp__
#define __SuperSawOscPatch_hpp__

#include "StompBox.h"

class SuperSawOscPatch : public Patch {
  SuperSaw saw;
private:
  const float VOLTAGE_MULTIPLIER = -4.40f;
  const float VOLTAGE_OFFSET = -0.0585f;
  const float div;
  float pos;
  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
public:
  SuperSawOscPatch(): div(getSampleRate()) {
    saw.setSampleRate(getSampleRate());
    registerParameter(PARAMETER_A, "gain");
  }
  void processAudio(AudioBuffer &buffer){
    float tune = getParameterValue(PARAMETER_A)*6.0 - 3.0;
    float detune = getParameterValue(PARAMETER_B);
    float mix = getParameterValue(PARAMETER_C);
    float gain = getParameterValue(PARAMETER_D);
    gain = gain*gain*1.2;
    gain *= 0.2+mix*0.8;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    saw.setMix(mix);
    saw.setDetune(detune);
    float volts = sample2volts(left[0]);
    float frequency = volts2hz(volts+tune);
    saw.setFrequency(frequency);
      //    for(int i=0; i<buffer.getSize(); ++i){
      //      saw.setFrequency(frequency);
      //      saw.updatePhaseIncrements();
      //      left[i] = saw.getNextSample();
      //    }
    saw.getSamples(left);
    float amp = right[0] - VOLTAGE_OFFSET;
    amp = gain + amp*amp*0.5;
    left.scale(amp);
  }
};

#endif // __SuperSawOscPatch_hpp__
