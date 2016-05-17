#ifndef __SuperSawPadPatch_hpp__
#define __SuperSawPadPatch_hpp__

#include "StompBox.h"
#include "SuperSaw.hpp"
#include "VoltsPerOctave.h"
#include "Oscillators.hpp"
#include "BiquadFilter.h"
#include "Envelope.h"
#include "SmoothValue.h"

#define P1      1.0
#define m2      (P1+1.0/12)
#define M2      (P1+2.0/12)
#define m3      (P1+3.0/12)
#define M3      (P1+4.0/12)
#define P4      (P1+5.0/12)
#define d5      (P1+6.0/12)
#define A4      (P1+6.0/12)
#define P5      (P1+7.0/12)
#define m6      (P1+8.0/12)
#define M6      (P1+9.0/12)
#define m7      (P1+10.0/12)
#define M7      (P1+11.0/12)
#define P8      (P1+12.0/12)
#define SEMI    (1.0/12)
#define WHOLE   (2.0/12)

class Voice : public Oscillator, public Envelope {
};

class PadVoice : public Voice {
private:
  SuperSaw* osc;
  Oscillator* lfo;
  AdsrEnvelope* eg1;
  AdsrEnvelope* eg2;
  BiquadFilter* filter;
  const float sr;
  const int bs;
  // const float q = FilterStage::BUTTERWORTH_Q;
  float q = 0.95;
  float lfoamt = 0.85;
public:
  PadVoice(float samplingrate, int blocksize) :
    sr(samplingrate), bs(blocksize) {
    osc = new SuperSaw(sr);
    lfo = new SineOscillator(sr/bs);
    eg1 = new AdsrEnvelope(sr);
    eg2 = new AdsrEnvelope(sr/bs);
    filter = BiquadFilter::create(1);
    eg1->setSustain(0.8);
    eg2->setSustain(1.0);
    eg2->setDecay(0.0);
    osc->setDetune(0.06);
    osc->setMix(0.4);
    setTime(0.6);
    setFrequency(440);
  }
  void setResonance(float resonance){
    q = resonance;
  }
  void setModulationRate(float rate){
    lfo->setFrequency(rate);
  }
  void setModulationAmount(float amount){
    lfoamt = amount*.1;
  }
  void setTime(float time){
    time += 0.1;
    time *= 4;
    eg1->setAttack(time*0.2);
    eg2->setAttack(time*0.3);
    eg1->setDecay(time*0.18);
    // eg2->setDecay(time*0.2);
    eg1->setRelease(time*1.2);
    eg2->setRelease(time*1.6);
    lfo->setFrequency(1/time);
  }
  void setFrequency(float freq){
    osc->setFrequency(freq);
    float fc = freq/((sr/bs)*2);
    fc += lfoamt * lfo->getNextSample();
    fc *= eg2->getNextSample();
    fc = min(0.999, max(0.01, fc)); // normalised and bounded
    filter->setBandPass(fc, q);
  }
  void trigger(bool state, int delay){
    eg1->trigger(state, delay);
    eg2->trigger(state, delay);
    // lfo free-wheels...
  }
  void gate(bool state, int delay){
    eg1->gate(state, delay);
    eg2->gate(state, delay);
  }
  /* idea:
   * supersaw with bandpass fc at fundamental frequency
   * lfo sweeps fc
   * eg1 > vca
   * eg2 > filter fc
   */
  float getNextSample(){
    float sample = osc->getNextSample();
    // sample *= lfoamt * lfo->getNextSample();
    sample = filter->process(sample);
    sample *= eg1->getNextSample();
  }
  void getSamples(FloatArray output){
    osc->getSamples(output);
    filter->process(output);
    eg1->attenuate(output);
  }
};

class SuperSawPadPatch : public Patch {
private:
  PadVoice* pad[3];
  VoltsPerOctave hz;
  FloatArray buffer;
  SmoothFloat resonance;
  bool buttonstate;
public:
  SuperSawPadPatch() {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Time");
    registerParameter(PARAMETER_C, "Modulation");
    registerParameter(PARAMETER_D, "Gain");
    registerParameter(PARAMETER_E, "Resonance");
    pad[0] = new PadVoice(getSampleRate(), getBlockSize());
    pad[1] = new PadVoice(getSampleRate(), getBlockSize());
    pad[2] = new PadVoice(getSampleRate(), getBlockSize());
    buffer = FloatArray::create(getBlockSize());
  }
  bool trigger(bool isOn, int delay = 0){
    if(buttonstate != isOn && isOn)
      pad[0]->trigger(isOn, delay);
      pad[1]->trigger(isOn, delay);
      pad[2]->trigger(isOn, delay);
    buttonstate = isOn;
  }
  void gate(bool isOn, int delay){
    if(buttonstate != isOn){
      pad[0]->gate(isOn, delay);
      pad[1]->gate(isOn, delay);
      pad[2]->gate(isOn, delay);
      buttonstate = isOn;
    }
  }
  void processAudio(AudioBuffer &buf){
    float tune = getParameterValue(PARAMETER_A)*4.0-4.0;
    float time = getParameterValue(PARAMETER_B);
    float lfo = getParameterValue(PARAMETER_C);
    float gain = getParameterValue(PARAMETER_D)*2;
    resonance = getParameterValue(PARAMETER_E)*4 + 0.7;
    pad[0]->setResonance(resonance);
    pad[1]->setResonance(resonance);
    pad[2]->setResonance(resonance);
    pad[0]->setTime(time);
    pad[1]->setTime(time*1.38);
    pad[2]->setTime(time*1.83);
    pad[0]->setModulationAmount(lfo);
    pad[1]->setModulationAmount(lfo);
    pad[2]->setModulationAmount(lfo);
    gate(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune);
    float frequency = hz.getFrequency(left[0]);
    pad[0]->setFrequency(frequency);
    pad[1]->setFrequency(frequency*M3);
    pad[2]->setFrequency(frequency*P5);
    pad[0]->getSamples(left);
    right.copyFrom(left);
    right.multiply(gain);
    left.multiply(0.4);
    pad[1]->getSamples(buffer);
    buffer.multiply(0.4);
    left.add(buffer);
    pad[2]->getSamples(buffer);
    buffer.multiply(0.4);
    left.add(buffer);
    left.multiply(gain);
  }
};

#endif // __SuperSawPadPatch_hpp__
