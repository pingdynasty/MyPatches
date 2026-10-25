#ifndef __TestTonePatch_hpp__
#define __TestTonePatch_hpp__

#include "Patch.h"
#include "SineOscillator.h"

/**
 * Simple test tone generator: Sine wave on the left and right output
 */
class TestTonePatch : public Patch {
private:
  SineOscillator* osc[2];
public:
  TestTonePatch() {
    registerParameter(PARAMETER_A, "Freq L");
    registerParameter(PARAMETER_B, "Ampl L");
    registerParameter(PARAMETER_C, "Freq R");
    registerParameter(PARAMETER_D, "Ampl R");
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.8);
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0.8);
    osc[0] = SineOscillator::create(getSampleRate());
    osc[1] = SineOscillator::create(getSampleRate());
    debugMessage("SR/BS/CH", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());
  }
  ~TestTonePatch(){
    SineOscillator::destroy(osc[0]);
    SineOscillator::destroy(osc[1]);
  }
  void processAudio(AudioBuffer &buffer) {
    float frequencyL = getParameterValue(PARAMETER_A) * 200+440;
    float amplitudeL = getParameterValue(PARAMETER_B);
    float frequencyR = getParameterValue(PARAMETER_C) * 200+440;
    float amplitudeR = getParameterValue(PARAMETER_D);
    // frequencyL = frequencyR = 440;
    // amplitudeL = amplitudeR = 0.5;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    osc[0]->setFrequency(frequencyL);
    osc[1]->setFrequency(frequencyR);
    osc[0]->generate(left);
    // osc[1]->generate(right);
    left.multiply(amplitudeL);
    right.multiply(amplitudeR);
    // for(int ch=2; ch<buffer.getChannels(); ++ch)
    //   buffer.getSamples(ch).copyFrom(left);
  }
};

#endif   // __TestTonePatch_hpp__
