#ifndef __QuadSamplerPatch_hpp__
#define __QuadSamplerPatch_hpp__

#include "OpenWareLibrary.h"

static const int TRIGGER_LIMIT = (1<<17);

#define VOICES 4
#define BUTTON_VELOCITY 100

#include "common.h"

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
    decay = decay*decay*oscL->getSampleLength()/240;
    env->setDecay(decay);
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
    // left.tanh();
    // right.tanh();
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

class QuadSamplerPatch : public Patch {
private:
  SynthVoices* voices;
  int basenote = 60;
  AgnesiOscillator* lfo1;
  ExponentialDecayEnvelope* env;
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
    Resource::destroy(resource);
    // filter sample
    DcBlockingFilter* filter = DcBlockingFilter::create();
    filter->process(sampleL, sampleL);
    filter->reset();
    filter->process(sampleR, sampleR);
    DcBlockingFilter::destroy(filter);
    // normalise levels
    float pkL = max(sampleL.getMaxValue(), -sampleL.getMinValue());
    float pkR = max(sampleR.getMaxValue(), -sampleR.getMinValue());
    float pk = max(pkL, pkR);
    sampleL.multiply(1/pk);
    // debugMessage("rms", sampleR.getRms(), sampleL.getRms());
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
    lfo1 = AgnesiOscillator::create(getBlockRate(), 0.5, 10);
    registerParameter(PARAMETER_E, "LFO");
    registerParameter(PARAMETER_F, "LFO>");
    registerParameter(PARAMETER_G, "Envelope>");

    env = ExponentialDecayEnvelope::create(getBlockRate());
  }

  ~QuadSamplerPatch() {
    AgnesiOscillator::destroy(lfo1);
    for(int i=0; i<VOICES; ++i){
      FloatArray::destroy(voices->getVoice(i)->getOscillator(LEFT_CHANNEL)->getSample());
      FloatArray::destroy(voices->getVoice(i)->getOscillator(RIGHT_CHANNEL)->getSample());
      StereoSamplerVoice::destroy(voices->getVoice(i));
    }
    SynthVoices::destroy(voices);
    ExponentialDecayEnvelope::destroy(env);
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

    float decay = getParameterValue(PARAMETER_D);
    env->setDecay(decay*decay*40);

    // lfo
    lfo1->setFrequency(getParameterValue(PARAMETER_E)*2);
    setParameterValue(PARAMETER_F, lfo1->generate());
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    setParameterValue(PARAMETER_G, env->generate());
    setButton(BUTTON_F, fmodf(lfo1->getPhase(), M_PI/2) < M_PI/4);
  }
};

#endif // __StereoSamplerPatch_hpp__
