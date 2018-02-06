/*
todo:
circular buffer for 1/2 pixel buffer pre-trigger history
pixel buffer 16x screen size (x and y)
display buffer 96x96

on trigger, copy circular buffer to pixel buffer and start filling
when full, copy to display buffer with current pos and divs x and y
on changing pos or divs copy from pixel buffer to display buffer. 

split processBlock() from processFrame()
*/

class ScopePatch : public Patch {
private:
  Colour bg = BLACK;
  Colour fg = WHITE;
  Colour trace1 = WHITE; // RED
  Colour trace2 = WHITE; // GREEN
  Colour buffer[2][128];
  int writepos = 0;
  int div = 1;
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
public:
  ScopePatch(){
    registerParameter(PARAMETER_A, "Divisions");
    reset();
  }
  void reset(){
    // width = height = 0;
    width = 128;
    height = 24;
    div = 2;
    writepos = 0;
    for(int i=0; i<128; ++i){
      buffer[0][i] = 31;
      buffer[1][i] = 33;
    }
  }

  // void encoderChanged(uint8_t encoder, int32_t dir){
  //   if(dir > 0 && div < 16)
  //     div++;
  //   else if(div > 1)
  //     div--;
  //   writepos = 0;
  //   reset();
  // }

  int height;
  int width;

  void processScreen(ScreenBuffer& screen){
    height = screen.getHeight()/2;
    width = screen.getWidth();
    screen.setTextColour(fg);
    screen.setTextSize(1);
    screen.fill(bg);
    screen.setCursor(35, 0);
    screen.print("Scope");
    screen.setCursor(0, screen.getHeight()-8);
    screen.print(div);
    int ly = buffer[0][0];
    int ry = buffer[1][0];
    for(int i=1; i<width; ++i){
      // ly = drawVerticalLine(screen, i, ly, buffer[0][i], trace1);
      ry = drawVerticalLine(screen, i, ry, buffer[1][i], trace2);
    }
   }

  void processAudio(AudioBuffer& samples){
    div = getParameterValue(PARAMETER_A)*16+1;
    int size = samples.getSize();
    float* left = samples.getSamples(0);
    float* right = samples.getSamples(1);
    float trig = 0.0f;
    int offset = 0;
    // fast forward to trigger
    // look for rising edge
    if(writepos == 0){
      while(left[offset] > trig-0.0001 && offset < size)
	offset++;
      while(left[offset] < trig && offset < size)
	offset++;
    }
    // if(offset+width*div < size)
      // writepos = 0;
    for(int i=offset; i<samples.getSize() && writepos < width; i+=div){
      buffer[0][writepos] = height+height*left[i];
      buffer[1][writepos] = height+height*right[i]+2;
      writepos++;
    }
    if(writepos >= width)
      writepos = 0;
  }
};
