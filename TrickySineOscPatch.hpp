#ifndef __TrickySineOscPatch_hpp__
#define __TrickySineOscPatch_hpp__

#include "StompBox.h"

/*
 * Digitally controlled non-linear oscillator based on the function
 * sin(x)^2 + sin(x^2), x=-5.48 to 5.48
 * Left input: pitch
 * Right input: amplitude
 * Left output: tricky wave
 */
class TrickySineOscPatch : public Patch {
private:
  const float divisor;
  float pos = 0.0f;
  const float VOLTAGE_MULTIPLIER = -4.40f;
  // const float VOLTAGE_OFFSET = 0.028f;
  const float VOLTAGE_OFFSET = -0.0585f;
  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
  float wave(float x){
    x *= 5.48;
    float sx = sinf(x);
    return sx*sx+sinf(x*x);
  }
public:
  TrickySineOscPatch() : divisor(getSampleRate()*16) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Octave");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*2.0 - 1.0;
    float octave = round(getParameterValue(PARAMETER_B)*16)+1;
    float gain = getParameterValue(PARAMETER_D);
    gain = gain*gain*0.8;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    for(int n = 0; n<buffer.getSize(); n++){
      float frequency = volts2hz(sample2volts(left[n])+tune);
      frequency *= octave;
      float linc = frequency/divisor;
      float amp = right[n] - VOLTAGE_OFFSET;
      amp = gain + amp*amp*0.5;
      left[n] = amp*wave(pos);
      if((pos += linc) > 1.0f)
	pos -= 2.0f;
    }
  }
};

#endif   // __TrickySineOscPatch_hpp__
