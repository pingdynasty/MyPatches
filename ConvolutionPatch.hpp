#ifndef __ConvolutionPatch_hpp__
#define __ConvolutionPatch_hpp__

#include "StompBox.h"
#include "FastFourierTransform.h"

class ConvolutionPatch : public Patch {
private:
  int segsize;
  int blocksize;
  ComplexFloatArray irbuf;
  ComplexFloatArray cbuf;
  ComplexFloatArray rbuf;
  FloatArray input;
  FloatArray overlap;
  FastFourierTransform fft;
public:
  ConvolutionPatch(){
    blocksize = getBlockSize();
    segsize = 2*blocksize;
    irbuf = ComplexFloatArray::create(segsize);
    cbuf = ComplexFloatArray::create(segsize);
    rbuf = ComplexFloatArray::create(segsize);
    overlap = FloatArray::create(blocksize);
    overlap.clear();
    FloatArray ir = FloatArray::create(segsize);
    // todo: load IR and zero-pad to segment size
    ir.clear();
    fft.init(segsize);
    // do forward fft into irbuf
    fft.fft(ir, irbuf);
    // IR buffer can now be used as input buffer
    input = ir;
    input.clear();
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
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    input.copyFrom(left, blocksize);
    // forward fft
    fft.fft(input, cbuf);
    // complex vector multiplication
    // rbuf.clear();
    // cmac(rbuf, cbuf, irbuf);
    // test if this can be done in-place: cbuf.multiply(irbuf);
    cbuf.complexByComplexMultiplication(irbuf, rbuf);
    // inverse fft
    fft.ifft(rbuf, input);
    // copy first half to destination
    left.copyFrom(input, blocksize);
    // add previous overlap
    left.add(overlap);
    // save overlap
    overlap.copyFrom(input.subArray(blocksize, blocksize));
  }
};

#endif   // __ConvolutionPatch_hpp__
