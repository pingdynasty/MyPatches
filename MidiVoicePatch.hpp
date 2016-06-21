#ifndef __MidiVoicePatch_hpp__
#define __MidiVoicePatch_hpp__

#include "StompBox.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"
#include "SmoothValue.h"

/*
 * A: Pitch
 * B: Fc exp
 * C: Resonance
 * D: Envelope / amplitude
 * E: Waveshape
 * Left: Pitch
 * Right: Fc lin
 */

class SynthVoice {
private:
  PolyBlepOscillator osc;
  BiquadFilter* filter;
  AdsrEnvelope env;
  SmoothFloat fc, q;
  float gain;
public:
  SynthVoice(float sr, BiquadFilter* f) : 
    osc(sr), filter(f), env(sr), fc(0.25), q(0.77), gain(1.0f) {    
    env.setSustain(1.0);
    env.setDecay(0.0);
    env.setRelease(0.0);
  }
  static SynthVoice* create(float sr){
    // 8-pole filter 48dB
    return new SynthVoice(sr, BiquadFilter::create(4));
  }
  static void destroy(SynthVoice* voice){
    BiquadFilter::destroy(voice->filter);    
  }
  void setFrequency(float freq){
    osc.setFrequency(freq);
  }
  void setResonance(float res){
    q = res;
  }
  void setCutoff(float freq){
    fc = freq;
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
  void setEnvelope(float attack, float release){
    env.setAttack(attack);
    env.setRelease(release);    
  }
  void setGain(float value){
    gain = value;
  }
  void setGate(bool state, int delay){
    env.gate(state, delay);
  }
  void getSamples(FloatArray samples){
    filter->setLowPass(fc, q);
    osc.getSamples(samples);
    filter->process(samples);
    samples.multiply(gain*(0.8-q*0.2)); // gain compensation for high q
    samples.multiply(0.8-q*0.2); // gain compensation for high q
    env.attenuate(samples);
  }
};

class MidiVoicePatch : public Patch {
private:
  VoltsPerOctave hz;
  float attack, release;
  SynthVoice* voice[2];
  int8_t notes[2];
public:
  MidiVoicePatch() {
    registerParameter(PARAMETER_A, "Waveshape");
    registerParameter(PARAMETER_B, "Fc");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    notes[0] = -1;
    notes[1] = -1;
    voice[0] = SynthVoice::create(getSampleRate());
    voice[1] = SynthVoice::create(getSampleRate());
  }
  ~MidiVoicePatch(){
    SynthVoice::destroy(voice[0]);
    SynthVoice::destroy(voice[1]);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      debugMessage("midi note", note, value, samples);
      if(value){
	// note on
	// float freq = (440.0f / 32) * (2 ^ ((note - 9) / 12));
	float freq = (440.0f / 32) * fastpow2f((note - 9) / 12.0f);
	if(notes[0] == -1){
	  notes[0] = note;
	  voice[0]->setFrequency(freq);
	  voice[0]->setGain(value/4095.0f);
	  voice[0]->setGate(true, samples);
	}else if(notes[1] == -1){
	  notes[1] = note;
	  voice[1]->setFrequency(freq);
	  voice[1]->setGain(value/4095.0f);
	  voice[1]->setGate(true, samples);
	}	  
      }else{
	// note off
	if(notes[0] == note){
	  notes[0] = -1;
	  voice[0]->setGate(false, samples);
	}else if(notes[1] == note){
	  notes[1] = -1;
	  voice[1]->setGate(false, samples);
	}
      }
    }else if(bid == PUSHBUTTON){
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float shape = getParameterValue(PARAMETER_A)*2;
    float cutoff = getParameterValue(PARAMETER_B)*0.5;
    float q = getParameterValue(PARAMETER_C)*3+0.75;
    float df = getParameterValue(PARAMETER_D)*4;
    int di = (int)df;
    float gain = 0.0f;
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
      gain = df-3;
      break;
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    voice[0]->setEnvelope(attack, release);
    voice[1]->setEnvelope(attack, release);
    voice[0]->setWaveshape(shape);
    voice[1]->setWaveshape(shape);
    voice[0]->setCutoff(cutoff);
    voice[1]->setCutoff(cutoff);
    voice[0]->setResonance(q);
    voice[1]->setResonance(q);

    voice[0]->getSamples(left);
    voice[1]->getSamples(right);
  }
};

#endif   // __MidiVoicePatch_hpp__
