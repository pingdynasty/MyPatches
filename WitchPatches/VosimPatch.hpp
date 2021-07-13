#ifndef __VosimPatch_hpp__
#define __VosimPatch_hpp__

#include "OpenWareLibrary.h"

#define VOICES 6
#define FORMANT_ABSOLUTE // undefine to modulate formant frequency as ratio of oscillator frequency

#include "WitchFX.hpp"

/**
 * Get a frequency multiplier that scales a given number of octaves up and down, 
 * for 0<=x<=1, where x=0.5 equals unity
 */
float getFrequencyScalar(float x, float octaves){
  return exp2f(2*octaves*x - octaves);
}
			 
class VosimSynth : public AbstractSynth {
protected:
  VosimOscillator* osc;
  WitchEnvelope* env;
  float gain;
  float mod1 = 0;
  float mod2 = 0;
public:
  enum Parameters {
		   PARAMETER_F1,
		   PARAMETER_F2,
		   PARAMETER_WAVESHAPE,
		   PARAMETER_ENVELOPE
  };
  VosimSynth(VosimOscillator* osc, WitchEnvelope* env) : osc(osc), env(env), gain(0) {}
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
  void setModulation(float modulation) override {
#ifdef FORMANT_ABSOLUTE
    mod2 = modulation*24;
#else
    mod2 = modulation*0.2;
#endif
  }
  void setPressure(float modulation) override {
#ifdef FORMANT_ABSOLUTE
    mod1 = modulation*24;
#else
    mod1 = modulation*0.2;
#endif
  }
  void setParameter(uint8_t parameter_id, float value){
#ifndef FORMANT_ABSOLUTE
    static constexpr float f1range = 3;
    static constexpr float f2range = 4;
#endif
    switch(parameter_id){
#ifdef FORMANT_ABSOLUTE
    case PARAMETER_F1:
      osc->setFormant1(osc->getFrequency()+noteToFrequency(value*12*8+mod1));
      break;
    case PARAMETER_F2:
      osc->setFormant2(osc->getFrequency()+noteToFrequency(value*12*8+mod2+12));
      break;
#else
    case PARAMETER_F1:
      osc->setFormant1(osc->getFrequency()*getFrequencyScalar(value+mod1, f1range));
      break;
    case PARAMETER_F2:
      osc->setFormant2(osc->getFrequency()*getFrequencyScalar(value+mod2, f2range));
      break;
#endif
    case PARAMETER_WAVESHAPE:
      osc->setWaveshape(value);
      break;
    case PARAMETER_ENVELOPE:
      env->adjust(value);
      break;
    }
  }
};

class VosimSignalGenerator : public VosimSynth, public SignalGenerator  {
public:
  VosimSignalGenerator(VosimOscillator* osc, WitchEnvelope* env) : VosimSynth(osc, env){}
  using SignalGenerator::generate;
  float generate(){
    return osc->generate()*env->generate()*gain;
  }
  static VosimSignalGenerator* create(float sr){
    VosimOscillator* osc = VosimOscillator::create(sr);
    WitchEnvelope* env = WitchEnvelope::create(sr);
    return new VosimSignalGenerator(osc, env);
  }
  static void destroy(VosimSignalGenerator* obj){
    VosimOscillator::destroy(obj->osc);
    WitchEnvelope::destroy(obj->env);
    delete obj;
  }
};

class VosimSignalProcessor : public VosimSynth, public SignalProcessor {
protected:
  // FloatArray buffer;
public:
  VosimSignalProcessor(VosimOscillator* osc, WitchEnvelope* env) : VosimSynth(osc, env){}
  using SignalProcessor::process;
  using MidiProcessor::process;
  float process(float input){
    // use input as a frequency scaler
    return osc->generate(input)*env->generate()*gain;
  }
  void process(FloatArray input, FloatArray output){
    osc->generate(output, input);
    env->process(output, output);
    output.multiply(gain);
  }
  static VosimSignalProcessor* create(float sr){
    VosimOscillator* osc = VosimOscillator::create(sr);
    WitchEnvelope* env = WitchEnvelope::create(sr);
    return new VosimSignalProcessor(osc, env);
  }
  static void destroy(VosimSignalProcessor* obj){
    VosimOscillator::destroy(obj->osc);
    WitchEnvelope::destroy(obj->env);
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
    registerParameter(PARAMETER_F, "Sine LFO>");
    registerParameter(PARAMETER_G, "Witch LFO>");

    setParameterValue(PARAMETER_FX_SELECT, fx->getParameterValueForEffect(WitchMultiEffect::PHASER));

    // voices
    voices = SynthVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, SynthVoice::create(getSampleRate()));

#ifdef USE_MPE
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 0, 18+6*cvrange);
#else
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 12*cvrange, 24);
#endif
    registerParameter(PARAMETER_WAVESHAPE, "Waveshape");
    setParameterValue(PARAMETER_WAVESHAPE, 0);
  }
  ~VosimPatch(){
    for(int i=0; i<VOICES; ++i)
      SynthVoice::destroy(voices->getVoice(i));
    SynthVoices::destroy(voices);
    CvNoteProcessor::destroy(cvnote);
  }
  void processAudio(AudioBuffer &buffer) {
    doprocess(buffer);
    
    voices->setParameter(VosimSynth::PARAMETER_F1, getParameterValue(PARAMETER_B));
    voices->setParameter(VosimSynth::PARAMETER_F2, getParameterValue(PARAMETER_C));
    voices->setParameter(VosimSynth::PARAMETER_ENVELOPE, getParameterValue(PARAMETER_D));
    voices->setParameter(SynthVoice::PARAMETER_WAVESHAPE, getParameterValue(PARAMETER_WAVESHAPE));

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
#ifdef USE_MPE
    // 2 * exp2f(2x - 2) : 0.5 to 2 (plus minus one octave modulation)
    // 4 * exp2f(4x - 4) : 0.25 to 4 (plus minus two octaves modulation)
    float fm = getFrequencyScalar(getParameterValue(PARAMETER_A), cvrange*0.5) - 1;
    left.add(fm);
#endif
    voices->process(left, right);
    left.copyFrom(right);
    dofx(buffer);
    dolfo();
  }
};

#endif   // __VosimPatch_hpp__
