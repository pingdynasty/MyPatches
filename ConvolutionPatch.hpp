#ifndef __ConvolutionPatch_hpp__
#define __ConvolutionPatch_hpp__

#include "StompBox.h"
#include "FastFourierTransform.h"
#include "impulse-response.h"
// ./wav2float MyPatches/ir/EMT\ 244\ \(A.\ Bernhard\)/0\,4s\ Low\*2\ \ High_2.wav > MyPatches/impulse-response.h

class ConvolutionPatch : public Patch {
private:
  static const int segments = 6;
  int segsize;
  int blocksize;
  ComplexFloatArray irbuf[segments];
  ComplexFloatArray cbuf[segments];
  ComplexFloatArray rbuf;
  FloatArray input;
  FloatArray overlap;
  FastFourierTransform fft;
  int current;
public:
  ConvolutionPatch() : current(0) {
    // ASSERT(sizeof(ir)/sizeof(float) >= segments*blocksize, "Impulse response array too small");
    blocksize = getBlockSize();
    segsize = 2*blocksize;
    input = FloatArray::create(segsize);
    rbuf = ComplexFloatArray::create(segsize);
    overlap = FloatArray::create(blocksize);
    overlap.clear();
    fft.init(segsize);

    // normalise IR level
    FloatArray impulse(ir, segments*blocksize);
    float scale = 0.5f/max(impulse.getMaxValue(), -impulse.getMinValue());
    impulse.multiply(scale);
    debugMessage("normalised", scale);

    // do forward fft into irbuf
    for(int i=0; i<segments; ++i){
      // allocate fft buffers
      irbuf[i] = ComplexFloatArray::create(segsize);
      cbuf[i] = ComplexFloatArray::create(segsize);
      cbuf[i].setAll(0);
      // load IR and zero-pad to segment size
      input.clear();
      input.copyFrom(impulse.subArray(i*blocksize, blocksize));
      fft.fft(input, irbuf[i]);
    }
    registerParameter(PARAMETER_D, "Wet/Dry");
  }

  void cmac(ComplexFloatArray dest, ComplexFloatArray a, ComplexFloatArray b){
    int size = dest.getSize();
    const size_t blockend = 4 * (size / 4);
    for(size_t i=0; i<blockend; i+=4){
      dest[i+0].re += a[i+0].re * b[i+0].re - a[i+0].im * b[i+0].im;
      dest[i+1].re += a[i+1].re * b[i+1].re - a[i+1].im * b[i+1].im;
      dest[i+2].re += a[i+2].re * b[i+2].re - a[i+2].im * b[i+2].im;
      dest[i+3].re += a[i+3].re * b[i+3].re - a[i+3].im * b[i+3].im;
      dest[i+0].im += a[i+0].re * b[i+0].im + a[i+0].im * b[i+0].re;
      dest[i+1].im += a[i+1].re * b[i+1].im + a[i+1].im * b[i+1].re;
      dest[i+2].im += a[i+2].re * b[i+2].im + a[i+2].im * b[i+2].re;
      dest[i+3].im += a[i+3].re * b[i+3].im + a[i+3].im * b[i+3].re;
    }
    for(size_t i=blockend; i<size; ++i){
      dest[i].re += a[i].re * b[i].re - a[i].im * b[i].im;
      dest[i].im += a[i].re * b[i].im + a[i].im * b[i].re;
    }
  }

  void processAudio(AudioBuffer &buffer) {
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0f - wet;
    wet *= 1.0f/segments; // gain adjust

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    input.clear();
    input.copyFrom(left, blocksize);

    // forward fft
    fft.fft(input, cbuf[current]);

    // complex multiply and accumulate
    rbuf.setAll(0);
    for(int i=1; i<segments; ++i){
      int seg = (current+i) % segments;
      cmac(rbuf, irbuf[i], cbuf[seg]);
    }
    cmac(rbuf, cbuf[current], irbuf[0]);
    // inverse fft
    fft.ifft(rbuf, input);

    for(int i=0; i<blocksize; ++i)
      left[i] = wet*(input[i]+overlap[i]) + dry*left[i];

    // save overlap
    overlap.copyFrom(input.subArray(blocksize, blocksize));
    current = (current > 0) ? (current - 1) : (segments - 1);
  }
};

#endif   // __ConvolutionPatch_hpp__
