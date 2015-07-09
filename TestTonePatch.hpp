#ifndef __TestTonePatch_hpp__
#define __TestTonePatch_hpp__

#include "StompBox.h"

/**
 * Simple test tone generator: Sine wave on the left output, saw on the right
 */
class TestTonePatch : public Patch {
private:
  float pos;
public:
  TestTonePatch() : pos(0.0f) {
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Amplitude");
  }
  void processAudio(AudioBuffer &buffer) {
    float frequency = getParameterValue(PARAMETER_A) * 10000;
    float amplitude = getParameterValue(PARAMETER_B);
    FloatArray left = buffer.getSamples(0);
    float linc = frequency/getSampleRate();
    int size = buffer.getSize();
    for(int n = 0; n<size; n++){
      left[n] = sinf(2*M_PI*pos) * amplitude;
      right[n] = pos * amplitude;
      if((pos += linc) > 1.0f)
	pos -= 1.0f;
    }
  }
};

#endif   // __TestTonePatch_hpp__
