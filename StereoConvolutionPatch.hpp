#ifndef __StereoConvolutionPatch_hpp__
#define __StereoConvolutionPatch_hpp__

#include "StompBox.h"
#include "FastFourierTransform.h"
#include "stereo-impulse-response.h"

class StereoConvolutionPatch : public Patch {
private:
  static const int segments = 3;
  const int blocksize;
  const int segsize;
  int current;
  FloatArray input;
  ComplexFloatArray cmulbuf;
  ComplexFloatArray cinbuf[segments];
  ComplexFloatArray irbufl[segments];
  ComplexFloatArray coutbufl;
  FloatArray overlapl;
  ComplexFloatArray irbufr[segments];
  ComplexFloatArray coutbufr;
  FloatArray overlapr;
  FastFourierTransform fft;
public:
  StereoConvolutionPatch() : blocksize(getBlockSize()), segsize(2*getBlockSize()), current(0) {
    ASSERT(sizeof(ir)/sizeof(float)/2 >= segments*blocksize, "Impulse response array too small");
    input = FloatArray::create(segsize);
    cmulbuf = ComplexFloatArray::create(segsize);
    coutbufl = ComplexFloatArray::create(segsize);
    overlapl = FloatArray::create(blocksize);
    overlapl.clear();
    coutbufr = ComplexFloatArray::create(segsize);
    overlapr = FloatArray::create(blocksize);
    overlapr.clear();
    fft.init(segsize);

    // normalise IR level
    FloatArray impulsel(ir[0], segments*blocksize);
    FloatArray impulser(ir[1], segments*blocksize);
    float scale = 1.0f/max(max(max(impulsel.getMaxValue(), -impulsel.getMinValue()), impulser.getMaxValue()), -impulser.getMinValue());
    impulsel.multiply(scale);
    impulser.multiply(scale);
    debugMessage("normalised", scale);

    // do forward fft into irbuf
    for(int i=0; i<segments; ++i){
      // allocate fft buffers
      cinbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i].clear();
      irbufl[i] = ComplexFloatArray::create(segsize);
      irbufr[i] = ComplexFloatArray::create(segsize);
      // load IR and zero-pad to segment size
      input.clear();
      input.copyFrom(impulsel.subArray(i*blocksize, blocksize));
      fft.fft(input, irbufl[i]);
      input.clear();
      input.copyFrom(impulser.subArray(i*blocksize, blocksize));
      fft.fft(input, irbufr[i]);
    }
    registerParameter(PARAMETER_D, "Wet/Dry");
  }

  void processAudio(AudioBuffer &buffer) {
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0f - wet;
    wet *= 1.0f/segments; // gain adjust

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    input.clear();
    input.copyFrom(left, blocksize);

    // forward fft
    fft.fft(input, cinbuf[current]);

    // complex multiply and accumulate
    irbufl[0].complexByComplexMultiplication(cinbuf[current], coutbufl);
    irbufr[0].complexByComplexMultiplication(cinbuf[current], coutbufr);
    for(int i=1; i<segments; ++i){
      int seg = (current+i) % segments;
      irbufl[i].complexByComplexMultiplication(cinbuf[seg], cmulbuf);
      coutbufl.add(cmulbuf);
      irbufr[i].complexByComplexMultiplication(cinbuf[seg], cmulbuf);
      coutbufr.add(cmulbuf);
    }

    // inverse fft
    fft.ifft(coutbufl, input);
    for(int i=0; i<blocksize; ++i)
      left[i] = wet*(input[i]+overlapl[i]) + dry*left[i];
    // save overlap
    overlapl.copyFrom(input.subArray(blocksize, blocksize));

    // inverse fft
    fft.ifft(coutbufr, input);
    for(int i=0; i<blocksize; ++i)
      right[i] = wet*(input[i]+overlapr[i]) + dry*right[i];
    // save overlap
    overlapr.copyFrom(input.subArray(blocksize, blocksize));

    current = (current > 0) ? (current - 1) : (segments - 1);
  }
};

#endif   // __ConvolutionPatch_hpp__
