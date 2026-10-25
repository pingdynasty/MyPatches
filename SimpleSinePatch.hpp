#ifndef __SimpleSinePatch_hpp__
#define __SimpleSinePatch_hpp__

#include "Patch.h"
#include "SmoothValue.h"

/**
 * OWL Modular oscillator patch
 * tracks 1v/octave on left input channel (at block rate) 
 * produces a sine wave on the left output.
 * 0v dc input generates 440Hz output signal
 */
class SimpleSinePatch : public Patch {
private:
  const float VOLTAGE_MULTIPLIER = -4.39f;
  const float VOLTAGE_OFFSET = -0.0589f;
  float pos;
  SmoothFloat freq;
  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
public:
  SimpleSinePatch(): pos(0){
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    debugMessage("min/mean/max", left.getMinValue(), left.getMean(), left.getMaxValue());
    freq = getParameterValue(PARAMETER_A)*5.5+0.01;
    float freq = getParameterValue(PARAMETER_A)*4-2;
    float gain = getParameterValue(PARAMETER_D)*2;
    float offset = 0.0;
    float frequency = 10*freq; // volts2hz(sample2volts(left[n])+freq);
    frequency = 1000;
    gain = 1.0;
    float linc = frequency/(getSampleRate()/2);
    // float linc = frequency/48000;
    for(int n = 0; n<buffer.getSize(); n++){
      left[n] = gain * sinf(M_PI*pos) + offset;
      right[n] = gain * sinf(M_PI*pos) + offset;
      if((pos += linc) >= 1.0f)
    	pos -= 2.0f;
    }
	
    // static float out1 = 0;
    // static float out2 = 0;
    // if(out1 > 1.0)
    //   out1 = 0;
    // out1 += 2 * getBlockSize() / getSampleRate();
    // if(out2 > 1.0)
    //   out2 = -1;
    // out2 += 0.5 * getBlockSize() / getSampleRate();
    // setParameterValue(PARAMETER_AA, out1);
    // setParameterValue(PARAMETER_AB, 1-out1);
    // setParameterValue(PARAMETER_AC, out2);
    // setParameterValue(PARAMETER_AD, 1-out2);
    // debugMessage("lfo", out1, out2);
}
};

#endif   // __SimpleSinePatch_hpp__
