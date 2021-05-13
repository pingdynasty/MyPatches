#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "OpenWareLibrary.h"
#include "WitchPatch.hpp"

#define VOICES 4
#define GAINFACTOR 0.4
#define BUTTON_VELOCITY 100
#define TRIGGER_LIMIT (1<<19)

class VosimSynth : public AbstractSynth {
protected:
  VosimOscillator* osc;
  PhaserSignalProcessor phaser;
  AdsrEnvelope* env;
  float gain;
  float mod1 = 0;
  float mod2 = 0;
public:
  enum Parameters {
		   PARAMETER_F1,
		   PARAMETER_F2,
		   PARAMETER_ENVELOPE,
		   PARAMETER_EFFECT,
		   PARAMETER_PHASER_DELAY,
		   PARAMETER_PHASER_FEEDBACK,
  };
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
    mod2 = modulation*24;
  }
  void setPressure(float modulation) override {
    mod1 = modulation*24;
  }
  void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case PARAMETER_F1:
      osc->setFormant1(osc->getFrequency()+noteToFrequency(value*12*8+mod1));
      break;
    case PARAMETER_F2:
      osc->setFormant2(osc->getFrequency()+noteToFrequency(value*12*8+mod2+12));
      break;
    case PARAMETER_ENVELOPE:
      setEnvelope(value*value*3);
      break;
    case PARAMETER_EFFECT:
      phaser.setDepth(min(1, value*2));
      phaser.setFeedback(max(0, value-0.2));
      break;
    case PARAMETER_PHASER_DELAY:
      phaser.setDelay(value*0.04833+0.01833); // range: 440/(sr/2) to 1600/(sr/2)
      break;
    case PARAMETER_PHASER_FEEDBACK:
      phaser.setFeedback(value);
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
    return phaser.process(osc->generate())*env->generate()*gain;
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

class VosimPatch : public Patch {
  VosimVoices* voices;
  int basenote = 60;
  TapTempoSineOscillator* lfo1;
  TapTempoSineOscillator* lfo2;
public:
  VosimPatch(){
    voices = VosimVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i){
      VosimSignalGenerator* voice = VosimSignalGenerator::create(getSampleRate());
      voices->setVoice(i, voice);
    }
    registerParameter(PARAMETER_A, "Fundamental");
    registerParameter(PARAMETER_B, "Formant Low");
    registerParameter(PARAMETER_C, "Formant High");
    registerParameter(PARAMETER_D, "Gain");
    lfo1 = TapTempoSineOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo2 = TapTempoSineOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo1->setBeatsPerMinute(120);
    lfo2->setBeatsPerMinute(240);
    // lfo2 = TapTempoAgnesiOscillator::create(getBlockRate(), TRIGGER_LIMIT);
  }
  ~VosimPatch(){
    for(int i=0; i<VOICES; ++i){
      VosimSignalGenerator::destroy(voices->getVoice(i));
    }
    VosimVoices::destroy(voices);
    TapTempoSineOscillator::destroy(lfo1);
    TapTempoSineOscillator::destroy(lfo2);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    // int note = 0;
    // static int lastnote[4];
    switch(bid){
    // case BUTTON_1:
    //   if(value){
    // 	note = basenote;
    // 	lastnote[0] = note;
    // 	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
    //   }else{
    // 	voices->noteOff(MidiMessage::note(0, lastnote[0], 0));
    //   }
    //   break;
    case BUTTON_2:
      lfo1->trigger(value, samples);
      // if(value){
      // 	note = basenote+3;
      // 	lastnote[1] = note;
      // 	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      // }else{
      // 	voices->noteOff(MidiMessage::note(0, lastnote[1], 0));
      // }
      break;
    case BUTTON_3:
      lfo2->trigger(value, samples);
      // if(value){
      // 	note = basenote+7;
      // 	lastnote[2] = note;
      // 	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      // }else{
      // 	voices->noteOff(MidiMessage::note(0, lastnote[2], 0));
      // }
      break;
    // case BUTTON_4:
    //   if(value){
    // 	note = basenote+7;
    // 	lastnote[3] = note;
    // 	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
    //   }else{
    // 	voices->noteOff(MidiMessage::note(0, lastnote[3], 0));
    //   }
    //   break;
    }
  }
  
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }
  void processAudio(AudioBuffer &buffer) {
    basenote = round(getParameterValue(PARAMETER_A)*12*5+42);
    voices->setParameter(VosimSynth::PARAMETER_F1, getParameterValue(PARAMETER_B));
    voices->setParameter(VosimSynth::PARAMETER_F2, getParameterValue(PARAMETER_C));
    voices->setParameter(VosimSynth::PARAMETER_ENVELOPE, getParameterValue(PARAMETER_D));
    voices->setParameter(VosimSynth::PARAMETER_EFFECT, getParameterValue(PARAMETER_E));
    static bool gate = false;
    static int lastnote[4] = {0};
    bool state = isButtonPressed(BUTTON_A);
    if(state != gate){
      gate = state;
      if(state){
    	lastnote[0] = basenote;
    	voices->noteOn(MidiMessage::note(0, basenote, BUTTON_VELOCITY));
      }else{
    	voices->noteOff(MidiMessage::note(0, lastnote[0], 0));
      }
    }
    // voices->getVoice(0)->setNote(basenote);
    // voices->getVoice(0)->gate(isButtonPressed(BUTTON_A));

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices->generate(left);
    left.tanh();
    right.copyFrom(left);

    // lfo
    lfo1->clock(getBlockSize());
    lfo2->clock(getBlockSize());
    setParameterValue(PARAMETER_F, lfo1->generate());
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    setParameterValue(PARAMETER_G, lfo2->generate());
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);

    voices->setParameter(VosimSynth::PARAMETER_PHASER_DELAY, lfo1->generate()*0.5+0.5);
    // voices->setParameter(VosimSynth::PARAMETER_PHASER_FEEDBACK, getParameterValue(PARAMETER_AA));

    // debugMessage("lfo1/2", lfo1->getBeatsPerMinute(), lfo2->getBeatsPerMinute());
    // doenv(getParameterValue(PARAMETER_D));
    // dolfo(getParameterValue(PARAMETER_E)*20);
    // dogate(voices->getNumberOfTakenVoices());
  }
};

#endif   // __VosimPatch_hpp__
