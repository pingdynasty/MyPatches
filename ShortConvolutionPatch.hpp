#ifndef __ShortConvolutionPatch_hpp__
#define __ShortConvolutionPatch_hpp__

#include "ShortPatch.h"
#include "ShortArray.h"
#include "ComplexShortArray.h"
#include "FastFourierTransform.h"
#include "impulse-response.h"

class ShortConvolutionPatch : public ShortPatch {
private:
  static const int segments = 10;
  int current;
  int segsize;
  int blocksize;
  ComplexShortArray irbuf[segments];
  ComplexShortArray cinbuf[segments];
  ComplexIntArray coutbuf;
  ComplexIntArray cmulbuf;
  ShortArray input;
  ShortArray overlap;
  FastFourierTransform fft;
public:
  ConvolutionPatch() : current(0), blocksize(getBlockSize()), segsize(2*getBlockSize()) {
    // ASSERT(sizeof(ir)/sizeof(float) >= segments*blocksize, "Impulse response array too small");
    input = ShortArray::create(segsize);
    coutbuf = ComplexIntArray::create(segsize);
    cmulbuf = ComplexIntArray::create(segsize);
    overlap = ShortArray::create(blocksize);
    overlap.clear();
    fft.init(segsize);

    // normalise IR level
    ShortArray impulse(ir[0], segments*blocksize);
    float scale = 1.0f/max(impulse.getMaxValue(), -impulse.getMinValue());
    impulse.multiply(scale);
    debugMessage("normalised", scale);

    // do forward fft into irbuf
    for(int i=0; i<segments; ++i){
      // allocate fft buffers
      irbuf[i] = ComplexShortArray::create(segsize);
      cinbuf[i] = ComplexShortArray::create(segsize);
      cinbuf[i].clear();
      // load IR and zero-pad to segment size
      input.clear();
      input.copyFrom(impulse.subArray(i*blocksize, blocksize));
      fft.fft(input, irbuf[i]);
      // fft.fft(impulse.subArray(i*blocksize, blocksize), irbuf[i]);
    }
    registerParameter(PARAMETER_D, "Wet/Dry");

    // repurpose impulse response memory
    // cmulbuf = ComplexIntArray((ComplexFloat*)(float*)impulse, segsize);
    // coutbuf = ComplexIntArray((ComplexFloat*)(float*)impulse.subArray(blocksize*4, blocksize*4), segsize);
    // // input = ShortArray((float*)impulse.subArray(blocksize*8, blocksize*2), segsize);
    // overlap = ShortArray((float*)impulse.subArray(blocksize*8, blocksize), blocksize);
  }

  void processAudio(ShortBuffer &buffer) {
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0f - wet;
    wet *= 1.0f/segments; // gain adjust

    ShortArray left = buffer.getSamples(LEFT_CHANNEL);
    input.clear();
    input.copyFrom(left, blocksize);

    // forward fft
    fft.fft(input, cinbuf[current]);

    // complex multiply and accumulate
    irbuf[0].complexByComplexMultiplication(cinbuf[current], coutbuf);
    for(int i=1; i<segments; ++i){
      int seg = (current+i) % segments;
      irbuf[i].complexByComplexMultiplication(cinbuf[seg], cmulbuf);
      coutbuf.add(cmulbuf);
    }
    // inverse fft
    fft.ifft(coutbuf, input);

    for(int i=0; i<blocksize; ++i)
    //   left[i] = wet*(input[i]+overlap[i]) + dry*left[i];
      left[i] = input[i]+overlap[i]; // fixed point addition

    // save overlap
    overlap.copyFrom(input.subArray(blocksize, blocksize));
    current = (current > 0) ? (current - 1) : (segments - 1);
  }
};

#endif   // __ConvolutionPatch_hpp__
