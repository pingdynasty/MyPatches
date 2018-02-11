/*
class SynthVoice {
protected:
  float freq;
  float gain;
  bool gate;
public:
  void setFreq(float value){
    freq = value;
  }
  void setGain(float value){
    gain = value;
  }
  virtual void setGate(bool state, int delay){
    gate = state;
  }
  virtual void getSamples(FloatArray samples) = 0;
};
*/

class TestPatch : public Patch {
  MonoVoiceAllocator allocator;
  void processMidi(MidiMessage& msg){
    uint16_t samples = 0;
    if(msg.isNoteOn()){
      allocator.noteOn(msg.getNote(), (uint16_t)msg.getVelocity()<<5, samples);
      debugMessage("note on", msg.getPort(), msg.getNote(), msg.getVelocity());
    }else if(msg.isNoteOff()){
      debugMessage("note off", msg.getPort(), msg.getNote(), msg.getVelocity());
      allocator.noteOff(msg.getNote(), samples);
    }else 
      if(msg.isPitchBend()){
	debugMessage("pb", msg.getPort(), msg.getChannel(), msg.getPitchBend());
	allocator.setPitchBend(msg.getPitchBend());
    }
  }
};

class ChannelVoiceAllocator {
  uint8_t channelNote[16];
  float channelBend[16];
  // ChannelVoiceAllocator(float& fq, float& gn, float& gt): freq(fq), gain(gn), gate(gt) {}
};

class MonoVoiceAllocator {
  float& freq, gain, gate;
  uint8_t notes[16];
  uint8_t lastNote = 0;
  float pb;
public:
  MonoVoiceAllocator(float& fq, float& gn, float& gt): freq(fq), gain(gn), gate(gt) {}
  
  void processMidi(MidiMessage& msg){
    uint16_t samples = 0;
    uint16_t samples = 0;
    if(msg.isNoteOn()){
      noteOn(msg.getNote(), (uint16_t)msg.getVelocity()<<5, samples);
    }else if(msg.isNoteOff()){
      debugMessage("note off", msg.getPort(), msg.getNote(), msg.getVelocity());
    }else if(msg.isPitchBend()){
      setPitchBend(msg.getPitchBend());
    }else if(msg.isControlChange()){
      if(msg.getControllerNumber() == MIDI_ALL_NOTES_OFF)
	allNotesOff();
    }
  }
  void setPitchBend(int16_t bend){
    // pb = bend/8192.0f;
    pb = bend/1024.0f; // 2 octave range?
  }
  void setPitchBend(float bend){
    pb = bend*2;
  }
  void noteOn(uint8_t note, uint16_t velocity, uint16_t delay){
    if(lastNote == 0)
      gate = 1;
    if(lastNote < 16)
      notes[lastNote++] = note;
    // compute frequency and gain
    freq = 440.0f*exp2f((note-69 + pb)/12.0);
    gain = velocity/4095.0;
  }
  void noteOff(uint8_t note, uint16_t delay){
    for(int i=0; i<lastNote; ++i){
      if(notes[i] == note){
	while(i<lastNote)
	  notes[i] = notes[++i];
      }
    }
    --lastNote;
    // compute frequency and gain
    float freq = 440.0f*exp2f((notes[lastNote-1]-69 + pb)/12.0);
    voice->setFreq(freq);
    if(lastNote == 0)
      gate = 0;
  }
  void allNotesOff(){
    lastNote = 0;
    pb = 0;
  }
};

#include "BitState.hpp"
// this won't revert to correct pitch when several keys are pressed and most recent one is released
class MonoVoiceAllocator {
  SynthVoice* voice;
  BitState128 notes;
public:
  // virtual void noteOn(float freq, float gain){
  // virtual void noteOff(float freq, float gain){
  virtual void noteOn(uint8_t note, uint16_t velocity, uint16_t delay){
    if(notes.isClear())
      voice->setGate(true, delay);
    // compute frequency and gain
    float freq = 440.0f*exp2f((note-69 + pb*2)/12.0);
    float gain = velocity/4095.0;
    voice->setFreq(freq);
    voice->setGain(gain);
    notes.setEngaged(note);
  }
  virtual void noteOff(uint8_t note, uint16_t delay){
    notes.clearEngaged(note);
    if(notes.isClear())
      voice->setGate(false, delay);    
  }  
};
