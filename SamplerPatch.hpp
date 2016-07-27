#ifndef __SamplerPatch_hpp__
#define __SamplerPatch_hpp__

#include "Patch.h"
#include "Envelope.h"
#include "Oscillator.h"
#include "VoltsPerOctave.h"
#define SAMPLENAME sample1[0]
#include "sample.h"
#define MIDDLE_C 261.6

#include "Control.h"
#include "SmoothValue.h"

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
  SampleOscillator(float sr, FloatArray sample) :
    fs(sr), buffer(sample){
    size = buffer.getSize()-1;
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
  uint16_t findZeroCrossing(uint16_t index){
    uint16_t limit = buffer.getSize()-1;
    uint16_t i = min(index, limit);
    if(buffer[i] > 0)
      while(i < limit && buffer[i] > 0)
	i++;
    else
      while(i < limit && buffer[i] < 0)
	i++;
    return i;
  }
  void setDuration(uint16_t samples){
    size = findZeroCrossing(samples);
  }
  /* @return the audio length in samples */
  uint16_t getSampleLength(){
    return buffer.getSize();
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
  void getSamples(FloatArray samples){
    osc.setDuration(duration*osc.getSampleLength());
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
  SamplerPatch() : 
    voices(getSampleRate(), getBlockSize(), 
	   FloatArray(SAMPLENAME, // SAMPLESIZE)){ // 
		      sizeof(SAMPLENAME)/sizeof(float))) {
    registerParameter(PARAMETER_A, "Waveshape");
    registerParameter(PARAMETER_B, "Fc");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    FloatArray sample(SAMPLENAME, sizeof(SAMPLENAME)/sizeof(float));
    float scale = 1.0f/max(sample.getMaxValue(), -sample.getMinValue());
    sample.multiply(scale);
    // debugMessage("normalised", scale);
  }
  ~SamplerPatch(){
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      // debugMessage("midi note", note, value, samples);
      if(value) // note on
	voices.noteOn(note, value, samples);
      else // note off
	voices.noteOff(note, samples);
    }else if(bid == PUSHBUTTON){
      // debugMessage("push", bid, value, samples);
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
    float env = getParameterValue(PARAMETER_D);
    cutoff += getParameterValue(PARAMETER_F)*0.25; // MIDI CC1/Modulation
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
    voices.getSamples(left);
    right.copyFrom(left);
  }
};

#endif   // __SamplerPatch_hpp__
