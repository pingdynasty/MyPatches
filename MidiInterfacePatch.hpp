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
  void processMidi(MidiMessage msg){
    if(msg.isNoteOn()){
      velocity = (uint16_t)msg.getVelocity() << 5;
      note = msg.getNote();
      setButton(PUSHBUTTON, ON, getBlockSize());
    }else if(msg.isNoteOff()){
      velocity = 0;
      setButton(PUSHBUTTON, OFF, getBlockSize());
    }
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON){
      if(value){
	uint8_t note = round(12*log2f(freqin/440.0f))+69;
	uint8_t velocity = min(127, (int)(ampin*127/5));
	sendMidi(MidiMessage::note(1, note, velocity));
	lastnote = note;
      }else{
	sendMidi(MidiMessage::note(1, lastnote, 0));
      }
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float pitchbend = getParameterValue(PARAMETER_G); // MIDI Pitchbend
    float freq = 440.0f*exp2f((note-69 + pitchbend*2)/12.0);
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
