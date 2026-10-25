#ifndef __SamplerPatch_hpp__
#define __SamplerPatch_hpp__

#include "Patch.h"
#include "OpenWareLibrary.h"
#define SAMPLENAME "sample1.wav"
#define MIDDLE_C 261.6

#define MIDI_VOICES 8

/*
  need a buffer that can be read in a circular fashion
 */
class MySampleOscillator : public Oscillator {
private:
  const float fs;
  FloatArray buffer;
  float pos = 0.0f;
  size_t size;
  float rate = 1.0f;
public:
  MySampleOscillator(float sr, FloatArray sample) :
    fs(sr), buffer(sample){
    size = buffer.getSize()-1;
  }
  float getFrequency(){
    return rate*MIDDLE_C;
  }
  void setFrequency(float freq){
    rate = freq/MIDDLE_C;
  }
  // void setPortamento(float portamento){
  //   rate.lambda = portamento*0.19 + 0.8;
  // }
  void reset(){
    pos = 0.0f;
  }
  void setPhase(float phase){
    pos = size*phase/(2*M_PI);
  }
  float getPhase(){
    return pos*2*M_PI/size;
  }
  void increment(){
    pos += rate;
    if(pos < 0.0f)
      pos += size;
    else if(pos > size)
      pos -= size;
  }
  static float interpolate(float index, FloatArray data){
    int idx = (int)index;
    float low = data[idx];
    float high = data[idx+1];
    float frac = index - idx;
    return low*frac + high*(1.0-frac);    
  }
  float generate(float fm){
    return generate();
  }
  float generate(){
    increment();
    return interpolate(pos, buffer);
  }
  using Oscillator::generate;
  size_t findZeroCrossing(size_t index){
    size_t limit = buffer.getSize()-1;
    size_t i = min(index, limit);
    if(buffer[i] > 0)
      while(i < limit && buffer[i] > 0)
	i++;
    else
      while(i < limit && buffer[i] < 0)
	i++;
    return i;
  }
  void setDuration(size_t samples){
    size = findZeroCrossing(samples);
  }
  /* @return the audio length in samples */
  size_t getSampleLength(){
    return buffer.getSize();
  }
  // void getSamples(FloatArray out){
  //   // unwind interpolation
  // }
};

class SamplerVoice {
private:
  MySampleOscillator osc;
  ExponentialAdsrEnvelope env;
  float gain;
  Control<PARAMETER_AA> attack = 0.0f;
  Control<PARAMETER_AB> decay = 0.0f;
  Control<PARAMETER_AC> sustain = 1.0f;
  Control<PARAMETER_AD> release = 0.0f;
  // Control<PARAMETER_AE> portamento = 0.0f;
  Control<PARAMETER_AF> duration = 1.0f;
public:
  SamplerVoice(float sr, FloatArray buf) : 
    osc(sr, buf), env(sr), gain(1.0f) {
    env.setSustain(1.0);
    env.setDecay(0.0);
    env.setRelease(0.0);
  }
  static SamplerVoice* create(float sr, FloatArray buf){
    return new SamplerVoice(sr, buf);
  }
  static void destroy(SamplerVoice* voice){
    delete voice;
  }
  void setFrequency(float freq){
    // osc.setPortamento(portamento); // portamento won't work across voices
    osc.setFrequency(freq);
  }
  void setEnvelope(float att, float rel){
    // env.setAttack(att);
    // env.setRelease(rel);
    env.setAttack((float)attack+att);
    env.setDecay(decay);
    env.setSustain(sustain);
    env.setRelease((float)release+rel);
  }
  void setGain(float value){
    gain = value;
  }
  void setGate(bool state, int delay){
    env.gate(state, delay);
    if(state)
      osc.reset();
  }
  void generate(FloatArray samples){
    osc.setDuration(duration*osc.getSampleLength());
    osc.generate(samples);
    samples.multiply(gain);
    env.process(samples, samples);
  }
};

template<int VOICES>
class Voices {
private:
  static const uint8_t EMPTY = 0xff;
  static const uint16_t TAKEN = 0xffff;
  SamplerVoice* voice[VOICES];
  uint8_t notes[VOICES];
  uint16_t allocation[VOICES];
  uint16_t allocated;
  FloatArray buffer;

private:
  void take(uint8_t ch, uint8_t note, uint16_t velocity, uint16_t samples){
    notes[ch] = note;
    allocation[ch] = TAKEN;
    voice[ch]->setGain(velocity/(4095.0f*(VOICES/2)));
    voice[ch]->setGate(true, samples);
  }

  void release(uint8_t ch, uint16_t samples){
    // notes[ch] = EMPTY;
    allocation[ch] = ++allocated;
    voice[ch]->setGate(false, samples);
  }

public:
  Voices() : allocated(0) {}
  
  void init(float sr, int bs, FloatArray buf){
    for(int i=0; i<VOICES; ++i){
      voice[i] = SamplerVoice::create(sr, buf);
      notes[i] = 69; // middle A, 440Hz
      allocation[i] = 0;
    }
    buffer = FloatArray::create(bs);
  }

  ~Voices(){
    for(int i=0; i<VOICES; ++i)
      SamplerVoice::destroy(voice[i]);
    FloatArray::destroy(buffer);
  }

  void allNotesOff(){
    for(int i=0; i<VOICES; ++i)
      release(i, 0);
    allocated = 0;
  }

  void allNotesOn(){
    for(int i=0; i<VOICES; ++i){
      voice[i]->setGain(0.6/VOICES);
      voice[i]->setGate(true, 0);
    }
  }

  void noteOn(uint8_t note, uint16_t velocity, uint16_t samples){
    uint16_t minval = allocation[0];
    uint8_t minidx = 0;
    // take oldest free voice, to allow voices to ring out
    for(int i=1; i<VOICES; ++i){
      if(notes[i] == note){
	minidx = i;
	break;
      }
      if(allocation[i] < minval){
	minidx = i;
	minval = allocation[i];
      }
    }
    // take oldest voice
    take(minidx, note, velocity, samples);
  }

  void noteOff(uint8_t note, uint16_t samples){
    for(int i=0; i<VOICES; ++i)
      if(notes[i] == note)
	release(i, samples);
  }

  void setParameters(float attack, float release, float pb){
    for(int i=0; i<VOICES; ++i){
      float freq = 440.0f*exp2f((notes[i]-69 + pb*2)/12.0);
      voice[i]->setFrequency(freq);
      voice[i]->setEnvelope(attack, release);
    }
  }

  void generate(FloatArray samples){
    voice[0]->generate(samples);
    for(int i=1; i<VOICES; ++i){
      voice[i]->generate(buffer);
      samples.add(buffer);
    }
  }
};

class SamplerPatch : public Patch {
private:
  Voices<MIDI_VOICES> voices;
public:
  SamplerPatch() {

    Resource* resource = getResource(SAMPLENAME);
    WavFile wav(resource->getData(), resource->getSize());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
    FloatArray sample = wav.createFloatArray(0);

    float scale = 1.0f/max(sample.getMaxValue(), -sample.getMinValue());
    sample.multiply(scale);

    voices.init(getSampleRate(), getBlockSize(), sample);

    registerParameter(PARAMETER_A, "Pitchbend");
    registerParameter(PARAMETER_D, "Envelope");

    // debugMessage("normalised", scale);
  }
  ~SamplerPatch(){
  }
  void processMidi(MidiMessage msg){
    if(msg.isNoteOn())
      voices.noteOn(msg.getNote(), msg.getVelocity(), 0);
    else if(msg.isNoteOff())
      voices.noteOff(msg.getNote(), 0);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON){
      // debugMessage("push", bid, value, samples);
      if(value)
	voices.allNotesOn();
      else
	voices.allNotesOff();
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float env = getParameterValue(PARAMETER_D);
    float pitchbend = getParameterValue(PARAMETER_G); // MIDI Pitchbend
    pitchbend += getParameterValue(PARAMETER_A);
    float attack = 0.0f;
    float release = 0.0f;
    if(env < 0.5)
      attack = 2*(0.5 - env);
    else
      release = 2*(env - 0.5);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices.setParameters(attack, release, pitchbend);
    voices.generate(left);
    right.copyFrom(left);
  }
};

#endif   // __SamplerPatch_hpp__
