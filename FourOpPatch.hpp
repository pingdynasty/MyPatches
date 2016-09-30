#ifndef __FourOpPatch_hpp__
#define __FourOpPatch_hpp__

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
  // int ratio = 1.0;
  float offset = 0.0;
  Operator() : env(48000), osc(48000) {}
  void setSampleRate(float value){
    osc.setSampleRate(value);
    env.setSampleRate(value);
  }
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
  void gate(bool state, uint16_t samples){
    env.gate(state, samples);
  }
};

class FourOp : public Oscillator {
public:
  Operator ops[4];
  Control<PARAMETER_A> a;
  Control<PARAMETER_B> b;
  Control<PARAMETER_C> c;
  Control<PARAMETER_D> d;
  Control<PARAMETER_G> pb;
  Control<PARAMETER_H> h;
  Control<PARAMETER_AA> aa;
  Control<PARAMETER_AB> ab;
  Control<PARAMETER_AC> ac;
  Control<PARAMETER_AD> ad;
  Control<PARAMETER_AE> ae;
  Control<PARAMETER_AF> af;
  Control<PARAMETER_AG> ag;
  Control<PARAMETER_AH> ah;
  Control<PARAMETER_BA> ba;
  Control<PARAMETER_BB> bb;
  Control<PARAMETER_BC> bc;
  Control<PARAMETER_BD> bd;
  Control<PARAMETER_BE> be;
  Control<PARAMETER_BF> bf;
  Control<PARAMETER_BG> bg;
  Control<PARAMETER_BH> bh;
  Control<PARAMETER_CA> ca;
  Control<PARAMETER_CB> cb;
  Control<PARAMETER_CC> cc;
  Control<PARAMETER_CD> cd;
  Control<PARAMETER_CE> ce;
  Control<PARAMETER_CF> cf;
  Control<PARAMETER_CG> cg;
  Control<PARAMETER_CH> ch;
  Control<PARAMETER_DA> da;
  Control<PARAMETER_DB> db;
  Control<PARAMETER_DC> dc;
  Control<PARAMETER_DD> dd;
  Control<PARAMETER_DE> de;
  Control<PARAMETER_DF> df;
  Control<PARAMETER_DG> dg;
  Control<PARAMETER_DH> dh;
  const float RATIO_DEFAULT = 1;
  const float RATIO_MIN = 0.5;
  const float RATIO_RANGE = 7.5;
  const float OFFSET_DEFAULT = 0.0;
  const float INDEX_DEFAULT = 0.5;
  const float ATTACK_DEFAULT = 0.5;
  const float DECAY_DEFAULT = 0.5;
  const float SUSTAIN_DEFAULT = 0.5;
  const float RELEASE_DEFAULT = 0.5;
public:
  FourOp(){
    aa = RATIO_DEFAULT;
    ab = OFFSET_DEFAULT;
    ac = INDEX_DEFAULT;
    ae = ATTACK_DEFAULT;
    af = DECAY_DEFAULT;
    ag = SUSTAIN_DEFAULT;
    ah = RELEASE_DEFAULT;
    ba = RATIO_DEFAULT;
    bb = OFFSET_DEFAULT;
    bc = INDEX_DEFAULT;
    be = ATTACK_DEFAULT;
    bf = DECAY_DEFAULT;
    bg = SUSTAIN_DEFAULT;
    bh = RELEASE_DEFAULT;
    ca = RATIO_DEFAULT;
    cb = OFFSET_DEFAULT;
    cc = INDEX_DEFAULT;
    ce = ATTACK_DEFAULT;
    cf = DECAY_DEFAULT;
    cg = SUSTAIN_DEFAULT;
    ch = RELEASE_DEFAULT;
    da = RATIO_DEFAULT;
    db = OFFSET_DEFAULT;
    dc = INDEX_DEFAULT;
    de = ATTACK_DEFAULT;
    df = DECAY_DEFAULT;
    dg = SUSTAIN_DEFAULT;
    dh = RELEASE_DEFAULT;
  }
  void gate(bool state, uint16_t samples){
    ops[0].gate(state, samples);
    ops[1].gate(state, samples);
    ops[2].gate(state, samples);
    ops[3].gate(state, samples);
  }
  void setSampleRate(float value){
    ops[0].setSampleRate(value);
    ops[1].setSampleRate(value);
    ops[2].setSampleRate(value);
    ops[3].setSampleRate(value);
  }
  void setFrequency(float freq){
    ops[0].setFrequency(freq);
    ops[0].index = ac;
    ops[0].ratio = RATIO_MIN + aa*RATIO_RANGE;
    ops[0].offset = ab;
    ops[0].setEnvelope(ae, af, ag, ah);

    ops[1].setFrequency(freq);
    ops[1].index = b * bc;
    ops[1].ratio = RATIO_MIN + ba*RATIO_RANGE;
    ops[1].offset = bb;
    ops[1].setEnvelope(be, bf, bg, bh);

    ops[2].setFrequency(freq);
    ops[2].index = c * cc;
    ops[2].ratio = RATIO_MIN + ca*RATIO_RANGE;
    ops[2].offset = cb;
    ops[2].setEnvelope(ce, cf, cg, ch);

    ops[3].setFrequency(freq);
    ops[3].index = d * dc;
    ops[3].ratio = RATIO_MIN + da*RATIO_RANGE;
    ops[3].offset = db;
    ops[3].setEnvelope(de, df, dg, dh);
  }
  float getNextSample(){
    return 0.0f;
  }
  void getSamples(FloatArray samples){
    int algo = h*13;
    debugMessage("algo", algo);
    switch(algo){
    case 0:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo1();
      break;
    case 1:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo2();
      break;
    case 2:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo3();
      break;
    case 3:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo4();
      break;
    case 4:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo5();
      break;
    case 5:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo6();
      break;
    case 6:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo7();
      break;
    case 7:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo8();
      break;
    case 8:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo9();
      break;
    case 9:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo10();
      break;
    case 10:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo11();
      break;
    case 11:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo12();
      break;
    case 12:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] = algo13();
      break;
    }
  }
  float algo1(){
    // 0>1>2>3>
    float sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    sample = ops[2].getNextSample(sample);
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo2(){
    // 0>
    // 1>2>3>
    float sample = ops[0].getNextSample();
    sample += ops[1].getNextSample();
    sample = ops[2].getNextSample(sample);
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo3(){
    // 1>2>
    //   0>3>
    float sample = ops[1].getNextSample();
    sample = ops[2].getNextSample(sample);
    sample += ops[0].getNextSample();
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo4(){
    // 0>1
    //  >2>3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    out += ops[2].getNextSample(sample);
    out = ops[3].getNextSample(out);
    return out;
  }
  float algo5(){
    // 0>1>2>
    //    >3>
    float sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    float out = ops[2].getNextSample(sample);
    out += ops[3].getNextSample(sample);
    return out;
  }
  float algo6(){
    // 0>1>2>
    //     3>
    float sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    float out = ops[2].getNextSample(sample);
    out += ops[3].getNextSample();
    return out;
  }
  float algo7(){
    // 0>
    // 1>
    // 2>3>
    float sample = ops[0].getNextSample();
    sample += ops[1].getNextSample();
    sample += ops[2].getNextSample();
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo8(){
    // 0>1>
    // 2>3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    sample = ops[2].getNextSample();
    out += ops[3].getNextSample(sample);
    return out;
  }
  float algo9(){
    // 0>1>
    //  >2>
    //  >3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    out += ops[2].getNextSample(sample);
    out += ops[3].getNextSample(sample);
    return out;
  }
  float algo10(){
    // 0>1>
    //   2>
    //   3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    out += ops[2].getNextSample();
    out += ops[3].getNextSample();
    return out;
  }
  float algo11(){
    // 0>
    // 1>
    // 2>
    // 3>
    float out = ops[0].getNextSample();
    out += ops[1].getNextSample();
    out += ops[2].getNextSample();
    out += ops[3].getNextSample();
    return out;
  }
  float algo12(){
    // 3>3>
    // 0>1>2>
    static float fb = 0.0f;
    fb = ops[3].getNextSample(fb);
    // tx81z appears to have global feedback(op4) parameter
    float out = ops[0].getNextSample();
    out = ops[1].getNextSample(out);
    out = ops[2].getNextSample(out+fb);
    return out;
  }
  float algo13(){
    // 3>3>2>
    //     0>1>
    static float fb = 0.0f;
    float out = ops[0].getNextSample();
    fb = ops[3].getNextSample(fb);
    out += ops[2].getNextSample(fb);
    out = ops[1].getNextSample(out);
    return out;
  }
  void noteOn(uint16_t note, uint16_t velocity, uint16_t samples){
    float freq = 440.0f*fastpow2f((note-69 + pb*2)/12.0);
    setFrequency(freq);
    gate(true, samples);
  }
  void noteOff(uint16_t samples){
    gate(false, samples);
  }
};

class FourOpPatch : public Patch {
private:
  FourOp algo;
  VoltsPerOctave hz;
  BiquadFilter* lp;
public:
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      if(value)
	algo.noteOn(note, value, samples);
      else
	algo.noteOff(samples);
    }else if(bid == PUSHBUTTON){
      algo.gate(value, samples);
    }
  }
  FourOpPatch() { // : algo(getBlockSize()) {
    algo.setSampleRate(getSampleRate());
    registerParameter(PARAMETER_A, "Operator 1");
    registerParameter(PARAMETER_B, "Operator 2");
    registerParameter(PARAMETER_C, "Operator 3");
    registerParameter(PARAMETER_D, "Operator 4");
    lp = StereoBiquadFilter::create(2);
    lp->setLowPass(0.8, FilterStage::BUTTERWORTH_Q);
  }
  ~FourOpPatch(){
  }
  void processAudio(AudioBuffer &buffer) {
    float fundamental = getParameterValue(PARAMETER_A)*5.0 - 1.0;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    hz.setTune(fundamental);
    float freq = hz.getFrequency(0);
    algo.setFrequency(freq);
    algo.getSamples(left);
    lp->process(left);
  }
};

#endif   // __FourOpPatch_hpp__
