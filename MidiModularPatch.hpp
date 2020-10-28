#ifndef __MidiModularPatch_hpp__
#define __MidiModularPatch_hpp__

#include "Patch.h"
#include "SineOscillator.h"
#include "VoltsPerOctave.h"

class State {
public:
  int channel = 0;
  int note = 69; // A4
  int velocity = 0;
  float freq = 0;
  float pitchbend = 0;
  int modulation = 0;
  int expression = 0;
};

class MidiModularPatch : public Patch {
private:
  SineOscillator osc;
  FloatArray fm;
  VoltsPerOctave hzIn;
  VoltsPerOctave hzOut;
  State in;
  State out;
  float saveLeft = 0;
  float saveRight = 0;
public:
  MidiModularPatch() : hzIn(0.0014, 10.2), hzOut(-0.0022, -10.75) {
    osc.setSampleRate(getSampleRate());
    fm = FloatArray::create(getBlockSize());
  }

  void processMidi(MidiMessage msg){
    switch(msg.getStatus()) {
    case NOTE_OFF:
      msg.data[3] = 0;
      // deliberate fall-through
    case NOTE_ON:
      if(!in.velocity){
	in.note = msg.getNote();
	in.velocity = msg.getVelocity();
      }else if(msg.getNote() == in.note){
	in.velocity = 0;
      }
      break;
    case PITCH_BEND_CHANGE:
      in.pitchbend = msg.getPitchBend()/8192.0f;
      break;
    case CONTROL_CHANGE:
      switch(msg.getControllerNumber()){
      case MIDI_CC_MODULATION:
	in.modulation = msg.getControllerValue();
	break;
      case MIDI_CC_EXPRESSION:
	in.expression = msg.getControllerValue();
	break;
      }
    }
  }
  
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_A:
      out.velocity = value ? 80 : 0;
      sendMidi(MidiMessage::note(out.channel, out.note, out.velocity));
      break;
    case BUTTON_B:
      break;
    }
  }

  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    // CV to MIDI
    out.freq = hzOut.getFrequency(left.getMean());
    if(out.velocity == 0)
      out.note = hzOut.hertzToNote(out.freq);
    int pb = right.getMean()*8192;
    if(pb != out.pitchbend){
      sendMidi(MidiMessage::pb(out.channel, pb));
      out.pitchbend = pb;
    }
    int cc = getParameterValue(PARAMETER_A)*127;
    if(cc != out.modulation){
      sendMidi(MidiMessage::cc(out.channel, MIDI_CC_MODULATION, cc));
      out.modulation = cc;
    }
    cc = getParameterValue(PARAMETER_B)*127;
    if(cc != out.expression){
      sendMidi(MidiMessage::cc(out.channel, MIDI_CC_EXPRESSION, cc));
      out.expression = cc;
    }

    // MIDI to CV
    in.freq = hzIn.noteToHertz(in.note);
    float value = hzIn.getSample(in.freq);
    left.ramp(saveLeft, value);
    saveLeft = value;
    right.ramp(saveRight, in.pitchbend);
    saveRight = in.pitchbend;
    setParameterValue(PARAMETER_F, in.modulation/127.0f);
    setParameterValue(PARAMETER_G, in.expression/127.0f);
    if(in.velocity)
      setButton(PUSHBUTTON, 4095, 0);      
    else
      setButton(PUSHBUTTON, 0, 0);

    // add a little oscillation
    osc.setFrequency(in.freq*(1+round(getParameterValue(PARAMETER_C)*12)/12));
    osc.getSamples(fm);
    fm.multiply(getParameterValue(PARAMETER_D)*0.2);
    left.add(fm);
  }
};

#endif   // __MidiModularPatch_hpp__
