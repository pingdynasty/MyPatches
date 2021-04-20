#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "OpenWareLibrary.h"
#include "WitchPatch.hpp"

#define VOICES 6
#define GAINFACTOR 0.4
#define BUTTON_VELOCITY 100

class VosimSynth : public AbstractSynth {
protected:
  VosimOscillator* osc;
  AdsrEnvelope* env;
  float gain;
  float mod = 0;
public:
  VosimSynth(VosimOscillator* osc, AdsrEnvelope* env) : osc(osc), env(env), gain(0) {}
  void setFrequency(float freq){
    osc->setFrequency(freq);    
  }
  void setGain(float gain){
    this->gain = gain*GAINFACTOR;
  }
  void trigger(){
    env->trigger();
  }
  void gate(bool state){
    env->gate(state);
  }
  void setEnvelope(float df){
    constexpr float tmin = 0.002;
    int di = (int)df;
    df = df - di;
    float attack, release;
    switch(di){
      // a/d
    case 0: // l/s
      attack = 1- df + tmin;
      release = tmin;
      break;
    case 1: // s/s
      attack = tmin;
      release = df + tmin;
      break;
    case 2: // s/l
      attack = df + tmin;
      release = 1.0 + tmin;
      break;
      // l/l
    }
    env->setAttack(attack);
    env->setRelease(release);
  }
  void setModulation(float modulation) override {
    mod = modulation*24;
  }
  void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case 1:
      // osc->setFormant1(noteToFrequency(value*12*6+12+mod));
      osc->setFormant1(osc->getFrequency()+noteToFrequency(value*12*8+mod));
      break;
    case 2:
      // osc->setFormant2(noteToFrequency(value*12*6+24+mod));
      osc->setFormant2(osc->getFrequency()+noteToFrequency(value*12*8+mod));
      break;
    case 3:
      setEnvelope(value*value*3);
      break;
    }
  }
};

class VosimSignalGenerator : public VosimSynth, public SignalGenerator  {
public:
  VosimSignalGenerator(VosimOscillator* osc, AdsrEnvelope* env) : VosimSynth(osc, env){}
  using SignalGenerator::generate;
  float generate(){
    // return osc->generate();
    return osc->generate()*env->generate()*gain;
  }
  static VosimSignalGenerator* create(float sr){
    VosimOscillator* osc = VosimOscillator::create(sr);
    AdsrEnvelope* env = AdsrEnvelope::create(sr);
    return new VosimSignalGenerator(osc, env);
  }
  static void destroy(VosimSignalGenerator* obj){
    VosimOscillator::destroy(obj->osc);
    AdsrEnvelope::destroy(obj->env);
    delete obj;
  }
};

#if VOICES == 1
typedef MonophonicSignalGenerator<VosimSignalGenerator> VosimVoices;
#else
typedef PolyphonicSignalGenerator<VosimSignalGenerator, VOICES> VosimVoices;
#endif

class VosimPatch : public WitchPatch {
  VosimVoices* voices;
  int basenote = 60;
  VoltsPerOctave hz;
public:
  VosimPatch(){
    voices = VosimVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i){
      VosimSignalGenerator* voice = VosimSignalGenerator::create(getSampleRate());
      voices->setVoice(i, voice);
    }
    registerParameter(PARAMETER_A, "Fundamental");
    registerParameter(PARAMETER_B, "Formant");
    registerParameter(PARAMETER_C, "Detune");
    registerParameter(PARAMETER_D, "Gain");
  }
  ~VosimPatch(){
    for(int i=0; i<VOICES; ++i){
      VosimSignalGenerator::destroy(voices->getVoice(i));
    }
    VosimVoices::destroy(voices);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    int note = 0;
    static int lastnote[4];
    switch(bid){
    case BUTTON_1:
      if(value){
	note = basenote;
	lastnote[0] = note;
	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(0, lastnote[0], 0));
      }
      break;
    case BUTTON_2:
      if(value){
	note = basenote+3;
	lastnote[1] = note;
	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(0, lastnote[1], 0));
      }
      break;
    case BUTTON_3:
      if(value){
	note = basenote+7;
	lastnote[2] = note;
	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(0, lastnote[2], 0));
      }
      break;
    case BUTTON_4:
      if(value){
	note = basenote+12;
	lastnote[3] = note;
	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(0, lastnote[3], 0));
      }
      break;
    }
  }
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }
  void processAudio(AudioBuffer &buffer) {
    basenote = getParameterValue(PARAMETER_A)*12*5+20;
    voices->setParameter(1, getParameterValue(PARAMETER_B));
    voices->setParameter(2, getParameterValue(PARAMETER_C));
    voices->setParameter(3, getParameterValue(PARAMETER_D));
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices->generate(left);
    left.tanh();
    right.copyFrom(left);
    doenv(getParameterValue(PARAMETER_D));
    dolfo(getParameterValue(PARAMETER_E)*20);
    dogate(voices->getNumberOfTakenVoices());
  }
};

#endif   // __VosimPatch_hpp__
