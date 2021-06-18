#ifndef __PolySubPatch_hpp__
#define __PolySubPatch_hpp__

#include "OpenWareLibrary.h"

#define USE_MPE
#define VOICES 8

#include "WitchFX.hpp"

class SubSynth : public AbstractSynth, public SignalGenerator, public SignalProcessor {
private:
  PolyBlepOscillator osc;
  BiquadFilter* filter;
  WitchEnvelope* env;
  SmoothFloat fc, q;
  float gain;
  float mod1 = 0;
  float mod2 = 0;
public:
  enum Parameters {
		   PARAMETER_WAVESHAPE,
		   PARAMETER_FILTER_CUTOFF,
		   PARAMETER_FILTER_RESONANCE,
		   PARAMETER_ENVELOPE,
  };
  SubSynth(float sr, BiquadFilter* f, WitchEnvelope* env) : 
    osc(sr), filter(f), env(env), fc(0.25), q(0.77), gain(1.0f) {
    env->setSustain(1.0);
    env->setDecay(0.0);
  }
  static SubSynth* create(float sr){
    // 8-pole filter 48dB
    BiquadFilter* filter = BiquadFilter::create(sr, 4);
    WitchEnvelope* env = WitchEnvelope::create(sr);
    return new SubSynth(sr, filter, env);
  }
  static void destroy(SubSynth* obj){
    WitchEnvelope::destroy(obj->env);    
    BiquadFilter::destroy(obj->filter);
    delete obj;
  }
  void setModulation(float modulation){
    mod1 = modulation;
  }
  void setPressure(float modulation){
    mod2 = modulation;
  }
  void setFrequency(float freq){
    osc.setFrequency(freq);
  }
  void setFilter(float freq, float res){
    // fc = min(0.45, max(0.05, freq));
    fc = freq;
    q = res;
  }
  void setWaveshape(float shape){
    float pw = 0.5;
    if(shape > 1.0){
      pw += 0.49*(shape-1.0); // pw 0.5 to 0.99
      shape = 1.0; // square wave
    }
    osc.setShape(shape);
    osc.setPulseWidth(pw);
  }
  void setGain(float gain){
    this->gain = gain*GAINFACTOR*0.6;
  }
  void trigger(){
    env->trigger();
  }
  void gate(bool state) {
    env->gate(state);
  }
  
  // SignalGenerator
  float generate(){
    filter->setLowPass(fc+mod1*fc, q);
    osc.setShape(mod2);
    return filter->process(osc.generate())*gain*(1-q*0.2)*env->generate();
  }
  void generate(FloatArray samples){
    filter->setLowPass(fc+mod1*fc, q);
    osc.setShape(mod2);
    osc.generate(samples);
    filter->process(samples);
    samples.multiply(gain*(1-q*0.2)); // gain compensation for high q
    env->process(samples, samples);
  }

  // SignalProcessor
  using MidiProcessor::process;
  float process(float input){
    filter->setLowPass(fc+mod1*fc, q);
    osc.setShape(mod2);
    return filter->process(osc.generate(input))*gain*(1-q*0.2)*env->generate();
  }
  void process(FloatArray fm, FloatArray output){
    filter->setLowPass(fc+mod1*fc, q);
    osc.setShape(mod2);
    // osc.generate(output);
    osc.generate(output, fm);
    filter->process(output);
    output.multiply(gain*(1-q*0.2)); // gain compensation for high q
    env->process(output, output);
  }

  void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case PARAMETER_WAVESHAPE:
      mod2 = value;
      break;
    case PARAMETER_FILTER_CUTOFF:
      fc = value*value*8000+400;
      break;
    case PARAMETER_FILTER_RESONANCE:
      q = value*3+0.75;
      break;
    case PARAMETER_ENVELOPE:
      env->adjust(value);
      break;
    }
  }
};

typedef SubSynth SynthVoice;

#if defined USE_MPE
typedef MidiPolyphonicExpressionProcessor<SynthVoice, VOICES> Allocator;
#elif VOICES == 1
typedef MonophonicProcessor<SynthVoice> Allocator;
#else
typedef PolyphonicProcessor<SynthVoice, VOICES> Allocator;
#endif

typedef VoiceAllocatorSignalProcessor<Allocator, SynthVoice, VOICES> SynthVoices;

#include "WitchPatch.hpp"

class PolySubPatch : public WitchPatch {
private:
public:
  PolySubPatch(){
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Cutoff");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_F, "Sine LFO>");
    registerParameter(PARAMETER_G, "Witch LFO>");
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.8);
    setParameterValue(PARAMETER_C, 0.2);
    setParameterValue(PARAMETER_D, 0.4);

    setParameterValue(PARAMETER_FX_SELECT, fx->getParameterValueForEffect(WitchMultiEffect::OVERDRIVE));

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
  ~PolySubPatch(){
    for(int i=0; i<VOICES; ++i)
      SynthVoice::destroy(voices->getVoice(i));
    SynthVoices::destroy(voices);
    CvNoteProcessor::destroy(cvnote);
  }

  void processAudio(AudioBuffer &buffer) {
    doprocess(buffer);

    voices->setParameter(SynthVoice::PARAMETER_FILTER_CUTOFF, getParameterValue(PARAMETER_B));
    voices->setParameter(SynthVoice::PARAMETER_FILTER_RESONANCE, getParameterValue(PARAMETER_C));
    voices->setParameter(SynthVoice::PARAMETER_ENVELOPE, getParameterValue(PARAMETER_D));

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    // float fm_amount = getParameterValue(PARAMETER_FM_AMOUNT)*0.2;
    // left.multiply(fm_amount);
    // right.clear(); // todo: mic amount?
#ifdef USE_MPE
    // 2 * exp2f(2x - 2) : 0.5 to 2 (plus minus one octave modulation)
    // 4 * exp2f(4x - 4) : 0.25 to 4 (plus minus two octaves modulation)
    float fm = exp2f(cvrange*getParameterValue(PARAMETER_A) - cvrange*0.5) - 1;    
    left.add(fm);
#endif
    voices->process(left, right);
    left.copyFrom(right);
    dofx(buffer);
    dolfo();
  }
};

#endif   // __PolySubPatch_hpp__
