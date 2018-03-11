#include "Patch.h"

// #include "midi.h"
// #include "gpio.h"
// #include "OpenWareMidiControl.h"
// extern "C" uint32_t HAL_GetTick();

#define MAX_PRESET     11
#define MANUAL_PRESET (MAX_PRESET+1)

#define PRESET_MODE 1
#define PRESET_MODE 1

#define GREY 0xAAAA

class PresetDisplayPatch : public Patch {
private:
  const int SEND_RATE = 357;
  const int OWL_A_CH = 0;
  const int OWL_B_CH = 1;
  const int OWL_C_CH = 2;
  const int OWL_D_CH = 3;
  int counter;
  int preset0, preset1;
  int debounceDelay = 0;
  int size = 4; // 5;
  uint16_t x = 2;
  uint16_t y = 36;
  int16_t hspace = 46;
  int16_t vspace = 38;
  uint16_t bg = BLACK;
  uint16_t fg = WHITE;

  // char presetnames[][8] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "Pass", "Man"};
  
public:
  PresetDisplayPatch(){
  }
  void setPreset(int ch, int preset){
#if 0
    if(ch == 0){
      midiSendCC(OWL_A_CH, PATCH_PARAMETER_E, preset);
      midiSendCC(OWL_B_CH, PATCH_PARAMETER_E, preset);
    }else{
      midiSendCC(OWL_C_CH, PATCH_PARAMETER_E, preset);
      midiSendCC(OWL_D_CH, PATCH_PARAMETER_E, preset);
    }
#else
    setParameterValue(PARAMETER_E, preset*4096/MANUAL_PRESET);
#endif
  }
  void reset(){
    counter = 0;
    preset0 = preset1 = 1;
    setPreset(0, preset0);
    setPreset(1, preset0);
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
  void encoderChanged(uint8_t encoder, int32_t dir){
    // if(abs(dir) < 2)
    //   return;
    static int last = 0;
    int now = HAL_GetTick();
    if(now - last < debounceDelay)
      return;
    last = now;
    if(encoder == 0){
      if(dir > 2)
	preset0 = min(MAX_PRESET, preset0+1);
      else if(dir < 2)
	preset0 = max(1, preset0-1);
      midiSendCC(OWL_A_CH, PATCH_PARAMETER_E, preset0*127/MANUAL_PRESET);
      midiSendCC(OWL_B_CH, PATCH_PARAMETER_E, preset0*127/MANUAL_PRESET);
      setParameterValue(PARAMETER_A, preset0*4096/MANUAL_PRESET);
      counter = 0;
    }else if(encoder == 1){
      if(dir > 1)
	preset1 = min(MAX_PRESET, preset1+1);
      else if(dir < 1)
	preset1 = max(1, preset1-1);
      midiSendCC(OWL_C_CH, PATCH_PARAMETER_E, preset1*127/MANUAL_PRESET);
      midiSendCC(OWL_D_CH, PATCH_PARAMETER_E, preset1*127/MANUAL_PRESET);
      setParameterValue(PARAMETER_B, preset1*4096/MANUAL_PRESET);
      counter = 0;
    }
#endif
  }
  void draw(ScreenBuffer& screen, uint8_t x, uint8_t y, uint8_t preset){
    switch(preset){
    case MAX_PRESET:
      screen.setTextSize(2);
      screen.print(x, y, "Pass");
      break;
    case MANUAL_PRESET:
      screen.setTextSize(2);
      screen.print(x, y, "Man");
      break;
    default:
      screen.setTextSize(size);
      screen.setCursor(x, y);
      screen.print(preset);
      // char low = (preset % 10) +'0';
      // char high = (preset/10)+'0';
      // screen.drawChar(x, y, high, fg, bg, size);
      // screen.drawChar(x+size*6+2, y, low, fg, bg, size);
      break;
    }
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
      // midiSendCC(OWL_A_CH, PATCH_PARAMETER_E, preset0*127/MANUAL_PRESET);
      // midiSendCC(OWL_B_CH, PATCH_PARAMETER_E, preset0*127/MANUAL_PRESET);
      // setParameterValue(PARAMETER_A, preset0*4096/MANUAL_PRESET);
      counter = 0;
    }
    preset = max(1, min(MANUAL_PRESET, round(getParameterValue(PARAMETER_B)*MANUAL_PRESET)));
    if(preset1 != preset){
      preset1 = preset;
      setPreset(1, preset1);
      // midiSendCC(OWL_C_CH, PATCH_PARAMETER_E, preset1*127/MANUAL_PRESET);
      // midiSendCC(OWL_D_CH, PATCH_PARAMETER_E, preset1*127/MANUAL_PRESET);
      // setParameterValue(PARAMETER_B, preset1*4096/MANUAL_PRESET);
      counter = 0;
    }

    if(++counter == SEND_RATE){
      if(preset0 < MANUAL_PRESET)
	setPreset(0, preset0);
	// midiSendCC(OWL_A_CH, PATCH_PARAMETER_E, preset0*127/MANUAL_PRESET);
    }else if(counter == SEND_RATE*2){
      if(preset0 < MANUAL_PRESET)
	setPreset(1, preset1);
	// midiSendCC(OWL_B_CH, PATCH_PARAMETER_E, preset0*127/MANUAL_PRESET);
    // }else if(counter == SEND_RATE*3){
    //   if(preset1 < MANUAL_PRESET)
    // 	midiSendCC(OWL_C_CH, PATCH_PARAMETER_E, preset1*127/MANUAL_PRESET);
    // }else if(counter >= SEND_RATE*4){
    //   if(preset1 < MANUAL_PRESET)
    // 	midiSendCC(OWL_D_CH, PATCH_PARAMETER_E, preset1*127/MANUAL_PRESET);
      counter = 0;
    }
  }
};
