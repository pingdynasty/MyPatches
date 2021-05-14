#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "OpenWareLibrary.h"
#include "WitchPatch.hpp"

#define VOICES 4
#define GAINFACTOR 0.35
#define BUTTON_VELOCITY 100
#define TRIGGER_LIMIT (1<<22)

class VosimSynth : public AbstractSynth {
protected:
  VosimOscillator* osc;
  AdsrEnvelope* env;
  float gain;
  float mod1 = 0;
  float mod2 = 0;
public:
  enum Parameters {
		   PARAMETER_F1,
		   PARAMETER_F2,
		   PARAMETER_ENVELOPE
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
    }
  }
};

class VosimSignalGenerator : public VosimSynth, public SignalGenerator  {
public:
  VosimSignalGenerator(VosimOscillator* osc, AdsrEnvelope* env) : VosimSynth(osc, env){}
  using SignalGenerator::generate;
  float generate(){
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

class StereoPhaserProcessor : public MultiSignalProcessor {
protected:
  PhaserSignalProcessor phaser_left;
  PhaserSignalProcessor phaser_right;
public:
  void setEffect(float value){
    phaser_left.setDepth(min(1, value*2));
    phaser_left.setFeedback(max(0, value*1.25-0.4));
    phaser_right.setDepth(min(1, value*2));
    phaser_right.setFeedback(max(0, value*1.25-0.4));
  }
  void setDelay(float value){
      // phaser.setDelay(value*0.04833+0.01833); // range: 440/(sr/2) to 1600/(sr/2)
    phaser_left.setDelay(value*0.04+0.02);
    phaser_right.setDelay(value*0.05+0.018);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    phaser_left.process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    phaser_right.process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
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
  TapTempoAgnesiOscillator* lfo2;
  StereoPhaserProcessor phaser;
public:
  VosimPatch(){
    voices = VosimVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, VosimSignalGenerator::create(getSampleRate()));
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Formant Low");
    registerParameter(PARAMETER_C, "Formant High");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_E, "Effect Amount");
    registerParameter(PARAMETER_F, "LFO1>");
    registerParameter(PARAMETER_G, "LFO2>");
    lfo1 = TapTempoSineOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo2 = TapTempoAgnesiOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo1->setBeatsPerMinute(60);
    lfo2->setBeatsPerMinute(120);
  }
  ~VosimPatch(){
    for(int i=0; i<VOICES; ++i)
      VosimSignalGenerator::destroy(voices->getVoice(i));
    VosimVoices::destroy(voices);
    TapTempoSineOscillator::destroy(lfo1);
    TapTempoAgnesiOscillator::destroy(lfo2);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_2:
      lfo1->trigger(value, samples);
      break;
    case BUTTON_3:
      lfo2->trigger(value, samples);
      break;
    case BUTTON_4:
      static bool sustain = false;
      if(value){
	sustain = !sustain; // toggle
	voices->setSustain(sustain);
      }
      setButton(BUTTON_4, sustain);
      break;
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
    phaser.setEffect(getParameterValue(PARAMETER_E));
    static bool gate = {false};
    static int lastnote = {0};
    bool state = isButtonPressed(BUTTON_1);
    if(state != gate){
      gate = state;
      if(state){
    	lastnote = basenote;
    	voices->noteOn(MidiMessage::note(0, basenote, BUTTON_VELOCITY));
      }else{
    	voices->noteOff(MidiMessage::note(0, lastnote, 0));
      }
    }

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices->generate(left);
    right.copyFrom(left);
    phaser.process(buffer, buffer);
    left.tanh();
    right.tanh();

    // lfo
    lfo1->clock(getBlockSize());
    lfo2->clock(getBlockSize());
    float lfo = lfo1->generate()*0.5+0.5;
    phaser.setDelay(lfo);
    setParameterValue(PARAMETER_F, lfo);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    setParameterValue(PARAMETER_G, lfo2->generate());
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
  }
};

#endif   // __VosimPatch_hpp__
