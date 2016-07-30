#ifndef __OperatorPatch_hpp__
#define __OperatorPatch_hpp__

#include "Patch.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"

#include "Oscillators.hpp"

class Operator : public Oscillator {
private:
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
private:
  Operator ops[6];
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
    
    ops[0].setEnvelope(0, 0.75, 0.4, 1.7);
    ops[1].setEnvelope(0, 0.15, 0.3, 0.4);
    ops[1].setEnvelope(0, 0.00, 0.9, 1.8);
    ops[1].setEnvelope(0, 0.00, 0.7, 0.4);
    ops[1].setEnvelope(0, 0.40, 0.3, 5.0);
    ops[1].setEnvelope(0, 0.08, 0.7, 3.0);
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

      ops[0].setEnvelope(0.0, 0.0, 1.0, 0.0);
      // osc[0].setEnvelope(0.0, 0.0, 1.0, 0.0);
    // osc.gate(true);
    // osc.setFrequency(440);
  }
 Operator osc;

  float getNextSample(){
    float sample, out;
    sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    out = ops[2].getNextSample(sample);
    sample = ops[3].getNextSample();
    sample = ops[4].getNextSample(sample);
    out += ops[5].getNextSample(sample);
    // out = ops[0].getNextSample();
    // out += ops[1].getNextSample();
    // out += ops[2].getNextSample();
    // out += ops[3].getNextSample();
    // out += ops[4].getNextSample();
    // out += ops[5].getNextSample();
    return out;
    // static SineOscillator osc(48000);
    // osc.setFrequency(440);
    // return ops[0].getNextSample();
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
    registerParameter(PARAMETER_B, "Detune");
    registerParameter(PARAMETER_C, "FM Ratio");
    registerParameter(PARAMETER_D, "FM Index");
    registerParameter(PARAMETER_E, "Gain");
  }
  ~OperatorPatch(){
  }
  bool buttonstate = false;
  void processAudio(AudioBuffer &buffer) {
    float fundamental = getParameterValue(PARAMETER_A)*6.0 - 1.0;
    float gain = getParameterValue(PARAMETER_D)*2;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    if(isButtonPressed(PUSHBUTTON) != buttonstate){
      buttonstate = isButtonPressed(PUSHBUTTON);
      algo.gate(buttonstate);
    }
    hz.setTune(fundamental);
    float freq = hz.getFrequency(0);
    algo.setFrequency(freq);
    // algo.setFrequency(440);
    algo.getSamples(left);
    left.multiply(gain);
  }
};

#endif   // __OperatorPatch_hpp__
