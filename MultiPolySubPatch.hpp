#ifndef __MultiPolySubPatch_hpp__
#define __MultiPolySubPatch_hpp__

#include "Patch.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"
#include "SmoothValue.h"
#include "SynthVoice.hpp"

#define MIDI_VOICES 4
#define PITCHBEND_RANGE 80 // 8

int8_t basenote = -69;

template<int VOICES>
class Voices {
private:
  static const uint8_t EMPTY = 0xff;
  static const uint16_t TAKEN = 0xffff;
  SynthVoice* voice[VOICES];
  uint8_t notes[VOICES];
  uint8_t channels[VOICES];
  uint16_t allocation[VOICES];
  uint16_t allocated;
  FloatArray buffer;
  float pressure[VOICES];
  float bend[VOICES];
  float modulation[VOICES];
  // SmoothFloat pressure[VOICES];
  // SmoothFloat bend[VOICES];
  // SmoothFloat modulation[VOICES];

private:
  void take(uint8_t idx, uint8_t note, uint16_t velocity, uint16_t samples){
    notes[idx] = note;
    allocation[idx] = TAKEN;
    float gain = velocity*velocity/16129.0f + 0.1;
    gain *= 1.0f/VOICES;
    voice[idx]->setGain(gain);
    voice[idx]->setGate(true, samples);
    // pressure[idx].reset(0);
    // modulation[idx].reset(0);
    // bend[idx].reset(0);
    pressure[idx] = 0;
    modulation[idx] = 0;
    bend[idx] = 0;
    // debugMessage("gain", gain);
  }

  void release(uint8_t idx, uint16_t samples){
    // notes[ch] = EMPTY;
    allocation[idx] = ++allocated;
    voice[idx]->setGate(false, samples);
    channels[idx] = EMPTY;
  }

public:
  Voices(float sr, int bs) : allocated(0) {
    for(int i=0; i<VOICES; ++i){
      voice[i] = SynthVoice::create(sr);
      notes[i] = 69; // middle A, 440Hz
      allocation[i] = 0;
      channels[i] = EMPTY;
    }
    buffer = FloatArray::create(bs);
  }

  ~Voices(){
    for(int i=0; i<VOICES; ++i)
      SynthVoice::destroy(voice[i]);
    FloatArray::destroy(buffer);
  }

  void setPressureForChannel(uint8_t ch, float value){
    for(int i=0; i<VOICES; ++i){
      if(channels[i] == ch)
	pressure[i] = value;
    }
  }

  void setBendForChannel(uint8_t ch, float value){
    for(int i=0; i<VOICES; ++i){
      if(channels[i] == ch)
	bend[i] = value;
    }
  }

  void setModulationForChannel(uint8_t ch, float value){
    for(int i=0; i<VOICES; ++i){
      if(channels[i] == ch)
	modulation[i] = value;
    }
  }

  void allNotesOff(){
    for(int i=0; i<VOICES; ++i){
      release(i, 0);
      allocation[i] = 0;
    }
    allocated = 0;
  }

  void allNotesOn(){
    for(int i=0; i<VOICES; ++i){
      voice[i]->setGain(0.5/VOICES);
      voice[i]->setGate(true, 0);
    }
  }

  void noteOn(uint8_t ch, uint8_t note, uint16_t velocity, uint16_t samples){
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
    channels[minidx] = ch;
    // debugMessage("idx/note/vel", minidx, note, velocity);
  }

  void noteOff(uint8_t note, uint16_t samples){
    for(int i=0; i<VOICES; ++i)
      if(notes[i] == note)
	release(i, samples);
  }

  void setParameters(float shape, float fc, float q, float attack, float release){
    for(int i=0; i<VOICES; ++i){
      float freq = 440.0f*exp2f((notes[i]+basenote + bend[i]*PITCHBEND_RANGE)/12.0);
      voice[i]->setFrequency(freq);
      shape = max(0, min(1.999, shape+pressure[i]));
      voice[i]->setWaveshape(shape);
      fc = max(0.001, min(0.499, fc+modulation[i]));
      voice[i]->setFilter(fc, q);
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

class MultiPolySubPatch : public Patch {
private:
  Voices<MIDI_VOICES> voices;
  BiquadFilter* hp;
public:
  MultiPolySubPatch() : voices(getSampleRate(), getBlockSize()) {
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
    hp = BiquadFilter::create(2);
    hp->setHighPass(40/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
  }
  ~MultiPolySubPatch(){
    BiquadFilter::destroy(hp);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      // uint8_t note = bid - MIDI_NOTE_BUTTON;
      // if(value) // note on
      // 	voices.noteOn(note, value>>5, samples);
      // else // note off
      // 	voices.noteOff(note, samples);
    }else if(bid == PUSHBUTTON){
      if(value)
	voices.allNotesOn();
      else
	voices.allNotesOff();
    }else if(bid == BUTTON_B){
      if(value)
      	voices.noteOn(0, 48, 80, samples);
      else
      	voices.noteOff(48, samples);
    }else if(bid == BUTTON_C){
      if(value)
      	voices.noteOn(0, 53, 80, samples);
      else
      	voices.noteOff(53, samples);
    }else if(bid == BUTTON_D){
      if(value)
      	voices.noteOn(0, 60, 80, samples);
      else
      	voices.noteOff(60, samples);
    }    
    // debugMessage("button/value/samples", bid, value, samples);
  }
  void processMidi(MidiMessage& msg){
    // debugMessage("port/status/d1", msg.data[0], msg.data[1], msg.data[2]);
    // debugMessage("port/channel/status", msg.getPort(), msg.getChannel(), msg.getStatus());
    // debugMessage("status/note/vel", msg.getStatus(), msg.getNote(), msg.getVelocity());
    // if(msg.getPort() != 1) // prevent double triggering on TouchKeys / Impulse
    //   return;
    uint16_t samples = 0;
    if(msg.isNoteOn()){
      voices.noteOn(msg.getChannel(), msg.getNote(), (uint16_t)msg.getVelocity(), samples);
      // debugMessage("note on", msg.getPort(), msg.getNote(), msg.getVelocity());
    }else if(msg.isNoteOff()){
      // debugMessage("note off", msg.getPort(), msg.getNote(), msg.getVelocity());
      voices.noteOff(msg.getNote(), samples);
    }else if(msg.isPitchBend()){
      // Pitch Bend (for X-axis movement)
      // debugMessage("pb", msg.getPort(), msg.getChannel(), msg.getPitchBend());
      voices.setBendForChannel(msg.getChannel(), msg.getPitchBend()/8192.0f);
      // pb = msg.getPitchBend()/8192.0f;
    }else if(msg.isChannelPressure()){
      // Channel Pressure (for finger pressure)
      voices.setPressureForChannel(msg.getChannel(), msg.getChannelPressure()/127.0f*0.5);
    }else if(msg.isControlChange()){
      if(msg.getControllerNumber() == 0x4a){
	// CC74 (for Y-axis movement)
	voices.setModulationForChannel(msg.getChannel(), msg.getControllerValue()/127.0f*0.25);
	// mod = msg.getControllerValue()/127.0f;
      }
    }
  }
  void setBasenote(float note){
    int i = (int)(note+0.5);
    if(abs(note-i) < 0.8)
      basenote = note;
  }
    
  void processAudio(AudioBuffer &buffer) {
    setBasenote(getParameterValue(PARAMETER_A)*24 - 81);
    float cutoff = getParameterValue(PARAMETER_B)*0.48 + 0.01;
    float q = getParameterValue(PARAMETER_C)*3+0.75;
    float df = getParameterValue(PARAMETER_D)*4;
    // cf += mod*0.25; // getParameterValue(PARAMETER_F)*0.25; // MIDI CC1/Modulation
    // cf = max(0.001, min(0.499, cf));
    // cutoff = cf;
    float shape = getParameterValue(PARAMETER_E)*2;
    // float pitchbend = getParameterValue(PARAMETER_G); // MIDI Pitchbend
    // pitchbend = pb;
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
    voices.setParameters(shape, cutoff, q, attack, release);
    voices.getSamples(left);
    left.multiply(0.6);
    hp->process(left);
    right.copyFrom(left);
  }
};

#endif   // __MultiPolySubPatch_hpp__
