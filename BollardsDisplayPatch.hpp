#include "Patch.h"

#define MAX_PRESET     11
#define MANUAL_PRESET (MAX_PRESET+1)

#define PRESET_MODE 1
#define PRESET_MODE 1

#define GREY 0xAAAA

#define MINS_PER_PRESET 60
#define MINS_IDLE_TIME  5

const char presetnames[][8] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Bypass", "Manual"};

#define PARAMETER_TICKS 63

class BollardsDisplayPatch : public Patch {
private:
  const int SEND_RATE = 1357;
  const int OWL_A_CH = 1;
  const int OWL_B_CH = 2;
  const int OWL_C_CH = 3;
  const int OWL_D_CH = 4;
  int preset0, preset1;
  int octave0, octave1;
  int parameterA, parameterB;
  int debounceDelay = 0;
  int size = 4; // 5;
  uint16_t x = 2;
  uint16_t y = 42;
  int16_t hspace = 0;
  int16_t vspace = 40;//34;
  uint16_t bg = GREEN; // BLUE; // BLACK;
  uint16_t fg = GREY; // WHITE;
  SmoothFloat tune1, tune2;
  uint32_t counter, idle;
  const uint32_t presetChangeInterval;
  const uint32_t maxIdleInterval;
public:
  BollardsDisplayPatch() :
    presetChangeInterval(MINS_PER_PRESET*60*getSampleRate()/getBlockSize()),
    maxIdleInterval(MINS_IDLE_TIME*60*getSampleRate()/getBlockSize()) {
    registerParameter(PARAMETER_A, "Preset");
    registerParameter(PARAMETER_B, "Octave");
    reset();
    // changePreset();
  }
  void setOctave(int ch, int oct){
#ifdef USE_MIDI_CALLBACK
    MidiMessage msg;
    if(ch == 0){    
      msg = MidiMessage::cc(OWL_A_CH, PATCH_PARAMETER_F, (oct-1)*127/5);
      sendMidi(msg);
      msg = MidiMessage::cc(OWL_B_CH, PATCH_PARAMETER_F, (oct-1)*127/5);
      sendMidi(msg);
    }else{
      msg = MidiMessage::cc(OWL_C_CH, PATCH_PARAMETER_F, (oct-1)*127/5);
      sendMidi(msg);
      msg = MidiMessage::cc(OWL_D_CH, PATCH_PARAMETER_F, (oct-1)*127/5);
      sendMidi(msg);
    }
#endif
  }
  void setPreset(int ch, int preset){
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
    counter = idle = 0;
    setParameterValue(PARAMETER_A, 1.0/PARAMETER_TICKS);
    setParameterValue(PARAMETER_B, 2.0/PARAMETER_TICKS);
    tune1 = SmoothFloat(0, 0.98);
    tune2 = SmoothFloat(0, 0.98);
  }
  void drawPreset(ScreenBuffer& screen, uint8_t x, uint8_t y, uint8_t preset){
    screen.setCursor(x, y);
    if(preset < MAX_PRESET){
      screen.setTextSize(1);
      screen.print("Preset ");
      screen.setTextSize(4);
    }else{
      screen.setTextSize(2);
    }      
    screen.print(presetnames[preset-1]);
  }
  void drawOctave(ScreenBuffer& screen, uint8_t x, uint8_t y, int8_t octave){
    screen.setTextSize(1);
    screen.print(x, y, "Octave ");
    screen.setTextSize(4);
    screen.print(octave);
  }
  void processScreen(ScreenBuffer& screen){
    screen.fill(bg);
    if(idle >= maxIdleInterval){
      screen.clear();
    }else{
      drawPreset(screen, x, y, preset0);
      drawOctave(screen, x+hspace, y+vspace, octave0);
    }
  }
  void changePreset(){
    int next = preset0 + 1;
    if(next >= MAX_PRESET)
      next = 0;
    setParameterValue(PARAMETER_A, float(next)/PARAMETER_TICKS);
  }
  void processAudio(AudioBuffer& samples){
    idle++;
    // debugMessage("a/b", getParameterValue(PARAMETER_A)*PARAMETER_TICKS, getParameterValue(PARAMETER_B)*PARAMETER_TICKS);
    int value = round(getParameterValue(PARAMETER_A)*PARAMETER_TICKS);
    if(parameterA != value){
      parameterA = value;
      if(parameterA > MANUAL_PRESET){
      	parameterA = MANUAL_PRESET;
      	setParameterValue(PARAMETER_A, float(parameterA)/PARAMETER_TICKS);
      }else if(parameterA < 1){
      	parameterA = 1;
      	setParameterValue(PARAMETER_A, float(parameterA)/PARAMETER_TICKS);	
      }			      
      if(preset0 != parameterA){
      	preset0 = parameterA;
      	preset1 = parameterA;
      	setPreset(0, preset0);
      	setPreset(1, preset1);
	idle = 0;
      }
    }
    value = round(getParameterValue(PARAMETER_B)*PARAMETER_TICKS);
    if(parameterB != value){
      parameterB = value;
      if(parameterB > 5){
      	parameterB = 5;
      	setParameterValue(PARAMETER_B, float(parameterB)/PARAMETER_TICKS);
      }else if(parameterB < 1){
      	parameterB = 1;
      	setParameterValue(PARAMETER_B, float(parameterB)/PARAMETER_TICKS);	
      }
      if(octave0 != parameterB){
	octave0 = parameterB;
	octave1 = parameterB;
	setOctave(0, octave0);
	setOctave(1, octave1);
	idle = 0;
      }
    }
    
    if(++counter >= presetChangeInterval){
      changePreset();
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

    // tune1 = getParameterValue(PARAMETER_E);
    // tune2 = getParameterValue(PARAMETER_F);
    // samples.getSamples(LEFT_CHANNEL).setAll(tune1*2-1);
    // samples.getSamples(RIGHT_CHANNEL).setAll(tune2*2-1);
    samples.getSamples(LEFT_CHANNEL).clear();
    samples.getSamples(RIGHT_CHANNEL).clear();
  }
};
