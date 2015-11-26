#ifndef __PolyBlepPatch_hpp__
#define __PolyBlepPatch_hpp__

#include "StompBox.h"
#include "Envelope.hpp"
#include "oscillator.h"
using namespace stmlib;

class PolyBlepPatch : public Patch {
private:
  const float VOLTAGE_MULTIPLIER = -4.40f;
  // const float VOLTAGE_OFFSET = 0.028f;
  const float VOLTAGE_OFFSET = -0.0585f;
  float mul;
  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
  AdsrEnvelope env;
  FloatArray envelope;
  Oscillator losc, rosc;
public:
  PolyBlepPatch() : mul(1.0/getSampleRate()), env(getSampleRate()) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Shape");
    registerParameter(PARAMETER_C, "Envelope");
    registerParameter(PARAMETER_D, "Gain");
    losc.Init();
    rosc.Init();
    env.setSustain(1.0);
    env.setDecay(0.0);
    env.setRelease(0.0);
    envelope = FloatArray::create(getBlockSize());
  }
  ~PolyBlepPatch(){
    FloatArray::destroy(envelope);
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*2.0 - 1.0;
    float shape = getParameterValue(PARAMETER_B)*2;
    float pw = 0.5;
    if(shape > 1.0){
      pw += 0.49*(shape-1.0); // pw 0.5 to 0.99
      shape = 1.0; // square wave
    }
    float c = getParameterValue(PARAMETER_C);
    if(c < 0.33){
      env.setAttack(c*3.33*2);
      env.setRelease(0.0);
    }else if(c < 0.66){
      env.setAttack((0.66-c)*3.33*2);
      env.setRelease((c-0.33)*3.33*2);
    }else{
      env.setAttack((c-0.66)*3.33*2);
      env.setRelease(2.0);
    }
    env.gate(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    float gain = getParameterValue(PARAMETER_D)*2;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    float lfreq = volts2hz(sample2volts(left[0]) + tune) * mul;
    float rfreq = volts2hz(sample2volts(right[0]) + tune) * mul;
    losc.Render<true>(lfreq, pw, shape, left, size);
    rosc.Render<true>(rfreq, pw, shape, right, size);
    env.getEnvelope(envelope);
    if(gain < 1.0){
      envelope.multiply(gain);
    }else{
      envelope.add(gain-1.0);
      //      envelope.multiply(1.0/gain); // scale down to avoid clipping
    }
    left.multiply(envelope);
    right.multiply(envelope);
  }
};

#endif   // __PolyBlepPatch_hpp__
