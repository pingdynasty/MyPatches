#ifndef __SineOscPatch_hpp__
#define __SineOscPatch_hpp__

#include "StompBox.h"

/**
 * OWL Modular oscillator patch
 * tracks 1v/octave on left input channel (at block rate) 
 * produces a sine wave on the left output.
 * 0v dc input generates 440Hz output signal
 */
class SineOscPatch : public Patch {
private:
  float pos = 0.0f;
  const float VOLTAGE_MULTIPLIER = -4.40f;
  const float VOLTAGE_OFFSET = .028f;
  const float BASE_VOLTAGE = 0.0f;
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
public:
  void processAudio(AudioBuffer &buffer) {
    float* samples = buffer.getSamples(0);
    float volts = (samples[0] - VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
    float frequency = volts2hz(volts+BASE_VOLTAGE);
    float linc = frequency/getSampleRate();
    for(int n = 0; n<buffer.getSize(); n++){
      samples[n] = sinf(2*M_PI*pos);
      if((pos += linc) > 1.0f)
	pos -= 1.0f;
    }
  }
};

#endif   // __SineOscPatch_hpp__
