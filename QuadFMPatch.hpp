#ifndef __QuadFMPatch_hpp__
#define __QuadFMPatch_hpp__

#include "Patch.h"
#include "VoltsPerOctave.h"
#include "SineOscillator.h"
#include "BiquadFilter.h"
#include "Control.h"
#include "SmoothValue.h"

class Operator : public Oscillator {
private:
public:
  SineOscillator osc;
public:
  float index = 1.0;
  float ratio = 1.0;
  float offset = 0.0;
  SmoothFloat level = index;
  Operator() : osc(48000) {
    level.lambda = 0.99;
  }
  void setSampleRate(float value){
    osc.setSampleRate(value);
  }
  void setFrequency(float freq){
    osc.setFrequency(freq*ratio+offset);
  }
  float getNextSample(){
    level = index;
    return level*osc.getNextSample();
  }
  float getNextSample(float fm){
    return index*osc.getNextSample(fm);
  }
};

class QuadFM : public Oscillator {
public:
  Operator ops[4];
  Control<PARAMETER_A> a;
  Control<PARAMETER_B> b;
  Control<PARAMETER_C> c;
  Control<PARAMETER_D> d;
  Control<PARAMETER_H> h;
  Control<PARAMETER_AA> aa;
  Control<PARAMETER_AB> ab;
  Control<PARAMETER_AC> ac;
  Control<PARAMETER_AD> ad;
  Control<PARAMETER_AE> ba;
  Control<PARAMETER_AF> bb;
  Control<PARAMETER_AG> bc;
  Control<PARAMETER_AH> bd;
  Control<PARAMETER_BA> ca;
  Control<PARAMETER_BB> cb;
  Control<PARAMETER_BC> cc;
  Control<PARAMETER_BD> cd;
  Control<PARAMETER_BE> da;
  Control<PARAMETER_BF> db;
  Control<PARAMETER_BG> dc;
  Control<PARAMETER_BH> dd;
  const float RATIO_DEFAULT = 1;
  const float RATIO_MIN = 0.5;
  const float RATIO_RANGE = 7.5;
  const float OFFSET_DEFAULT = 0.0;
  const float OFFSET_RANGE = 1000;
  const float INDEX_DEFAULT = 0.1;
public:
  QuadFM(){
    h = 0; // algo1 0>1>2>3>
    aa = RATIO_DEFAULT*3;
    ab = OFFSET_DEFAULT;
    ac = INDEX_DEFAULT;
    ba = RATIO_DEFAULT*2;
    bb = OFFSET_DEFAULT;
    bc = INDEX_DEFAULT;
    ca = RATIO_DEFAULT*0.5;
    cb = OFFSET_DEFAULT;
    cc = INDEX_DEFAULT;
    da = 1;
    db = OFFSET_DEFAULT;
    dc = INDEX_DEFAULT;
  }
  void setSampleRate(float value){
    ops[0].setSampleRate(value);
    ops[1].setSampleRate(value);
    ops[2].setSampleRate(value);
    ops[3].setSampleRate(value);
  }
  void setFrequency(float freq){
    ops[0].setFrequency(freq);
    ops[0].index = a * ac;
    ops[0].ratio = RATIO_MIN + aa*RATIO_RANGE;
    ops[0].offset = ab*OFFSET_RANGE;

    ops[1].setFrequency(freq);
    ops[1].index = b * bc;
    ops[1].ratio = RATIO_MIN + ba*RATIO_RANGE;
    ops[1].offset = bb*OFFSET_RANGE;

    ops[2].setFrequency(freq);
    ops[2].index = c * cc;
    ops[2].ratio = RATIO_MIN + ca*RATIO_RANGE;
    ops[2].offset = cb*OFFSET_RANGE;

    ops[3].setFrequency(freq);
    ops[3].index = d * dc;
    ops[3].ratio = RATIO_MIN + da*RATIO_RANGE;
    ops[3].offset = db*OFFSET_RANGE;
  }
  float getNextSample(){
    return 0.0f;
  }
  void getSamples(FloatArray samples){
    int algo = h*13;
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
};

class QuadFMPatch : public Patch {
private:
  QuadFM algo;
  VoltsPerOctave hz;
  BiquadFilter* lp;
public:
  QuadFMPatch() {
    algo.setSampleRate(getSampleRate());
    registerParameter(PARAMETER_A, "Operator 1");
    registerParameter(PARAMETER_B, "Operator 2");
    registerParameter(PARAMETER_C, "Operator 3");
    registerParameter(PARAMETER_D, "Operator 4");
    lp = StereoBiquadFilter::create(2);
    lp->setLowPass(0.8, FilterStage::BUTTERWORTH_Q);
    hz.offset = -0.0589f;
    hz.multiplier = -4.399f;
  }
  ~QuadFMPatch(){
  }
  void processAudio(AudioBuffer &buffer) {
    float fundamental = -2.0; // getParameterValue(PARAMETER_A)*5.0 - 1.0;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    hz.setTune(fundamental);
    float freq = hz.getFrequency(left[0]);
    algo.setFrequency(freq);
    algo.getSamples(left);
    lp->process(left);
  }
};

#endif   // __QuadFMPatch_hpp__
