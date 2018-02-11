#ifndef __SimpleSinePatch_hpp__
#define __SimpleSinePatch_hpp__

#include "StompBox.h"

/**
 * OWL Modular oscillator patch
 * tracks 1v/octave on left input channel (at block rate) 
 * produces a sine wave on the left output.
 * 0v dc input generates 440Hz output signal
 */
class SimpleSinePatch : public Patch {
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
  SimpleSinePatch(): div(getSampleRate()), pos(0){}
  void processAudio(AudioBuffer &buffer) {
    FloatArray samples = buffer.getSamples(LEFT_CHANNEL);
    float gain = isButtonPressed(PUSHBUTTON) ? 1.0 : 0.0;
    for(int n = 0; n<buffer.getSize(); n++){
      float frequency = volts2hz(sample2volts(samples[n]));
      float linc = frequency/div;
      samples[n] = gain * sinf(2*M_PI*pos);
      if((pos += linc) > 1.0f)
	pos -= 1.0f;
    }
  }
};

#endif   // __SimpleSinePatch_hpp__
