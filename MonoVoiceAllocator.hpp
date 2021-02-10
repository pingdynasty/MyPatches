#ifndef __MonoVoiceAllocator_hpp__
#define __MonoVoiceAllocator_hpp__

class MonoVoiceAllocator {
  int8_t key;
  int8_t velocity;
  int16_t bend;
  uint8_t notes[16];
  uint8_t lastNote = 0;
  static constexpr int BEND_RANGE = 2; // +/- 2 octaves
public:
  // MonoVoiceAllocator()
  //   : key(0), freq(0), gain(0), gate(0), bend(0) {}
  // MonoVoiceAllocator(float ky, float fq, float gn, float gt, float bd)
  //   : key(ky), freq(fq), gain(gn), gate(gt), bend(bd) {}
  int8_t getKey(){
    return key;
  }
  int8_t getVelocity(){
    return velocity;
  }
  float getFreq() {
    return noteToHz(key);
  }
  float getGain() {
    return velocityToGain(velocity);
  }
  bool getGate() {
    return lastNote != 0;
  }
  /**
   * get bend as a value between -1 and 1, no bend is 0
   */
  float getBend() {
    return bend/8192.0f;
  }
  /**
   * get bend as a multiplier between 0.25 and 2, no bend is 1
   */
  float getBendMultiplier() {
    return exp2f(getBend()*BEND_RANGE);
  }
  float getNote() {
    return key+getBend()*BEND_RANGE*12;
  }
  void processMidi(MidiMessage msg) {
    if(msg.isNoteOn()) {
      noteOn(msg.getNote(), (uint16_t)msg.getVelocity());
    }
    else if(msg.isNoteOff()) {
      noteOff(msg.getNote(), (uint16_t)msg.getVelocity());
    }
    else if(msg.isPitchBend()) {
      setPitchBend(msg.getPitchBend());
    }
    else if(msg.isControlChange()) {
      if(msg.getControllerNumber() == MIDI_ALL_NOTES_OFF)
	allNotesOff();
    }
  }
  void setPitchBend(int16_t pb) {
    bend = pb;
  }
  float noteToHz(uint8_t note) {
    return 440.0f * exp2f((note - 69) / 12.0);
  }
  float velocityToGain(uint16_t velocity) {
    float value = velocity / 127.0f;
    return value*value;
  }
  void noteOn(uint8_t note, uint16_t velocity) {
    if (lastNote < 16)
      notes[lastNote++] = note;
    key = note;
  }
  void noteOff(uint8_t note, uint16_t velocity) {
    int i;
    for (i = 0; i < lastNote; ++i) {
      if (notes[i] == note)
	break;
    }
    if (lastNote > 1) {
      lastNote--;
      while (i < lastNote) {
	notes[i] = notes[i + 1];
	i++;
      }
      key = notes[lastNote - 1];
    }
    else {
      lastNote = 0;	    
    }
  }
  void allNotesOff() {
    lastNote = 0;
    bend = 0;
  }
};

#endif /* __MonoVoiceAllocator_hpp__ */
