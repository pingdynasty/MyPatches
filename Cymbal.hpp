#ifndef __Cymbal_hpp__
#define __Cymbal_hpp__

#include "BiquadFilter.h"
#include "Envelope.h"
#include "Oscillators.hpp"
#include "Drum.hpp"

class CymbalVoice : public Drum {
private:
public:
  SquareFMOscillator* osc[3];
  BiquadFilter* bp;
  BiquadFilter* hp;
  AdsrEnvelope* eg[2];
  const float fs;
  float gain;
  float accent;
public:
  CymbalVoice(float sr) : fs(sr), gain(0.0), accent(0.0) {
    osc[0] = new SquareFMOscillator(sr);
    osc[1] = new SquareFMOscillator(sr);
    osc[2] = new SquareFMOscillator(sr);
    eg[0] = new AdsrEnvelope(sr);
    eg[1] = new AdsrEnvelope(sr);
    eg[0]->setAttack(0.0f);
    eg[0]->setDecay(0.4f);
    eg[0]->setSustain(0.0f);
    eg[0]->setRelease(0.0f);
    eg[1]->setAttack(0.0f);
    eg[1]->setDecay(0.2f);
    eg[1]->setSustain(0.8f);
    eg[1]->setRelease(0.2f);
    bp = BiquadFilter::create(2);
    hp = BiquadFilter::create(2);
    bp->setBandPass(0.6, FilterStage::BUTTERWORTH_Q);
    hp->setHighPass(0.6, FilterStage::BUTTERWORTH_Q);
    osc[0]->setFrequency(835.34);
    osc[0]->setGain(2.5);
    osc[0]->setModulatorFrequency(310.47);
    osc[0]->setModulatorGain(0.016);
    osc[1]->setFrequency(1875.62);
    osc[1]->setGain(1.5);
    osc[1]->setModulatorFrequency(536.22);
    osc[1]->setModulatorGain(0.0414);
    osc[2]->setFrequency(7436.77);
    osc[2]->setGain(1);
    osc[2]->setModulatorFrequency(936.8196);
    osc[2]->setModulatorGain(0.1052);
  }
  CymbalVoice(float sr, float freq, float filter, float fm) : fs(sr), gain(0.0), accent(0.0) {
    osc[0] = new SquareFMOscillator(sr);
    osc[1] = new SquareFMOscillator(sr);
    osc[2] = new SquareFMOscillator(sr);
    osc[0]->setGain(2.5);
    osc[1]->setGain(1.5);
    osc[2]->setGain(1);
    eg[0] = new AdsrEnvelope(sr);
    eg[1] = new AdsrEnvelope(sr);
    eg[0]->setAttack(0.0f);
    eg[0]->setDecay(0.4f);
    eg[0]->setSustain(0.0f);
    eg[0]->setRelease(0.0f);
    eg[1]->setAttack(0.0f);
    eg[1]->setDecay(0.2f);
    eg[1]->setSustain(0.8f);
    eg[1]->setRelease(0.2f);
    bp = BiquadFilter::create(2);
    hp = BiquadFilter::create(2);
    setFrequency(freq);
    setFilter(filter);
    setFmAmount(fm);
  }
  void trigger(bool state, int delay){
    eg[0]->trigger(state, delay);
    eg[1]->trigger(state, delay);
    gain = accent * 0.6 + 0.7;
  }
  void gate(bool state, int delay){
    eg[0]->trigger(state, delay);
    eg[1]->gate(state, delay);
    gain = accent * 0.6 + 0.7;
  }
  void setFrequency(float f){
    osc[0]->setFrequency(f*2);
    osc[1]->setFrequency(f*3);
    osc[2]->setFrequency(f*4);
    osc[0]->setModulatorFrequency(f*2*0.93);
    osc[1]->setModulatorFrequency(f*3*0.84);
    osc[2]->setModulatorFrequency(f*4*1.33);
  }
  void setFmAmount(float value){
    osc[0]->setModulatorGain(value*0.1);
    osc[1]->setModulatorGain(value*0.2);
    osc[2]->setModulatorGain(value*0.4);
  }
  void setDecay(float d){
    eg[0]->setDecay(d*0.5);
    eg[1]->setDecay(d*0.25); 
    eg[1]->setRelease(d*0.33);
 }
  void setFilter(float f){
    bp->setBandPass(f, FilterStage::BUTTERWORTH_Q);
    hp->setHighPass(f, FilterStage::BUTTERWORTH_Q);
  }
  void setAccent(float amount){
    accent = amount;
  }
  float getNextSample(){
    float impact = osc[0]->getNextSample();
    impact += osc[1]->getNextSample();
    impact += osc[2]->getNextSample();
    float body = impact;
    impact = bp->process(impact);
    impact *= eg[0]->getNextSample();
    body = hp->process(body);
    body *= eg[1]->getNextSample();
    return (impact + body) * gain;
  }
};

#endif // __Cymbal_hpp__

