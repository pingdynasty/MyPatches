#ifndef __SpiralSamplerPatch_hpp__
#define __SpiralSamplerPatch_hpp__

#include "OpenWareLibrary.h"
#include "TapTempo.hpp"

static const int TRIGGER_LIMIT = (1<<17);

#define SAMPLE_NAME "sample1.wav"
// #define SAMPLE_NAME "wavetable1.wav"
// #define SAMPLE_NAME "sample.wav"
#define MIDDLE_C 261.6
#define POLYPHONY 8

/*
  need a buffer that can be read in a circular fashion
 */
class SampleOscillator : public Oscillator {
private:
    const float fs;
    FloatArray buffer;
    float pos = 0.0f;
    uint16_t size;
    float rate = 1.0f;

public:
  SampleOscillator(float sr, FloatArray sample)
    : fs(sr)
    , buffer(sample) {
    size = buffer.getSize() - 1;
  }
  void setFrequency(float freq) {
    rate = freq / MIDDLE_C;
  }
  // void setPortamento(float portamento){
  //   rate.lambda = portamento*0.19 + 0.8;
  // }
  void setSample(FloatArray sample){
    buffer = sample;
    size = buffer.getSize() -1;
  }
  void reset() {
    pos = 0.0f;
  }
  static float interpolate(float index, FloatArray data) {
    size_t idx = (int)index;
    float low = data[idx];
    float high = data[idx + 1];
    float frac = index - idx;
    return low * frac + high * (1.0 - frac);
  }
  void setPhase(float phase){
    pos = size*phase/(2*M_PI);
  }
  float getPhase(){
    return pos*2*M_PI/size;
  }
  using Oscillator::generate;
  float generate(float fm) {
    return generate();
  }
  float generate() {
    float sample = 0.0f;
    if(pos >= 0.0f && pos < size){
      sample = interpolate(pos, buffer);
      pos += rate;
    }
    return sample;
  }
  // void generate(FloatArray out){
  //   // todo: unwind interpolation
  // }
  uint16_t findZeroCrossing(uint16_t index) {
    uint16_t i = min(index, size);
    if (buffer[i] > 0)
      while (i < size && buffer[i] > 0)
	i++;
    else
      while (i < size && buffer[i] < 0)
	i++;
    return i;
  }
  void setDuration(uint16_t samples) {
    size = findZeroCrossing(samples);
  }
  /* @return the audio length in samples */
  uint16_t getSampleLength() {
    return buffer.getSize();
  }
  static SampleOscillator* create(float sr, FloatArray buf) {
    return new SampleOscillator(sr, buf);
  }
  static void destroy(SampleOscillator* obj) {
    delete obj;
  }
};

class SamplerVoice : public AbstractSynth {
private:
  SampleOscillator* osc;
  AdsrEnvelope* env;
  float gain;
  Control<PARAMETER_D> attack_release = 0.5f;
  Control<PARAMETER_AB> decay = 0.0f;
  Control<PARAMETER_AC> sustain = 1.0f;
  // Control<PARAMETER_AE> portamento = 0.0f;
  Control<PARAMETER_AF> duration = 1.0f;
public:
  SamplerVoice(float sr, FloatArray buf)
    : gain(1.0f) {
    osc = SampleOscillator::create(sr, buf);
    env = AdsrEnvelope::create(sr);
    env->setSustain(1.0);
    env->setDecay(0.0);
    env->setRelease(0.0);
  }
  static SamplerVoice* create(float sr) {
    return new SamplerVoice(sr, FloatArray());
  }
  static SamplerVoice* create(float sr, FloatArray buf) {
    return new SamplerVoice(sr, buf);
  }
  static void destroy(SamplerVoice* obj) {
    SampleOscillator::destroy(obj->osc);
    AdsrEnvelope::destroy(obj->env);
    delete obj;
  }
  void setSample(FloatArray sample){
    osc->setSample(sample);
  }
  void setFrequency(float freq) {
    osc->setFrequency(freq);
  }
  void setEnvelope() {
    float df = attack_release*4;
    int di = (int)df;
    float attack, release;
    switch(di){
      // a/d
    case 0: // l/s
      attack = 1.0-df;
      release = 0.0;
      break;
    case 1: // s/s
      attack = 0.0;
      release = df-1;
      break;
    case 2: // s/l
      attack = df-2;
      release = 1.0;
      break;
    case 3: // l/l
      attack = 1.0;
      release = 1.0;
      break;
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
    if(state)
      osc->reset();
  }
  void trigger() {
    osc->reset();
    env->trigger();
  }
  float generate() {
    setEnvelope();
    osc->setDuration(duration * osc->getSampleLength());
    return osc->generate()*env->generate()*gain;
  }
  void generate(FloatArray samples) {
    setEnvelope();
    osc->setDuration(duration * osc->getSampleLength());
    osc->generate(samples);
    samples.multiply(gain);
    env->process(samples, samples);
  }
};

class SpiralSamplerPatch : public Patch {
private:
  PolyphonicProcessor<SamplerVoice, POLYPHONY>* voices;
  int basenote = 60;

  SineOscillator* lfo1;
  SineOscillator* lfo2;
  TapTempo<TRIGGER_LIMIT> tempo1;
  TapTempo<TRIGGER_LIMIT> tempo2;
public:

  SpiralSamplerPatch() :
    tempo1(getSampleRate()*0.5), tempo2(getSampleRate()*0.25) {
    registerParameter(PARAMETER_A, "Frequency");
    setParameterValue(PARAMETER_A, 0.8);
    registerParameter(PARAMETER_AB, "Decay");
    registerParameter(PARAMETER_AC, "Sustain");
    //registerParameter(PARAMETER_AE, "Portamento");
    registerParameter(PARAMETER_AF, "Duration");

    // load sample and create voices
    Resource* resource = getResource(SAMPLE_NAME);
    WavFile wav = WavFile::create(resource->getData());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
    FloatArray sample = wav.createFloatArray();
    Resource::destroy(resource);
    voices = PolyphonicProcessor<SamplerVoice, POLYPHONY>::create(getBlockSize());
    for(size_t i=0; i<POLYPHONY; ++i)
      voices->setVoice(i, SamplerVoice::create(getSampleRate(), sample));

      // lfo
    lfo1 = SineOscillator::create(getBlockRate());
    lfo2 = SineOscillator::create(getBlockRate());
    registerParameter(PARAMETER_F, "LFO1>");
    registerParameter(PARAMETER_G, "LFO2>");
}

  ~SpiralSamplerPatch() {
    SineOscillator::destroy(lfo1);
    SineOscillator::destroy(lfo2);
    for(int i=0; i<POLYPHONY; ++i)
      SamplerVoice::destroy(voices->getVoice(i));
    PolyphonicProcessor<SamplerVoice, POLYPHONY>::destroy(voices);
  }
    
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples) {
    int note = 0;
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      note = basenote;
      break;
    case BUTTON_B:
      note = basenote+3;
      break;
    case BUTTON_C:
      tempo1.trigger(value, samples);
      if(value)
	lfo1->reset();
      // note = basenote+7;
      break;
    case BUTTON_D:
      tempo2.trigger(value, samples);
      if(value)
	lfo2->reset(); // todo: instead of hard reset, calculate to sync on next edge
      // note = basenote+12;
      break;
    }
    if(note && value)
      voices->noteOn(MidiMessage::note(0, note, 80));
    if (bid == PUSHBUTTON) {
      if(value)
	voices->allNotesOn();
      else
	voices->allNotesOff();
    }
  }

  void processMidi(MidiMessage msg){
    voices->process(msg);
  }    

  void processAudio(AudioBuffer& buffer) {
    basenote = getParameterValue(PARAMETER_A)*48+40;

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices->generate(left);
    right.copyFrom(left);

    // lfo
    tempo1.clock(getBlockSize());
    tempo2.clock(getBlockSize());
    float rate = getSampleRate()/tempo1.getSamples();
    lfo1->setFrequency(rate);
    setParameterValue(PARAMETER_F, lfo1->generate()*0.5+0.5);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    rate = getSampleRate()/tempo2.getSamples();
    lfo2->setFrequency(rate);
    setParameterValue(PARAMETER_G, lfo2->generate()*0.5+0.5);
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
}
};

#endif // __ResSamplerPatch_hpp__
