
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
      buffer[writepos++] = max(0, min(height*2, height + height*offset + height*samples[i]*gain));
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
