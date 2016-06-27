#ifndef __SamplerPatch_hpp__
#define __SamplerPatch_hpp__

#include "StompBox.h"
#include "Envelope.h"
#include "Oscillator.h"
#include "VoltsPerOctave.h"
#define SAMPLENAME sample1[0]
#define SAMPLESIZE (13*1024)
#include "sample.h"
#define MIDDLE_C 261.6

/*
  need a buffer that can be read in a circular fashion
 */
class SampleOscillator : public Oscillator {
private:
  const float fs;
  FloatArray buffer;
  float rate = 1.0f;
  float pos = 0.0f;
  uint16_t size;
public:
  SampleOscillator(float sr, FloatArray sample) :
    fs(sr), buffer(sample){
    size = buffer.getSize()-1;
  }
  void setFrequency(float freq){
    rate = freq/MIDDLE_C;
  }
  void reset(){
    pos = 0.0f;
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
  float getNextSample(){
    increment();
    return interpolate(pos, buffer);
  }
  void setLength(uint16_t samples){
    size = min(samples, buffer.getSize()-1);
  }
  // void getSamples(FloatArray out){
  //   // unwind interpolation
  // }
};

class SamplerVoice {
private:
  SampleOscillator osc;
  AdsrEnvelope env;
  float gain;
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
    osc.setFrequency(freq);
  }
  void setEnvelope(float attack, float release){
    env.setAttack(attack);
    env.setRelease(release);
  }
  void setGain(float value){
    gain = value;
  }
  void setGate(bool state, int delay){
    env.gate(state, delay);
    if(state)
      osc.reset();
  }
  void getSamples(FloatArray samples){
    osc.getSamples(samples);
    samples.multiply(gain);
    env.attenuate(samples);
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
  Voices(float sr, int bs, FloatArray buf) : allocated(0) {
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
      if(allocation[i] < minval)
	minidx = i;
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
      float freq = 440.0f*fastpow2f((notes[i]-69 + pb*2)/12.0);
      voice[i]->setFrequency(freq);
      voice[i]->setEnvelope(attack, release);
    }
  }

  void getSamples(FloatArray samples){
    voice[0]->getSamples(samples);
    for(int i=1; i<VOICES; ++i){
      voice[i]->getSamples(buffer);
      samples.add(buffer);
    }
  }
};

class SamplerPatch : public Patch {
private:
  Voices<8> voices;
public:
  SamplerPatch() : voices(getSampleRate(), getBlockSize(), FloatArray(SAMPLENAME, SAMPLESIZE)) {
    registerParameter(PARAMETER_A, "Waveshape");
    registerParameter(PARAMETER_B, "Fc");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    FloatArray sample(SAMPLENAME, SAMPLESIZE);
    float scale = 1.0f/max(sample.getMaxValue(), -sample.getMinValue());
    sample.multiply(scale);
    debugMessage("normalised", scale);
  }
  ~SamplerPatch(){
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      debugMessage("midi note", note, value, samples);
      if(value) // note on
	voices.noteOn(note, value, samples);
      else // note off
	voices.noteOff(note, samples);
    }else if(bid == PUSHBUTTON){
      debugMessage("push", bid, value, samples);
      if(value)
	voices.allNotesOn();
      else
	voices.allNotesOff();
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float shape = getParameterValue(PARAMETER_A)*2;
    float cutoff = getParameterValue(PARAMETER_B)*0.48 + 0.01;
    float q = getParameterValue(PARAMETER_C)*3+0.75;
    float df = getParameterValue(PARAMETER_D)*4;
    cutoff += getParameterValue(PARAMETER_F)*0.25; // MIDI CC1/Modulation
    float pitchbend = getParameterValue(PARAMETER_G); // MIDI Pitchbend
    pitchbend += getParameterValue(PARAMETER_A);
    // debugMessage("af/ag/ah", getParameterValue(PARAMETER_AF), getParameterValue(PARAMETER_AG), getParameterValue(PARAMETER_AH));
    // debugMessage("f/g/h", getParameterValue(PARAMETER_F), getParameterValue(PARAMETER_G), getParameterValue(PARAMETER_H));
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
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices.setParameters(attack, release, pitchbend);
    voices.getSamples(left);
    right.copyFrom(left);
  }
};

#endif   // __SamplerPatch_hpp__
