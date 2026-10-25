#include "MonochromeScreenPatch.h"
#include "BitmapFile.h"

// #include "out.h"
// #define IMG out_bmp
#include "polygons.h"
#define IMG polygons_bmp
// #include "splash.h"
// #define IMG splash_bmp
// #define IMG MagickImage

// #include "genii.h"
// #define IMG genii_bmp

// convert  Genius/drawings/MinoanGenii.png -monochrome BMP3:genii.bmp
// xxd -i genii.bmp > genii.h

class SplashPatch : public MonochromeScreenPatch {
private:
  float ypos = 0;
  BitmapFile* bmp;
  unsigned int height = 156;
public:
  SplashPatch(){
    debugMessage("sizeof", (int)sizeof(IMG));
    registerParameter(PARAMETER_A, "Rate");
    setParameterValue(PARAMETER_A, 0.5);
    registerParameter(PARAMETER_B, "Pos");
    setParameterValue(PARAMETER_B, 1);

    bmp = new BitmapFile(IMG, sizeof(IMG));
    if(bmp->isValid())
      debugMessage("bmp valid", (int)bmp->getWidth(), bmp->getHeight(), bmp->getBitsPerPixel());
    else
      debugMessage("bmp invalid", (int)bmp->getWidth(), bmp->getHeight(), bmp->getBitsPerPixel());

    ypos = bmp->getHeight() - 1;
    height = bmp->getHeight() - 64;
  }
  void processAudio(AudioBuffer& samples){
    // float a = getParameterValue(PARAMETER_A);
    // ypos = clamp(a * bmp->getHeight(), 0.0f, bmp->getHeight()-1.0f);

    float rate = 200*(getParameterValue(PARAMETER_A)-0.5)/getBlockRate();
    // float rate = 100/getBlockRate();
    ypos += rate;
    ypos = getParameterValue(PARAMETER_B) * height;
    if(ypos >= height)
      ypos = height - 1;
    // if(ypos < 0)
    //   ypos = height;
    if(ypos < 0)
      ypos = 0;

    // debugMessage("ypos/rate", ypos, rate);
  }
  static Colour getPixel(unsigned int x, unsigned int y, const uint8_t* img, unsigned int width, unsigned int height){
    if(x < width && y < height){
    // uint8_t  ucByteOffset = 0;
    // uint16_t usiArrayLoc = 0;
    // // Determine array location
    // usiArrayLoc = (y/8)+(x*8);
    // // Determine byte offset
    // ucByteOffset = y-((uint8_t)(y/8)*8);		
    // // Set pixel in buffer
    // return img[usiArrayLoc] & (1 << ucByteOffset) ? WHITE : BLACK;
      unsigned int top = width*(height-1)/8;
      uint8_t pxl = img[top - y*width/8 + x/8];
    // uint8_t pxl = img[y*width/8 + x/8];
      return pxl & (1 << ((7-x) % 8)) ? WHITE : BLACK;
    }
    return BLACK;
  }
  void processScreen(MonochromeScreenBuffer& screen){
    // screen.clear();
    // bmp->draw(screen, x, y+ypos);

    MonochromeImage img = bmp->getImage(0, ypos, 128, 64);
    img.draw(screen, 0, 0);

    // uint8_t* img = (uint8_t*)bmp->getData();
    // size_t height = bmp->getHeight();
    // // uint8_t* img = (uint8_t*)(IMG+62);
    // // size_t height = 192;
    // for(int y=0; y<screen.getHeight(); ++y){
    //   for(int x=0; x<screen.getWidth(); ++x){
    // 	// screen.setPixel(x, y, getPixel(x, y + ypos, img, 128, height));
    // 	screen.setPixel(x, y, bmp->getPixel(x, y + ypos));
    //   }
    // }
  }
};
