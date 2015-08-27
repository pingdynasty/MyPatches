#ifndef __SineOscPatch_hpp__
#define __SineOscPatch_hpp__

#include "StompBox.h"

/**
 * OWL Modular oscillator patch.
 * Tracks 1v/octave on left input channel.
 * Produces a sine wave on the left and right outputs.
 * 0v dc input generates 440Hz output signal
 * Left input: pitch
 * Right input: amplitude
 * Left/right output: tricky wave
 */
class SineOscPatch : public Patch {
private:
  const float div;
  float pos;
  const float VOLTAGE_MULTIPLIER = -4.40f;
  // const float VOLTAGE_OFFSET = 0.028f;
  const float VOLTAGE_OFFSET = -0.0585f;
  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
public:
  SineOscPatch(): div(getSampleRate()*64), pos(0){
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Octave");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*2.0 - 1.0;
    int octave = round(getParameterValue(PARAMETER_B)*10);
    octave = 1<<octave;
    float gain = getParameterValue(PARAMETER_D);
    gain = gain*gain;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    for(int n = 0; n<buffer.getSize(); n++){
      float volts = sample2volts(left[n]);
      float frequency = volts2hz(volts+tune);
      frequency *= octave;
      float linc = frequency/div;
      float amp = right[n] - VOLTAGE_OFFSET;
      amp = gain + amp*amp*0.5;
      left[n] = amp*sinf(2*M_PI*pos);
      if((pos += linc) > 1.0f)
	pos -= 1.0f;
      right[n] = left[n];
    }
  }
};

#endif   // __SineOscPatch_hpp__
