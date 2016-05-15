#ifndef __SynthVoicePatch_hpp__
#define __SynthVoicePatch_hpp__

#include "StompBox.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"
#include "SmoothValue.h"

class TriggerButton {
private:
  Envelope* env;
  bool state;
public:
  TriggerButton(Envelope* envelope) : 
    env(envelope), state(false){}
  bool trigger(bool isOn, int delay = 0){
    if(state != isOn && isOn)
      env->trigger(delay);
    state = isOn;
  }
  void gate(bool isOn, int delay){
    if(state != isOn){
      env->gate(isOn, delay);
      state = isOn;
    }
  }
};

/* todo: quantize A to semitones */
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
  TriggerButton* button;
  FloatArray envelope;
  PolyBlepOscillator osc;
  VoltsPerOctave hz;
  BiquadFilter* filter;
  SmoothFloat tune, fc, q;
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
    button = new TriggerButton(&env);
  }
  ~SynthVoicePatch(){
    FloatArray::destroy(envelope);
    BiquadFilter::destroy(filter);
  }
  void processAudio(AudioBuffer &buffer) {
    tune = getParameterValue(PARAMETER_A)*10.0 - 6.0;
    float cutoff = getParameterValue(PARAMETER_B)*10.0 - 4.0;
    q = getParameterValue(PARAMETER_C)*3+0.75;
    float shape = getParameterValue(PARAMETER_E)*2;
    float pw = 0.5;
    if(shape > 1.0){
      pw += 0.49*(shape-1.0); // pw 0.5 to 0.99
      shape = 1.0; // square wave
    }
    float df = getParameterValue(PARAMETER_D)*4;
    int di = (int)df;
    float gain = 0.0f;
    switch(di){
      // a/d
    case 0: // l/s
      env.setAttack(1.0-df);
      env.setRelease(0.0);
      break;
    case 1: // s/s
      env.setAttack(0.0);
      env.setRelease(df-1);
      break;
    case 2: // s/l
      env.setAttack(df-2);
      env.setRelease(1.0);
      break;
    case 3: // l/l
      env.setAttack(1.0);
      env.setRelease(1.0);
      gain = df-3;
      break;
    }
    button->gate(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    // if(button.gate(isButtonPressed(PUSHBUTTON))
    //    env.trigger(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    // vco
    hz.setTune(tune);
    float freq = hz.getFrequency(left[0]);
    osc.setFrequency(freq);
    osc.setShape(shape);
    osc.setPulseWidth(pw);
    osc.getSamples(left);
    // vcf
    hz.setTune(cutoff);
    cutoff = hz.getFrequency(right[0]);
    fc = min(0.999, max(0.01, cutoff/(getSampleRate()*2))); // normalised and bounded
    filter->setLowPass(fc, q);
    right.copyFrom(left);
    filter->process(right);
    right.multiply(0.8-q*0.2); // gain compensation for high q
    // vca
    env.getEnvelope(envelope);
    envelope.add(gain);
    left.multiply(envelope);
    right.multiply(envelope);
  }
};

#endif   // __SynthVoicePatch_hpp__
