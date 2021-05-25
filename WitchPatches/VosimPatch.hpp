#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "OpenWareLibrary.h"

#define USE_MPE
#define VOICES 4
#define BUTTON_VELOCITY 100
#define TRIGGER_LIMIT (1<<22)

#include "WitchPatch.hpp"

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
      attack = df*df*2 + tmin;
      release = 1.0 + df*df; // allow extra-long decays
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
      setEnvelope(value*3);
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

class VosimSignalProcessor : public VosimSynth, public SignalProcessor {
public:
  VosimSignalProcessor(VosimOscillator* osc, AdsrEnvelope* env) : VosimSynth(osc, env){}
  using SignalProcessor::process;
  using MidiProcessor::process;
  float process(float input){
    // use input as a frequency scaler
    return osc->generate(input)*env->generate()*gain;
  }
  static VosimSignalProcessor* create(float sr){
    VosimOscillator* osc = VosimOscillator::create(sr);
    AdsrEnvelope* env = AdsrEnvelope::create(sr);
    return new VosimSignalProcessor(osc, env);
  }
  static void destroy(VosimSignalProcessor* obj){
    VosimOscillator::destroy(obj->osc);
    AdsrEnvelope::destroy(obj->env);
    delete obj;
  }
};

#if defined USE_MPE
typedef VosimSignalProcessor SynthVoice;
typedef MidiPolyphonicExpressionSignalProcessor<SynthVoice, VOICES> SynthVoices;
#elif VOICES == 1
typedef VosimSignalGenerator SynthVoice;
typedef MonophonicSignalGenerator<SynthVoice> SynthVoices;
#else
typedef VosimSignalProcessor SynthVoice;
typedef PolyphonicSignalProcessor<SynthVoice, VOICES> SynthVoices;
#endif

typedef StereoPhaserProcessor FxProcessor;
// typedef StereoChorusProcessor FxProcessor;

class VosimPatch : public Patch {
private:
  SynthVoices* voices;
  TapTempoSineOscillator* lfo1;
  TapTempoAgnesiOscillator* lfo2;
  CvNoteProcessor* cvnote;
  FxProcessor* fx;
  static constexpr float cvrange = 5;
public:
  VosimPatch(){
    voices = SynthVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, SynthVoice::create(getSampleRate()));
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Formant Low");
    registerParameter(PARAMETER_C, "Formant High");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_E, "Effect Amount");
    registerParameter(PARAMETER_F, "Sine LFO>");
    registerParameter(PARAMETER_G, "Witch LFO>");
    lfo1 = TapTempoSineOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo2 = TapTempoAgnesiOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo1->setBeatsPerMinute(60);
    lfo2->setBeatsPerMinute(120);

#ifdef USE_MPE
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 0, 18+6*cvrange);
    // send MPE Configuration Message RPN
    sendMidi(MidiMessage::cc(0, 100, 5));
    sendMidi(MidiMessage::cc(0, 101, 0));
    sendMidi(MidiMessage::cc(0, 6, VOICES));
#else
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 12*cvrange, 24);
#endif
    // fx = FxProcessor::create(getSampleRate(), getBlockSize(), 0.240*getSampleRate());
    fx = FxProcessor::create();
  }
  ~VosimPatch(){
    for(int i=0; i<VOICES; ++i)
      SynthVoice::destroy(voices->getVoice(i));
    SynthVoices::destroy(voices);
    TapTempoSineOscillator::destroy(lfo1);
    TapTempoAgnesiOscillator::destroy(lfo2);
    CvNoteProcessor::destroy(cvnote);
    FxProcessor::destroy(fx);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      cvnote->gate(value, samples);
      break;
    case BUTTON_2:
      lfo1->trigger(value, samples);
      if(value)
	lfo1->reset();
      break;
    case BUTTON_3:
      lfo2->trigger(value, samples);
      if(value)
	lfo2->reset();
      break;
    case BUTTON_4:
// #ifdef USE_MPE
//       /// todo! MPE sustain
//       // or: cvnote2->gate(value, samples);
#if VOICES == 1
#else
      static bool sustain = false;
      if(value){
	sustain = !sustain; // toggle
	voices->setSustain(sustain);
	if(!sustain)
	  voices->allNotesOff();
      }
      setButton(BUTTON_4, sustain);
#endif
      break;
    }
  }
  
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }

  void processAudio(AudioBuffer &buffer) {
    cvnote->cv(getParameterValue(PARAMETER_A));
    cvnote->clock(getBlockSize());
    voices->setParameter(VosimSynth::PARAMETER_F1, getParameterValue(PARAMETER_B));
    voices->setParameter(VosimSynth::PARAMETER_F2, getParameterValue(PARAMETER_C));
    voices->setParameter(VosimSynth::PARAMETER_ENVELOPE, getParameterValue(PARAMETER_D));
    fx->setEffect(getParameterValue(PARAMETER_E));

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    float fm_amount = getParameterValue(PARAMETER_AA);
    left.multiply(fm_amount);
#ifdef USE_MPE
    // 2 * exp2f(2x - 2) : 0.5 to 2 (plus minus one octave modulation)
    // 4 * exp2f(4x - 4) : 0.25 to 4 (plus minus two octaves modulation)
    float fm = exp2f(cvrange*getParameterValue(PARAMETER_A) - cvrange*0.5) - 1;    
    left.add(fm);
    voices->process(left, right);
#elif VOICES == 1
    voices->generate(right);
#else
    voices->process(left, right);
#endif
    left.copyFrom(right);
    fx->process(buffer, buffer);

    // lfo
    lfo1->clock(getBlockSize());
    lfo2->clock(getBlockSize());
    float lfo = lfo1->generate()*0.5+0.5;
    fx->setModulation(lfo);
    setParameterValue(PARAMETER_F, lfo*0.86+0.02);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    setParameterValue(PARAMETER_G, lfo2->generate()*0.86+0.02);
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
  }
};

#endif   // __VosimPatch_hpp__
