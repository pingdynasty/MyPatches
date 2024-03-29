#ifndef __QuadSamplerPatch_hpp__
#define __QuadSamplerPatch_hpp__

#include "OpenWareLibrary.h"

static const int TRIGGER_LIMIT = (1<<17);

#define VOICES 4
#define BUTTON_VELOCITY 100
#define FX_SELECT FX_DELAY

#include "common.h"
#include "WitchFX.hpp"

// typedef SampleOscillator<LINEAR_INTERPOLATION> Sampler;
typedef SampleOscillator<COSINE_INTERPOLATION> Sampler;
// typedef SampleOscillator<CUBIC_4P_SMOOTH_INTERPOLATION> Sampler;

class StereoSamplerVoice : public AbstractSynth {
private:
  Sampler* oscL;
  Sampler* oscR;
  ExponentialDecayEnvelope* env;
  float gain;
  SmoothFloat decaytime;
  StereoBiquadFilter* filter;
  FloatArray buffer;
public:
  StereoSamplerVoice(float sr, size_t bs, FloatArray left, FloatArray right)
    : gain(1.0f) {
    oscL = Sampler::create(sr, left);
    oscR = Sampler::create(sr, right);
    // oscL->setRepeatMode(Sampler::REPEAT_FORWARD);
    // oscR->setRepeatMode(Sampler::REPEAT_FORWARD);
    env = ExponentialDecayEnvelope::create(sr);
    filter = StereoBiquadFilter::create(sr, 2);
    buffer = FloatArray::create(bs);
  }
  static StereoSamplerVoice* create(float sr, size_t bs, FloatArray left, FloatArray right) {
    return new StereoSamplerVoice(sr, bs, left, right);
  }
  static void destroy(StereoSamplerVoice* obj) {
    Sampler::destroy(obj->oscL);
    Sampler::destroy(obj->oscR);
    FloatArray::destroy(obj->buffer);
    ExponentialDecayEnvelope::destroy(obj->env);
    StereoBiquadFilter::destroy(obj->filter);
    delete obj;
  }
  void setFrequency(float freq) {
    oscL->setFrequency(freq);
    oscR->setFrequency(freq);
    freq *= 30;
    freq = min(18000.0f, freq);
    // filter->setLowPass(freq, FilterStage::BUTTERWORTH_Q);
  }
  void setDecay(float decay) {
    decay = decay*decay*oscL->getSampleLength()*2 + 200;
    env->setDecaySamples(decay);
  }
  void setGain(float value) {
    gain = value;
  }
  void gate(bool state) {
    // env->gate(state);
    if(state){
      env->trigger(state);
      oscL->reset();
      oscR->reset();
    }
  }
  void trigger() {
    oscL->reset();
    oscR->reset();
    env->trigger();
  }
  void generate(AudioBuffer& output){
    // oscL->setDuration(duration);
    // oscR->setDuration(duration);    
    FloatArray left = output.getSamples(LEFT_CHANNEL);
    FloatArray right = output.getSamples(RIGHT_CHANNEL);
    oscL->generate(left);
    oscR->generate(right);
    env->generate(buffer);
    buffer.multiply(gain);
    left.multiply(buffer);
    right.multiply(buffer);
    filter->process(output, output);    
  }
  float filter_fc = 8000;
  float filter_q = 4;
  float filter_gain = 0;
  virtual void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case 1:
      filter_fc = value;
      filter->setPeak(filter_fc, filter_q, filter_gain);
      break;
    case 2:
      filter_q = value;
      break;
    case 3:
      filter_gain = value;
      break;
    case 4:
      setDecay(value);
      break;
    }
  }
  Sampler* getOscillator(size_t channel){
    if(channel == 0)
      return oscL;
    else if(channel == 1)
      return oscR;
    return NULL;
  }
};

typedef StereoSamplerVoice SynthVoice;

#if defined USE_MPE
typedef MidiPolyphonicExpressionProcessor<SynthVoice, VOICES> Allocator;
#elif VOICES == 1
typedef MonophonicProcessor<SynthVoice> Allocator;
#else
typedef PolyphonicProcessor<SynthVoice, VOICES> Allocator;
#endif

typedef VoiceAllocatorMultiSignalGenerator<Allocator, SynthVoice, VOICES> SynthVoices;

#include "WitchPatch.hpp"

class QuadSamplerPatch : public AbstractWitchPatch {
private:
  SynthVoices* voices;
  int basenote = 60;
  WitchLFO* lfo1;
  ExponentialDecayEnvelope* env;
  WitchMultiEffect* fx;
public:

  StereoSamplerVoice* createVoice(const char* name){
    // load sample
    Resource* resource = getResource(name);
    WavFile wav(resource->getData(), resource->getSize());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
    FloatArray sampleL = wav.createFloatArray(0);
    FloatArray sampleR = wav.createFloatArray(1);
    // debugMessage("wav", (int)wav.getNumberOfChannels(), wav.getNumberOfSamples(), wav.getSize());
    // debugMessage("wav", (int)wav.getAudioFormat(), wav.getBitsPerSample());
    // debugMessage("wav", (int)sampleL.getSize(), sampleR.getSize()));
    Resource::destroy(resource);
    // filter sample to remove DC offset
    DcBlockingFilter* filter = DcBlockingFilter::create();
    filter->process(sampleL, sampleL);
    filter->reset();
    filter->process(sampleR, sampleR);
    DcBlockingFilter::destroy(filter);
    // // normalise levels
    float pkL = max(sampleL.getMaxValue(), -sampleL.getMinValue());
    float pkR = max(sampleR.getMaxValue(), -sampleR.getMinValue());
    float pk = max(pkL, pkR);
    sampleL.multiply(1/pk);
    sampleR.multiply(1/pk);
    return StereoSamplerVoice::create(getSampleRate(), getBlockSize(), sampleL, sampleR);
  }
  QuadSamplerPatch() {
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Cutoff");
    registerParameter(PARAMETER_C, "Peak");
    registerParameter(PARAMETER_D, "Decay");

    // create voices
    voices = SynthVoices::create(2, getBlockSize());
    voices->setVoice(0, createVoice("sample1.wav"));
    voices->setVoice(1, createVoice("sample2.wav"));
    voices->setVoice(2, createVoice("sample3.wav"));
    voices->setVoice(3, createVoice("sample4.wav"));
    
    // lfo
    lfo1 = WitchLFO::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate(), getBlockSize());
    registerParameter(PARAMETER_E, "LFO");
    registerParameter(PARAMETER_F, "LFO>");
    registerParameter(PARAMETER_G, "Envelope>");

    env = ExponentialDecayEnvelope::create(getBlockRate());

    // fx
    fx = WitchMultiEffect::create(getSampleRate(), getBlockSize());
    registerParameter(PARAMETER_H, "FX Amount");
    restore();
  }

  ~QuadSamplerPatch() {
    WitchLFO::destroy(lfo1);
    for(int i=0; i<VOICES; ++i){
      FloatArray::destroy(voices->getVoice(i)->getOscillator(LEFT_CHANNEL)->getSample());
      FloatArray::destroy(voices->getVoice(i)->getOscillator(RIGHT_CHANNEL)->getSample());
      StereoSamplerVoice::destroy(voices->getVoice(i));
    }
    SynthVoices::destroy(voices);
    ExponentialDecayEnvelope::destroy(env);
    WitchMultiEffect::destroy(fx);
  }
    
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    StereoSamplerVoice* voice = NULL;
    switch(bid){
    case BUTTON_1:
      voice = voices->getVoice(0);
      break;
    case BUTTON_2:
      voice = voices->getVoice(1);
      break;
    case BUTTON_3:
      voice = voices->getVoice(2);
      break;
    case BUTTON_4:
      voice = voices->getVoice(3);
      break;
    }
    if(voice){
      if(value){
	voice->noteOn(MidiMessage::note(0, basenote, BUTTON_VELOCITY));
      }else{
	voice->gate(false);
      }
    }
    if(value)
      env->trigger();
  }
  void processMidi(MidiMessage msg){
    voices->process(msg);
    if(msg.isControlChange()){
      uint8_t value = msg.getControllerValue();
      switch(msg.getControllerNumber()){
      case PATCH_PARAMETER_LFO1_SHAPE:
	lfo1->select(value/128.0f);
	break;
      case PARAMETER_DYNAMIC_RANGE:
	voices->setDynamicRange(value);
	break;
      case PARAMETER_FX_SELECT:
	fx->select(value/128.0f);
	break;
      default:
	AbstractWitchPatch::processMidi(msg);
	break;
      }
    }
  }

  void processAudio(AudioBuffer& buffer) {
    basenote = getParameterValue(PARAMETER_A)*48+40;

    float cutoff = getParameterValue(PARAMETER_B);
    cutoff = cutoff*cutoff*12000+400;
    float gain = getParameterValue(PARAMETER_C)*24-12;
    voices->setParameter(3, gain);
    voices->setParameter(1, cutoff);
    voices->setParameter(4, getParameterValue(PARAMETER_D));
    
    voices->generate(buffer);

    // decay envelope
    float decay = getParameterValue(PARAMETER_D);
    env->setDecay(decay*decay*4);

    // lfo
    float freq = getParameterValue(PARAMETER_E);
    freq = freq*freq*7.9+0.1; // 0.1 to 8 Hz
    lfo1->setFrequency(freq);
    float lfo = lfo1->generate()*0.5+0.5;
    float phase = lfo1->getPhase();
    setParameterValue(PARAMETER_F, lfo);
    setButton(BUTTON_E, phase < M_PI);
    setParameterValue(PARAMETER_G, env->generate());
    setButton(BUTTON_F, fmodf(phase, M_PI/2) < M_PI/4);

    // fx
    fx->setFrequency(freq);
    fx->setEffect(getParameterValue(PARAMETER_WAVESHAPE));
    fx->process(buffer, buffer);
}
};

#endif // __QuadSamplerPatch_hpp__
