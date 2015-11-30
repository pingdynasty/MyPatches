#ifndef __PolyBlepPatch_hpp__
#define __PolyBlepPatch_hpp__

#include "StompBox.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "oscillator.h"
using namespace stmlib;

class PolyBlepPatch : public Patch {
private:
  float mul;
  AdsrEnvelope env;
  FloatArray envelope;
  Oscillator losc, rosc;
  VoltsPerOctave hz;
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
    float tune = getParameterValue(PARAMETER_A)*10.0 - 5.0;
    float shape = getParameterValue(PARAMETER_B)*2;
    float pw = 0.5;
    if(shape > 1.0){
      pw += 0.49*(shape-1.0); // pw 0.5 to 0.99
      shape = 1.0; // square wave
    }
    float c = getParameterValue(PARAMETER_C);
    if(c < 0.5){
      env.setAttack((0.25-c*c));
      env.setRelease(0.0);
    }else{
      env.setAttack(0.0);
      env.setRelease((c*c-0.25));
    }
    env.trigger(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    float gain = getParameterValue(PARAMETER_D)*2;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune);
    float lfreq = hz.getFrequency(left[0]) * mul;
    float rfreq = hz.getFrequency(right[0]) * mul;
    int size = buffer.getSize();
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
