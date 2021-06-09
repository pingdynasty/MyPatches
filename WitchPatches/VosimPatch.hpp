#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "OpenWareLibrary.h"

#define USE_MPE
#define VOICES 8

#include "WitchFX.hpp"

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

typedef VosimSignalProcessor SynthVoice;

#if defined USE_MPE
typedef MidiPolyphonicExpressionProcessor<SynthVoice, VOICES> Allocator;
#elif VOICES == 1
typedef MonophonicProcessor<SynthVoice> Allocator;
#else
typedef PolyphonicProcessor<SynthVoice, VOICES> Allocator;
#endif

typedef VoiceAllocatorSignalProcessor<Allocator, SynthVoice, VOICES> SynthVoices;

#include "WitchPatch.hpp"

class VosimPatch : public WitchPatch {
private:
public:
  VosimPatch(){
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Formant Low");
    registerParameter(PARAMETER_C, "Formant High");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_E, "Effect Amount");
    registerParameter(PARAMETER_F, "Sine LFO>");
    registerParameter(PARAMETER_G, "Witch LFO>");

    registerParameter(PARAMETER_AA, "FM Amount");
    setParameterValue(PARAMETER_AA, 0.1);
    registerParameter(PARAMETER_AD, "FX Select");
    setParameterValue(PARAMETER_AD, fx->getParameterValueForEffect(WitchMultiEffect::CHORUS));

    // voices
    voices = SynthVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, SynthVoice::create(getSampleRate()));

#ifdef USE_MPE
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 0, 18+6*cvrange);
#else
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 12*cvrange, 24);
#endif
  }
  ~VosimPatch(){
    for(int i=0; i<VOICES; ++i)
      SynthVoice::destroy(voices->getVoice(i));
    SynthVoices::destroy(voices);
    CvNoteProcessor::destroy(cvnote);
  }
  void processAudio(AudioBuffer &buffer) {
    cvnote->clock(getBlockSize());
    cvnote->cv(getParameterValue(PARAMETER_A));

    voices->setParameter(VosimSynth::PARAMETER_F1, getParameterValue(PARAMETER_B));
    voices->setParameter(VosimSynth::PARAMETER_F2, getParameterValue(PARAMETER_C));
    voices->setParameter(VosimSynth::PARAMETER_ENVELOPE, getParameterValue(PARAMETER_D));

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
    dofx(buffer);
    dolfo();
  }
};

#endif   // __VosimPatch_hpp__
