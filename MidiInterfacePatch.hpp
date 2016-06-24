#ifndef __MidiInterfacePatch_hpp__
#define __MidiInterfacePatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"

class MidiInterfacePatch : public Patch {
private:
  VoltsPerOctave hz;
  float freqin, ampin;
  float freqout, ampout;
  float pitchbend;
public:
  MidiInterfacePatch() {
  }
  ~MidiInterfacePatch(){
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      debugMessage("midi note in", note, value, samples);
      if(value){ // note on
	float freq = 440.0f*fastpow2f((note-69 + pitchbend*2)/12.0);
	freqout = hz.voltsToSample(hz.hertzToVolts(freq));
	ampout = value/4096.0f;
	setButton(PUSHBUTTON, ON, samples+getBlockSize());
      }else{ // note off
	setButton(PUSHBUTTON, OFF, samples+getBlockSize());
      }
    }else if(bid == PUSHBUTTON){
      int note = round(12*log2f(freqin/440.0f))+69;
      debugMessage("midi note out", (float)note, (float)freqin, (float)value);
      setButton((PatchButtonId)(MIDI_NOTE_BUTTON+note), ampin*4096.0f, samples+getBlockSize());
    }
  }
  void processAudio(AudioBuffer &buffer) {
    pitchbend = getParameterValue(PARAMETER_G); // MIDI Pitchbend
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    freqin = hz.getFrequency(left.getMean());
    ampin = right.getMean();
    left.setAll(freqout);
    right.setAll(ampout);
  }
};

#endif   // __MidiInterfacePatch_hpp__
