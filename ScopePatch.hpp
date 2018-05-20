/*
todo:
circular buffer for 1/2 pixel buffer pre-trigger history
pixel buffer 16x screen size (x and y)
display buffer 96x96

on trigger, copy circular buffer to pixel buffer and start filling
when full, copy to display buffer with current pos and divs x and y
on changing pos or divs copy from pixel buffer to display buffer. 
*/

#include "AudioDisplay.hpp"

class ScopePatch : public Patch {
private:
  Colour bg = BLACK;
  Colour fg = WHITE;
  Colour trace1 = WHITE; // RED
  Colour trace2 = WHITE; // GREEN
  int divisions;
  float triggerLevel;
  float gain;
  float offset;
  AudioDisplay display;
public:
  ScopePatch(){
    registerParameter(PARAMETER_A, "X Scale");
    registerParameter(PARAMETER_B, "Trigger");
    registerParameter(PARAMETER_C, "Y Scale");
    registerParameter(PARAMETER_D, "Y Offset");
    reset();
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0.5);
  }
  void reset(){
    divisions = 2;
    triggerLevel = 0.012345;
    gain = 1.2345678;
    offset = 0.0;
    display.reset();
  }

  void processScreen(ScreenBuffer& screen){
    // int y = screen.getHeight()-9;
    int y = 8; // screen.getHeight()-17;
    screen.setTextColour(fg);
    screen.setTextSize(1);
    // screen.fill(bg);
    // screen.setCursor(0, 8);
    // screen.print("Scope");
    screen.setCursor(0, y);
    screen.print(divisions);
    screen.setCursor(32, y);
    screen.print(triggerLevel);
    screen.setCursor(64, y);
    screen.print(gain);
    screen.setCursor(96, y);
    screen.print(offset);
    display.draw(screen, trace1);
   }

  void processAudio(AudioBuffer& samples){
    divisions = getParameterValue(PARAMETER_A)*64+1;
    triggerLevel = getParameterValue(PARAMETER_B)*2-1;
    gain = getParameterValue(PARAMETER_C)*4;
    offset = getParameterValue(PARAMETER_D)*2-1;
    FloatArray left = samples.getSamples(LEFT_CHANNEL);
    float* right = samples.getSamples(1);
    display.update(left, divisions, triggerLevel, gain, offset);
  }
};
