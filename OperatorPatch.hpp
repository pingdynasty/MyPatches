#ifndef __OperatorPatch_hpp__
#define __OperatorPatch_hpp__

#include "Patch.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"
#include "Control.h"

#include "Oscillators.hpp"

class Operator : public Oscillator {
private:
public:
  AdsrEnvelope env;
  SineOscillator osc;
public:
  float index = 1.0;
  float ratio = 1.0;
  float offset = 0.0;
  Operator() : env(48000), osc(48000) {}
  void setFrequency(float freq){
    osc.setFrequency(freq*ratio+offset);
    // osc.setFrequency(freq);
  }
  void setEnvelope(float a, float d, float s, float r){
    env.setAttack(a);
    env.setDecay(d);
    env.setSustain(s);
    env.setRelease(r);
  }
  float getNextSample(){
    return env.getNextSample()*index*osc.getNextSample();
  }
  float getNextSample(float fm){
    return env.getNextSample()*index*osc.getNextSample(fm);
  }
  void gate(bool state){
    env.gate(state);
  }
};

class SixOp : public Oscillator {
// private:
public:
  Operator ops[6];
  Control<PARAMETER_B> mix;
  Control<PARAMETER_C> attack;
  Control<PARAMETER_D> release;
public:
  SixOp(){
    // D-7 preset
    ops[0].index = 0.17;
    ops[0].ratio = 12.0057;
    ops[0].offset = 4.67;

    ops[1].index = 0.65;
    ops[1].ratio = 0.4997;
    ops[1].offset = -0.26;

    ops[2].index = 0.60;
    ops[2].ratio = 1.997;
    ops[2].offset = -0.26;

    ops[3].index = 0.19;
    ops[3].ratio = 2.9995;
    ops[3].offset = -0.39;

    ops[4].index = 0.68;
    ops[4].ratio = 0.5002;
    ops[4].offset = -0.19;

    ops[5].index = 0.60;
    ops[5].ratio = 0.5001;
    ops[5].offset = -0.06;
    
    ops[0].setEnvelope(0, 0.75, 0.4, 1.3);
    ops[1].setEnvelope(0, 0.15, 0.3, 0.7);
    ops[2].setEnvelope(0, 0.00, 0.9, 0.7);
    ops[3].setEnvelope(0, 0.00, 0.7, 0.4);
    ops[4].setEnvelope(0, 0.40, 0.3, 2.0);
    ops[5].setEnvelope(0, 0.08, 0.7, 1.2);
  }
  void gate(bool state){
    ops[0].gate(state);
    ops[1].gate(state);
    ops[2].gate(state);
    ops[3].gate(state);
    ops[4].gate(state);
    ops[5].gate(state);
  }
  void setFrequency(float freq){
    ops[0].setFrequency(freq);
    ops[1].setFrequency(freq);
    ops[2].setFrequency(freq);
    ops[3].setFrequency(freq);
    ops[4].setFrequency(freq);
    ops[5].setFrequency(freq);
    ops[2].index = 1.0-mix;
    ops[5].index = mix;
    ops[2].env.setAttack(attack*0.2);
    ops[5].env.setAttack(attack*1.2);
    ops[2].env.setRelease(release*1.3);
    ops[5].env.setRelease(release*1.9);
  }
  float getNextSample(){
    float sample, out;
    // 0>1>2>
    // 3>4>5>
    sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    out = ops[2].getNextSample(sample);
    sample = ops[3].getNextSample();
    sample = ops[4].getNextSample(sample);
    out += ops[5].getNextSample(sample);
    return out;
  }
};

class OperatorPatch : public Patch {
private:
  SixOp algo;
  VoltsPerOctave hz;
public:
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
    }else if(bid == PUSHBUTTON){
      algo.gate(value);
    }
  }
  OperatorPatch() {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Operator Mix");
    registerParameter(PARAMETER_C, "Attack");
    registerParameter(PARAMETER_D, "Decay");
  }
  ~OperatorPatch(){
  }
  void processAudio(AudioBuffer &buffer) {
    float fundamental = getParameterValue(PARAMETER_A)*5.0 - 1.0;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    hz.setTune(fundamental);
    float freq = hz.getFrequency(0);
    algo.setFrequency(freq);
    algo.getSamples(left);
  }
};

#endif   // __OperatorPatch_hpp__
