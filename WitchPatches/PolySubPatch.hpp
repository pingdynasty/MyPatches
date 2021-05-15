#ifndef __PolySubPatch_hpp__
#define __PolySubPatch_hpp__

#include "OpenWareLibrary.h"
#include "WitchPatch.hpp"

#define VOICES 4
#define GAINFACTOR 0.35
#define BUTTON_VELOCITY 100
#define TRIGGER_LIMIT (1<<22)

class SubSynth : public AbstractSynth, public SignalGenerator {
private:
  PolyBlepOscillator osc;
  BiquadFilter* filter;
  AdsrEnvelope* env;
  SmoothFloat fc, q;
  float gain;
public:
  enum Parameters {
		   PARAMETER_WAVESHAPE,
		   PARAMETER_FILTER_CUTOFF,
		   PARAMETER_FILTER_RESONANCE,
		   PARAMETER_ENVELOPE,
  };
  SubSynth(float sr, BiquadFilter* f, AdsrEnvelope* env) : 
    osc(sr), filter(f), env(env), fc(0.25), q(0.77), gain(1.0f) {
    env->setSustain(1.0);
    env->setDecay(0.0);
  }
  static SubSynth* create(float sr){
    // 8-pole filter 48dB
    BiquadFilter* filter = BiquadFilter::create(sr, 4);
    AdsrEnvelope* env = AdsrEnvelope::create(sr);
    return new SubSynth(sr, filter, env);
  }
  static void destroy(SubSynth* obj){
    AdsrEnvelope::destroy(obj->env);    
    BiquadFilter::destroy(obj->filter);
    delete obj;
  }
  void setFrequency(float freq){
    osc.setFrequency(freq);
  }
  void setFilter(float freq, float res){
    fc = min(0.45, max(0.05, freq));
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
  void setGain(float gain){
    this->gain = gain*GAINFACTOR;
  }
  void trigger(){
    env->trigger();
  }
  void gate(bool state) {
    env->gate(state);
  }
  float generate(){
    filter->setLowPass(fc, q);
    return filter->process(osc.generate())*gain*(1-q*0.2)*env->generate();
  }
  void generate(FloatArray samples){
    filter->setLowPass(fc, q);
    osc.generate(samples);
    filter->process(samples);
    samples.multiply(gain*(1-q*0.2)); // gain compensation for high q
    env->process(samples, samples);
  }
  void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case PARAMETER_WAVESHAPE:
      setWaveshape(value*2);
      break;
    case PARAMETER_FILTER_CUTOFF:
      fc = value*value*12000+400;
      break;
    case PARAMETER_FILTER_RESONANCE:
      q = value*3+0.75;
      break;
    case PARAMETER_ENVELOPE:
      setEnvelope(value*value*3);
      break;
    }
  }
};

#if VOICES == 1
typedef MonophonicSignalGenerator<SubSynth> SynthVoices;
#else
typedef PolyphonicSignalGenerator<SubSynth, VOICES> SynthVoices;
#endif

class PolySubPatch : public Patch {
private:
  SynthVoices* voices;
  TapTempoSineOscillator* lfo1;
  TapTempoAgnesiOscillator* lfo2;
  StereoPhaserProcessor phaser;
  CvNoteProcessor* cvnote;
public:
  PolySubPatch() {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Cutoff");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_E, "Waveshape");
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.8);
    setParameterValue(PARAMETER_C, 0.2);
    setParameterValue(PARAMETER_D, 0.4);
    setParameterValue(PARAMETER_E, 0.5);

    // voices
    voices = SynthVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, SubSynth::create(getSampleRate()));

    // lfo
    lfo1 = TapTempoSineOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo2 = TapTempoAgnesiOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo1->setBeatsPerMinute(60);
    lfo2->setBeatsPerMinute(120);

    cvnote = CvNoteProcessor::create(getSampleRate(), 3, voices);
  }
  ~PolySubPatch(){
    for(int i=0; i<VOICES; ++i)
      SubSynth::destroy(voices->getVoice(i));
    SynthVoices::destroy(voices);
    TapTempoSineOscillator::destroy(lfo1);
    TapTempoAgnesiOscillator::destroy(lfo2);
    CvNoteProcessor::destroy(cvnote);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      cvnote->gate(value, samples);
      break;
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
	if(!sustain)
	  voices->allNotesOff();
      }
      setButton(BUTTON_4, sustain);
      break;
    }
  }
  
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }

  void processAudio(AudioBuffer &buffer) {
    cvnote->cv(getParameterValue(PARAMETER_A));
    cvnote->clock(getBlockSize());

    voices->setParameter(SubSynth::PARAMETER_FILTER_CUTOFF, getParameterValue(PARAMETER_B));
    voices->setParameter(SubSynth::PARAMETER_FILTER_RESONANCE, getParameterValue(PARAMETER_C));
    voices->setParameter(SubSynth::PARAMETER_ENVELOPE, getParameterValue(PARAMETER_D));
    phaser.setEffect(getParameterValue(PARAMETER_E));

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

#endif   // __PolySubPatch_hpp__
