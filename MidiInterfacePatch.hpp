#ifndef __MidiInterfacePatch_hpp__
#define __MidiInterfacePatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"

class MidiInterfacePatch : public Patch {
private:
  VoltsPerOctave hz;
  uint8_t note = 60;
  int velocity = 0;
  float freqin, ampin;
  uint8_t lastnote;
public:
  MidiInterfacePatch() {
  }
  ~MidiInterfacePatch(){
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      velocity = value;
      if(value){ // note on
	note = bid - MIDI_NOTE_BUTTON;
	setButton(PUSHBUTTON, ON, samples+getBlockSize());
      }else{ // note off
	setButton(PUSHBUTTON, OFF, samples+getBlockSize());
      }
    }else if(bid == PUSHBUTTON){
      if(value){
	uint8_t note = round(12*log2f(freqin/440.0f))+69;
	uint16_t velocity = min(4095, (uint16_t)(ampin*4095/5));
	setButton((PatchButtonId)(MIDI_NOTE_BUTTON+note), velocity, samples+getBlockSize());
	lastnote = note;
      }else{
	setButton((PatchButtonId)(MIDI_NOTE_BUTTON+lastnote), 0, samples+getBlockSize());
      }
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float pitchbend = getParameterValue(PARAMETER_G); // MIDI Pitchbend
    float freq = 440.0f*fastpow2f((note-69 + pitchbend*2)/12.0);
    freq = hz.voltsToSample(hz.hertzToVolts(freq));
    float amp = hz.voltsToSample(5*velocity/4096.0f);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    freqin = hz.getFrequency(left.getMean());
    ampin = abs(hz.sampleToVolts(right.getMean()));
    left.setAll(freq);
    right.setAll(amp);
  }
};

#endif   // __MidiInterfacePatch_hpp__
