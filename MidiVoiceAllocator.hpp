
class MidiVoice {
public:
  virtual void noteOn(uint8_t note, uint8_t velocity, uint16_t delay){}
  virtual void noteOff(uint8_t note, uint16_t delay){}
  virtual void pitchBend(int16_t bend, uint16_t delay){}
  virtual void controlChange(uint8_t cc, uint8_t value, uint16_t delay){}
};

#define MIDI_CHANNELS 4
// allocator that assumes one MIDI channel per voice, MPE style
class MidiVoiceAllocator {
public:
  MidiVoice* voice[MIDI_CHANNELS];
  // uint8_t channelNote[MIDI_CHANNELS];
  // float channelBend[MIDI_CHANNELS];
  // ChannelVoiceAllocator(float& fq, float& gn, float& gt): freq(fq), gain(gn), gate(gt) {}
  // ChannelVoiceAllocator(){
  //   for(int i=0; i<MIDI_CHANNELS; ++i){
  //   }
  // }
  void processMidi(MidiMessage& msg){
    uint16_t samples = 0;
    uint8_t ch = msg.getChannel();
    if(msg.isNoteOn()){
      if(ch < MIDI_CHANNELS)
	voice[ch]->noteOn(msg.getNote(), (uint16_t)msg.getVelocity()<<5, samples);
    }else if(msg.isNoteOff()){
      if(ch < MIDI_CHANNELS)
	voice[ch]->noteOff(msg.getNote(), samples);
    }else if(msg.isPitchBend()){
      if(ch < MIDI_CHANNELS)
	voice[ch]->pitchBend(msg.getPitchBend(), samples);
    }else if(msg.isControlChange()){
      switch(msg.getControllerNumber()){
      case MIDI_ALL_NOTES_OFF:
	allNotesOff(samples);
	break;
      default:
	if(ch < MIDI_CHANNELS)
	  voice[ch]->controlChange(msg.getControllerNumber(), msg.getControllerValue(), samples);
      }
    }
  }
  void allNotesOff(uint16_t samples){
    for(int i=0; i<MIDI_CHANNELS; ++i)
      voice[i]->noteOff(-1, samples);
  }
};
