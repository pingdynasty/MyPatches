#ifndef __PolyDrumPatch_hpp__
#define __PolyDrumPatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"
#include "Cymbal.hpp"
#include "BassDrum.hpp"

class PolyDrumPatch : public Patch {
private:
  Drum* voices[4];
  FloatArray sum;
  float pitchbend;
public:
  PolyDrumPatch(){
    sum = FloatArray::create(getBlockSize());
    registerParameter(PARAMETER_A, "Kick Decay");
    registerParameter(PARAMETER_B, "Snare Decay");
    registerParameter(PARAMETER_C, "Hi Decay");
    registerParameter(PARAMETER_D, "Hat Decay");
    voices[0] = new BassDrumVoice(getSampleRate(), 80, 0.1);
    voices[1] = new BassDrumVoice(getSampleRate(), 120, 0.3);
    voices[2] = new CymbalVoice(getSampleRate(), 1760, 0.75, 0.7);
    voices[3] = new CymbalVoice(getSampleRate(), 3520, 0.8, 0.8);
  }
  ~PolyDrumPatch(){
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      if(value){
	if(note < 40){
	  float freq = 220.0f*fastpow2f((note-69 + pitchbend*2)/12.0);
	  voices[0]->setFrequency(freq);
	  voices[0]->setAccent(value/4096.0f);
	  voices[0]->trigger();
	}else if(note < 60){
	  float freq = 220.0f*fastpow2f((note-69 + pitchbend*2)/12.0);
	  voices[1]->setFrequency(freq);
	  voices[1]->setAccent(value/4096.0f);
	  voices[1]->trigger();
	}else if(note < 80){
	  float freq = 440.0f*fastpow2f((note-69 + pitchbend*2)/12.0);
	  voices[2]->setFrequency(freq);
	  voices[2]->setAccent(value/4096.0f);
	  voices[2]->trigger();
	}else{
	  float freq = 440.0f*fastpow2f((note-69 + pitchbend*2)/12.0);
	  voices[3]->setFrequency(freq);
	  voices[3]->setAccent(value/4096.0f);
	  voices[3]->trigger();
	}
	debugMessage("note on", note, value);
      }
    }else if(bid == PUSHBUTTON){
      for(int i=0; i<4; ++i)
	voices[i]->trigger(value, samples);
    }
  }
  void processAudio(AudioBuffer& buffer){
    voices[0]->setDecay(getParameterValue(PARAMETER_A));
    voices[1]->setDecay(getParameterValue(PARAMETER_B));
    voices[2]->setDecay(getParameterValue(PARAMETER_C));
    voices[3]->setDecay(getParameterValue(PARAMETER_D));
    float pitchbend = getParameterValue(PARAMETER_G); // MIDI Pitchbend
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices[0]->getSamples(left);
    voices[1]->getSamples(sum);
    left.add(sum);
    left.multiply(0.75);
    voices[2]->getSamples(right);
    voices[3]->getSamples(sum);
    right.add(sum);
    right.multiply(0.75);
  }
};

#endif   // __PolyDrumPatch_hpp__
