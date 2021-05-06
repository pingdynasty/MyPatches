#ifndef __StereoSamplerPatch_hpp__
#define __StereoSamplerPatch_hpp__

#include "OpenWareLibrary.h"
#include "TapTempo.hpp"

static const int TRIGGER_LIMIT = (1<<17);

#define SAMPLE_NAME "sample1.wav"
#define POLYPHONY 2
#define BUTTON_VELOCITY 100

class StereoSamplerVoice : public AbstractSynth {
private:
  SampleOscillator* oscL;
  SampleOscillator* oscR;
  AdsrEnvelope* env;
  float gain;
  Control<PARAMETER_D> attack_release = 0.5f;
  Control<PARAMETER_AB> decay = 0.0f;
  Control<PARAMETER_AC> sustain = 1.0f;
  // Control<PARAMETER_AE> portamento = 0.0f;
  Control<PARAMETER_E> duration = 1.0f;
  StereoBiquadFilter* filter;
  FloatArray buffer;
public:
  StereoSamplerVoice(float sr, size_t bs, FloatArray left, FloatArray right)
    : gain(1.0f) {
    oscL = SampleOscillator::create(sr, left);
    oscR = SampleOscillator::create(sr, right);
    env = AdsrEnvelope::create(sr);
    filter = StereoBiquadFilter::create(sr, 2);
    buffer = FloatArray::create(bs);
  }
  static StereoSamplerVoice* create(float sr, size_t bs, FloatArray left, FloatArray right) {
    return new StereoSamplerVoice(sr, bs, left, right);
  }
  static void destroy(StereoSamplerVoice* obj) {
    SampleOscillator::destroy(obj->oscL);
    SampleOscillator::destroy(obj->oscR);
    FloatArray::destroy(obj->buffer);
    AdsrEnvelope::destroy(obj->env);
    StereoBiquadFilter::destroy(obj->filter);
    delete obj;
  }
  void setFrequency(float freq) {
    oscL->setFrequency(freq);
    oscR->setFrequency(freq);
    freq *= 30;
    freq = min(18000, freq);
    // filter->setLowPass(freq, FilterStage::BUTTERWORTH_Q);
  }
  void setEnvelope() {
    float df = attack_release*3;
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
    env->setDecay(decay);
    env->setSustain(sustain);
    env->setRelease(release);
  }
  void setGain(float value) {
    gain = value;
  }
  void gate(bool state) {
    env->gate(state);
    if(state){
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
    setEnvelope();
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
    left.tanh();
    right.tanh();
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
    }
  }
};

#if defined USE_MPE
typedef MidiPolyphonicExpressionMultiSignalGenerator<StereoSamplerVoice, POLYPHONY> VoiceProcessor;
#elif POLYPHONY == 1
typedef MonophonicMultiSignalGenerator<StereoSamplerVoice> VoiceProcessor;
#else
typedef PolyphonicMultiSignalGenerator<StereoSamplerVoice, POLYPHONY> VoiceProcessor;
#endif

class StereoSamplerPatch : public Patch {
private:
  VoiceProcessor* voices;
  int basenote = 60;

  SineOscillator* lfo1;
  AgnesiOscillator* lfo2;
  TapTempo<TRIGGER_LIMIT> tempo1;
  TapTempo<TRIGGER_LIMIT> tempo2;
  FloatArray sampleL, sampleR;
public:

  StereoSamplerPatch() :
    tempo1(getSampleRate()*0.5), tempo2(getSampleRate()*0.25) {
    registerParameter(PARAMETER_A, "Frequency");
    setParameterValue(PARAMETER_A, 0.8);
    registerParameter(PARAMETER_AB, "Decay");
    registerParameter(PARAMETER_AC, "Sustain");
    //registerParameter(PARAMETER_AE, "Portamento");
    // registerParameter(PARAMETER_AF, "Duration");

    registerParameter(PARAMETER_B, "Cutoff");
    setParameterValue(PARAMETER_B, 0.5);
    registerParameter(PARAMETER_C, "Peak");
    setParameterValue(PARAMETER_C, 0.5);

    // load sample
    Resource* resource = getResource(SAMPLE_NAME);
    WavFile wav(resource->getData(), resource->getSize());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
    sampleL = wav.createFloatArray(0);
    sampleR = wav.createFloatArray(1);
    debugMessage("wav", (int)wav.getNumberOfChannels(), wav.getNumberOfSamples(), wav.getSize());
    Resource::destroy(resource);
    
    // filter sample
    // BiquadFilter* filter = BiquadFilter::create(getSampleRate(), 2);
    // // filter->setHighPass();
    // filter->setLowPass(6000, FilterStage::BUTTERWORTH_Q);
    // filter->process(sample, sample);
    // BiquadFilter::destroy(filter);
    DcBlockingFilter* filter = DcBlockingFilter::create();
    filter->process(sampleL, sampleL);
    filter->reset();
    filter->process(sampleR, sampleR);
    DcBlockingFilter::destroy(filter);

    // create voices
    voices = VoiceProcessor::create(2, getBlockSize());
    for(size_t i=0; i<POLYPHONY; ++i)
      voices->setVoice(i, StereoSamplerVoice::create(getSampleRate(), getBlockSize(), sampleL, sampleR));
    
      // lfo
    lfo1 = SineOscillator::create(getBlockRate());
    // lfo2 = SineOscillator::create(getBlockRate());
    lfo2 = AgnesiOscillator::create(getBlockRate(), 0.5, 10);
    registerParameter(PARAMETER_F, "LFO1>");
    registerParameter(PARAMETER_G, "LFO2>");
  }

  ~StereoSamplerPatch() {
    SineOscillator::destroy(lfo1);
    // SineOscillator::destroy(lfo2);
    AgnesiOscillator::destroy(lfo2);
    for(int i=0; i<POLYPHONY; ++i)
      StereoSamplerVoice::destroy(voices->getVoice(i));
    VoiceProcessor::destroy(voices);
    FloatArray::destroy(sampleL);
    FloatArray::destroy(sampleR);
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
      tempo1.trigger(value, samples);
      if(value)
	lfo1->reset();
      // note = basenote+7;
      break;
    case BUTTON_4:
      tempo2.trigger(value, samples);
      if(value)
	lfo2->reset(); // todo: instead of hard reset, calculate to sync on next edge
      // note = basenote+12;
      break;
    // case BUTTON_3:
    //   if(value){
    // 	note = basenote+7;
    // 	lastnote[2] = note;
    // 	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
    //   }else{
    // 	voices->noteOff(MidiMessage::note(0, lastnote[2], 0));
    //   }
    //   break;
    // case BUTTON_4:
    //   if(value){
    // 	note = basenote+12;
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

  void processAudio(AudioBuffer& buffer) {
    basenote = getParameterValue(PARAMETER_A)*48+40;

    float cutoff = getParameterValue(PARAMETER_B);
    cutoff = cutoff*cutoff*12000+400;
    float gain = getParameterValue(PARAMETER_C)*24-12;
    voices->setParameter(3, gain);
    voices->setParameter(1, cutoff);
    
    voices->generate(buffer);

    // lfo
    tempo1.clock(getBlockSize());
    tempo2.clock(getBlockSize());
    float rate = getSampleRate()/tempo1.getSamples();
    lfo1->setFrequency(rate);
    setParameterValue(PARAMETER_F, lfo1->generate()*0.5+0.5);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    rate = getSampleRate()/tempo2.getSamples();
    lfo2->setFrequency(rate);
    // setParameterValue(PARAMETER_G, lfo2->generate()*0.5+0.5);
    setParameterValue(PARAMETER_G, lfo2->generate());
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
}
};

#endif // __StereoSamplerPatch_hpp__
