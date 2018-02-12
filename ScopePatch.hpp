/*
todo:
circular buffer for 1/2 pixel buffer pre-trigger history
pixel buffer 16x screen size (x and y)
display buffer 96x96

on trigger, copy circular buffer to pixel buffer and start filling
when full, copy to display buffer with current pos and divs x and y
on changing pos or divs copy from pixel buffer to display buffer. 
*/

#define MAX_WIDTH 128
class AudioDisplay {
  int16_t buffer[MAX_WIDTH]; // reduced sample buffer
  int writepos;
  int height;
  int width;
public:
  AudioDisplay(){
    reset();
  }
  void reset(){
    writepos = 0;
    width = MAX_WIDTH;
    height = 32;
    for(int i=0; i<MAX_WIDTH; ++i){
      buffer[i] = height;
    }
  }
  void update(FloatArray samples, uint16_t divisions, float triggerLevel, float gain, float offset){
    int skip = 0;
    int size = samples.getSize();
    if(writepos == 0){
      // fast forward to trigger
      while(samples[skip] > triggerLevel-0.0001 && skip < size)
	skip++;
      while(samples[skip] < triggerLevel && skip < size)
	skip++;
    }
    for(int i=skip; i<size && writepos < width; i+=divisions)
      buffer[writepos++] = height + height*offset + height*samples[i]*gain;
    if(writepos >= width)
      writepos = 0;
  }
  void draw(ScreenBuffer& screen, Colour c){
    height = screen.getHeight()>>2;
    width = screen.getWidth();
    int y = buffer[0];
    for(int i=1; i<width; ++i)
      y = drawVerticalLine(screen, i, y, buffer[i], c);
  }
private:
  uint16_t drawVerticalLine(ScreenBuffer& screen, uint16_t x, uint16_t y, uint16_t to, uint16_t c){
    if(y > to)
      drawVerticalLine(screen, x, to, y, c);
    else
      do{
	screen.setPixel(x, y++, c);
      }while(y < to);
    return to;
  }
};

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
