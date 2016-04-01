#ifndef __SynthVoicePatch_hpp__
#define __SynthVoicePatch_hpp__

#include "StompBox.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"

/*
 * A: Pitch
 * B: Fc exp
 * C: Resonance
 * D: Envelope / amplitude
 * E: Waveshape
 * Left: Pitch
 * Right: Fc lin
 */
class SynthVoicePatch : public Patch {
private:
  AdsrEnvelope env;
  FloatArray envelope;
  PolyBlepOscillator osc;
  VoltsPerOctave hz;
  BiquadFilter* filter;
public:
  SynthVoicePatch() : osc(getSampleRate()), env(getSampleRate()) {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Fc");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_E, "Waveshape");
    env.setSustain(1.0);
    env.setDecay(0.0);
    env.setRelease(0.0);
    envelope = FloatArray::create(getBlockSize());
    filter = BiquadFilter::create(4); // 8-pole filter 48dB
    filter->setLowPass(0.8, FilterStage::BUTTERWORTH_Q);
  }
  ~SynthVoicePatch(){
    FloatArray::destroy(envelope);
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*10.0 - 6.0;
    float fc = getParameterValue(PARAMETER_B)*10.0 - 6.0;
    float q = getParameterValue(PARAMETER_C)*3+0.25;
    float shape = getParameterValue(PARAMETER_E)*2;
    float pw = 0.5;
    if(shape > 1.0){
      pw += 0.49*(shape-1.0); // pw 0.5 to 0.99
      shape = 1.0; // square wave
    }
    float df = getParameterValue(PARAMETER_D);
    int di = getParameterValue(PARAMETER_D)*4;
    float gain = 0.0f;
    switch(di){
      // a/d
    case 0: // l/s
      env.setAttack(1.0-df*4);
      env.setRelease(0.0);
      break;
    case 1: // s/s
      env.setAttack(0.0);
      env.setRelease((df-0.25)*4);
      break;
    case 2: // s/l
      env.setAttack((df-0.5)*4);
      env.setRelease(1.0);
      break;
    case 3: // l/l
      env.setAttack(1.0);
      env.setRelease(1.0);
      gain = (df-0.75)*4;
      break;
    }
    env.trigger(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune);
    float lfreq = hz.getFrequency(left[0]);
    osc.setFrequency(lfreq);
    osc.setShape(shape);
    osc.setPulseWidth(pw);
    osc.getSamples(left);
    env.getEnvelope(envelope);
    envelope.add(gain);
    left.multiply(envelope);
    hz.setTune(fc);
    fc = hz.getFrequency(right[0]);
    fc = min(0.999, max(0.01, fc/(getSampleRate()*2))); // normalised and bounded
    filter->setLowPass(fc, q);
    right.copyFrom(left);
    filter->process(right);
    right.multiply(0.8); // avoid excessive clipping
  }
};

#endif   // __SynthVoicePatch_hpp__
