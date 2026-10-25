#ifndef __BitmapFile_h__
#define __BitmapFile_h__

#include "MonochromeScreenPatch.h"

// http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
// https://en.wikipedia.org/wiki/BMP_file_format

template<size_t bits_per_pixel>
class Image {
protected:
  void* data;
  uint16_t width;
  uint16_t height;
public:
  Image(void* data, uint16_t width, uint16_t height): data(data), width(width), height(height){}
  size_t getWidth(){
    return width;
  }
  size_t getHeight(){
    return height;
  }
  Colour getPixel(unsigned int x, unsigned int y);
  void draw(MonochromeScreenBuffer& screen, unsigned int x, unsigned int y){
    draw(screen, x, y, getWidth(), getHeight());
  }
  void draw(MonochromeScreenBuffer& screen, unsigned int x, unsigned int y, unsigned int width, unsigned int height){
    for(size_t yi=0; yi<height; ++yi)
      for(size_t xi=0; xi<width; ++xi)
	screen.setPixel(x+xi, y+yi, getPixel(xi, yi));
  }
};

typedef Image<1> MonochromeImage;

template<>
Colour MonochromeImage::getPixel(unsigned int x, unsigned int y){
  if(x < width && y < height){
    uint8_t* bitmap = (uint8_t*)data;
    unsigned int top = getWidth()*(getHeight()-1)/8;
    uint8_t pxl = bitmap[top - y*getWidth()/8 + x/8];
    return pxl & (1 << ((7-x) % 8)) ? WHITE : BLACK;
  }
  return BLACK;
}

typedef struct {
  char     id[2];
  uint32_t filesize;
  uint32_t reserved;
  uint32_t offset;
}  __attribute__((packed)) BitmapFileHeader;
// sizeof BitmapFileHeader = 14 bytes

typedef struct {
  uint32_t size; // size = 40
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bits_per_pixel;
  uint32_t compression;
  uint32_t image_size;
  uint32_t x_pixels_per_meter;
  uint32_t y_pixels_per_meter;
  uint32_t colors_used;
  uint32_t important_colors;
} BitmapInfoHeader;
// sizeof BitmapInfoHeader = 40 bytes
// BITMAPCOREHEADER = 12 bytes
// BITMAPCOREHEADER2 = 64 bytes
// BITMAPINFOHEADER = 40 bytes
// BITMAPV2INFOHEADER = 52 bytes
// BITMAPV3INFOHEADER = 56 bytes
// BITMAPV4INFOHEADER = 108 bytes
// BITMAPV5INFOHEADER = 124 bytes

class BitmapFile {
  BitmapFileHeader* fileheader;
  BitmapInfoHeader* infoheader;
  void* pixels;
  size_t size;
public:
  // The bitmap is monochrome, and the palette contains two entries. Each bit in the bitmap array represents a pixel. If the bit is clear, the pixel is displayed with the color of the first entry in the palette; if the bit is set, the pixel has the color of the second entry in the table.
  BitmapFile(void* data, size_t len) : fileheader((BitmapFileHeader*)data), size(len) {
    ASSERT(sizeof(BitmapFileHeader) == 14, "Invalid file header size");
    ASSERT(sizeof(BitmapInfoHeader) == 40, "Invalid info header size");
    infoheader = (BitmapInfoHeader*)((uint8_t*)data + sizeof(BitmapFileHeader));
    pixels = (uint8_t*)data + fileheader->offset;
  }
  BitmapFileHeader* getFileHeader(){
    return fileheader;
  }
  BitmapInfoHeader* getInfoHeader(){
    return infoheader;
  }
  bool isValid(){
    if(!fileheader || strncmp(fileheader->id, "BM", 2))
      return false;
    if(fileheader->filesize != size)
      return false;
    if(!infoheader || infoheader->size != 40)
      return false;
    if(infoheader->compression != 0) // compression is not supported
      return false;
    return true;
  }
  size_t getBitsPerPixel(){
    return infoheader->bits_per_pixel;
  }
  size_t getWidth(){
    return infoheader->width;
  }
  size_t getHeight(){
    return infoheader->height;
  }
  // get image size in pixels
  size_t getImageSize(){
    return getWidth()*getHeight();
  }
  // get file size in bytes
  size_t getSize(){
    return size;
  }
  void* getData(){
    // The bits representing the bitmap pixels are packed in rows. The size of each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding.
    // For images with height above 1, multiple padded rows are stored consecutively, forming a Pixel Array.
    return pixels;
  }
  MonochromeImage getImage(){
    return getImage(0, 0, getWidth(), getHeight());
  }
  MonochromeImage getImage(unsigned int x, unsigned int y, unsigned int width, unsigned int height){
    width = min(width, getWidth() - x);
    height = min(height, getHeight() - y);
    uint8_t* bitmap = (uint8_t*)pixels;
    return MonochromeImage(bitmap + (y*getWidth() + x)/8, width, height);
  }
};

#endif // __BitmapFile_h__
