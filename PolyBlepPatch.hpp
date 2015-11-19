#ifndef __PolyBlepPatch_hpp__
#define __PolyBlepPatch_hpp__

#include "StompBox.h"
#include "oscillator.h"
using namespace stmlib;

class PolyBlepPatch : public Patch {
private:
  const float div;
  const float VOLTAGE_MULTIPLIER = -4.40f;
  // const float VOLTAGE_OFFSET = 0.028f;
  const float VOLTAGE_OFFSET = -0.0585f;
  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
  Oscillator osc;
public:
  PolyBlepPatch(): div(getSampleRate()*64) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Shape");
    registerParameter(PARAMETER_C, "PW");
    registerParameter(PARAMETER_D, "Gain");
    osc.Init();
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*2.0 - 1.0;
    float shape = getParameterValue(PARAMETER_B);
    float pw = getParameterValue(PARAMETER_C)*0.8 + 0.1; // polyblep doesn't like full-range pwm
    float gain = getParameterValue(PARAMETER_D)*2;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    float freq = (tune * 100 + 100.00f) / getSampleRate();
    // float frequency = 4*volts2hz(sample2volts(left[0]) + tune) / getSampleRate();
    float frequency = (tune * 4000 + 4020)/getSampleRate();
    osc.Render<true>(frequency, pw, shape, left, size);
    // osc.Render<true>(tune, // master
    // 		     tune * detune,  // slave
    // 		     pw,  // pulse-width
    // 		     shape,  // shape: 0.0 for saw, 1.0 for square
    // 		     left, size);
    left.multiply(gain);
    right.copyFrom(left);
  }
};

#endif   // __PolyBlepPatch_hpp__
