#include "Patch.h"

#define MAX_PRESET     11
#define MANUAL_PRESET (MAX_PRESET+1)

#define PRESET_MODE 1
#define PRESET_MODE 1

#define GREY 0xAAAA

// extern "C" {
//   void doMidiSend(uint8_t port, uint8_t d0, uint8_t d1, uint8_t d2);
// }

// void midiSendCC(uint8_t ch, uint8_t cc, uint8_t value){
//   doMidiSend(USB_COMMAND_CONTROL_CHANGE, CONTROL_CHANGE|ch, cc, value);
// }

const char presetnames[][8] = {" 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "Bypass", "Manual"};

class PresetDisplayPatch : public Patch {
private:
  const int SEND_RATE = 1357;
  const int OWL_A_CH = 1;
  const int OWL_B_CH = 2;
  const int OWL_C_CH = 3;
  const int OWL_D_CH = 4;
  int counter;
  int preset0, preset1;
  int octave0, octave1;
  int debounceDelay = 0;
  int size = 4; // 5;
  uint16_t x = 2;
  uint16_t y = 36;
  int16_t hspace = 20;
  int16_t vspace = 34;
  uint16_t bg = BLACK;
  uint16_t fg = GREY; // WHITE;
  SmoothFloat tune1, tune2;
public:
  PresetDisplayPatch(){
    reset();
    registerParameter(PARAMETER_A, "Preset 1");
    registerParameter(PARAMETER_B, "Preset 2");
    registerParameter(PARAMETER_C, "Octave 1");
    registerParameter(PARAMETER_D, "Octave 2");
    registerParameter(PARAMETER_E, "Tune 1");
    registerParameter(PARAMETER_F, "Tune 2");
    tune1.lambda = tune2.lambda = 0.98;
    setParameterValue(PARAMETER_A, 0.0);
    setParameterValue(PARAMETER_B, 0.0);
    setParameterValue(PARAMETER_C, 1.0/5);
    setParameterValue(PARAMETER_D, 1.0/5);
    setParameterValue(PARAMETER_E, 0.5);
    setParameterValue(PARAMETER_F, 0.5);
  }
  void setOctave(int ch, int oct){
#ifdef USE_MIDI_CALLBACK
    MidiMessage msg;
    if(ch == 0){    
      msg = MidiMessage::cc(OWL_A_CH, PATCH_PARAMETER_F, oct*127/5);
      sendMidi(msg);
      msg = MidiMessage::cc(OWL_B_CH, PATCH_PARAMETER_F, oct*127/5);
      sendMidi(msg);
    }else{
      msg = MidiMessage::cc(OWL_C_CH, PATCH_PARAMETER_F, oct*127/5);
      sendMidi(msg);
      msg = MidiMessage::cc(OWL_D_CH, PATCH_PARAMETER_F, oct*127/5);
      sendMidi(msg);
    }
#endif
  }
  void setPreset(int ch, int preset){
    // setParameterValue(PARAMETER_E, preset*4096/MANUAL_PRESET);
#ifdef USE_MIDI_CALLBACK
    MidiMessage msg;
    if(ch == 0){
      msg = MidiMessage::cc(OWL_A_CH, PATCH_PARAMETER_E, preset*127/MANUAL_PRESET);
      sendMidi(msg);
      msg = MidiMessage::cc(OWL_B_CH, PATCH_PARAMETER_E, preset*127/MANUAL_PRESET);
      sendMidi(msg);
    }else{
      msg = MidiMessage::cc(OWL_C_CH, PATCH_PARAMETER_E, preset*127/MANUAL_PRESET);
      sendMidi(msg);
      msg = MidiMessage::cc(OWL_D_CH, PATCH_PARAMETER_E, preset*127/MANUAL_PRESET);
      sendMidi(msg);
    }
#endif
  }
  void reset(){
    counter = 0;
    preset0 = preset1 = 1;
    setPreset(0, preset0);
    setPreset(1, preset1);
    octave0 = octave1 = 1;
    setOctave(0, octave0);
    setOctave(1, octave1);
#if 0    
    setParameterValue(PARAMETER_A, preset0*4096/MANUAL_PRESET);
    setParameterValue(PARAMETER_B, preset1*4096/MANUAL_PRESET);

    // // send PC to change to last program, E8: PC40
    // midiSendPC(OWL_A_CH, 40);
    // send CC 26 value 127 to turn remote control on: CC26/127
    midiSendCC(OWL_A_CH, PATCH_CONTROL, 127);
    // send parameter E value 0 to select first preset: CC24/0
    midiSendCC(OWL_A_CH, PATCH_PARAMETER_E, preset0);

    midiSendCC(OWL_B_CH, PATCH_CONTROL, 127);
    midiSendCC(OWL_B_CH, PATCH_PARAMETER_E, preset0);

    midiSendCC(OWL_C_CH, PATCH_CONTROL, 127);
    midiSendCC(OWL_C_CH, PATCH_PARAMETER_E, preset1);
    midiSendCC(OWL_D_CH, PATCH_CONTROL, 127);
    midiSendCC(OWL_D_CH, PATCH_PARAMETER_E, preset1);
  }
#endif
  }
  void draw(ScreenBuffer& screen, uint8_t x, uint8_t y, uint8_t preset){
    if(preset < MAX_PRESET)
      screen.setTextSize(4);
    else
      screen.setTextSize(2);
    screen.print(x, y, presetnames[preset-1]);
  }
  void processScreen(ScreenBuffer& screen){
    // screen.fill(bg);
    draw(screen, x, y, preset0);
    draw(screen, x+hspace, y+vspace, preset1);
  }

  void processAudio(AudioBuffer& samples){
#if 0
    static bool sw1_en = false;
    static bool sw2_en = false;
    if(sw1()){
      if(!sw1_en){
	midiSendCC(OWL_A_CH, PATCH_BUTTON, 127);
	sw1_en = true;
	bg = BLUE;
      }
    }else{
      if(sw1_en){
	midiSendCC(OWL_A_CH, PATCH_BUTTON, 127);
	sw1_en = false;
	bg = BLACK;
      }
    }
    if(sw2()){
      if(!sw2_en){
	midiSendCC(OWL_C_CH, PATCH_BUTTON, 127);
	sw2_en = true;
	bg = BLUE;
      }
    }else{
      if(sw2_en){
	midiSendCC(OWL_C_CH, PATCH_BUTTON, 127);
	sw2_en = false;
	bg = BLACK;
      }
    }
#endif
    int preset = max(1, min(MANUAL_PRESET, round(getParameterValue(PARAMETER_A)*MANUAL_PRESET)));
    if(preset0 != preset){
      preset0 = preset;
      setPreset(0, preset0);
      counter = 0;
    }
    preset = max(1, min(MANUAL_PRESET, round(getParameterValue(PARAMETER_B)*MANUAL_PRESET)));
    if(preset1 != preset){
      preset1 = preset;
      setPreset(1, preset1);
      counter = 0;
    }

    int octave = max(0, min(4, round(getParameterValue(PARAMETER_C)*5)));
    if(octave != octave0){
      octave0 = octave;
      setOctave(0, octave0);
      counter = 0;
    }
    octave = max(0, min(4, round(getParameterValue(PARAMETER_D)*5)));
    if(octave != octave1){
      octave1 = octave;
      setOctave(1, octave1);
      counter = 0;
    }
  
    // if(++counter == SEND_RATE){
    //   if(preset0 < MANUAL_PRESET)
    // 	setPreset(0, preset0);
    // }else if(counter == SEND_RATE*2){
    //   if(preset0 < MANUAL_PRESET)
    // 	setPreset(1, preset1);
    //   counter = 0;
    // }

    tune1 = getParameterValue(PARAMETER_E);
    tune2 = getParameterValue(PARAMETER_F);
    samples.getSamples(LEFT_CHANNEL).setAll(tune1*2-1);
    samples.getSamples(RIGHT_CHANNEL).setAll(tune2*2-1);
  }
};
