#ifndef __ConvolutionPatch_hpp__
#define __ConvolutionPatch_hpp__

#include "StompBox.h"
#include "FastFourierTransform.h"
#include "Resource.h"
// ./wav2float MyPatches/ir/EMT\ 244\ \(A.\ Bernhard\)/0\,4s\ Low\*2\ \ High_2.wav > MyPatches/impulse-response.h

#define IR_RESOURCE "impulse1"

class ConvolutionPatch : public Patch {
private:
  static const int segments = 11;
  int current;
  int segsize;
  int blocksize;
  ComplexFloatArray irbuf[segments];
  ComplexFloatArray cinbuf[segments];
  ComplexFloatArray coutbuf;
  ComplexFloatArray cmulbuf;
  FloatArray input;
  FloatArray overlap;
  FastFourierTransform fft;
public:
  ConvolutionPatch() : current(0), blocksize(getBlockSize()), segsize(2*getBlockSize()) {
    // ASSERT(sizeof(ir)/sizeof(float) >= segments*blocksize, "Impulse response array too small");
    input = FloatArray::create(segsize);
    overlap = FloatArray::create(blocksize);
    coutbuf = ComplexFloatArray::create(segsize);
    cmulbuf = ComplexFloatArray::create(segsize);
    overlap.clear();
    fft.init(segsize);

    Resource resource = Resource::getResource(IR_RESOURCE);
    ASSERT(resource, "Missing resource " IR_RESOURCE);

    // FloatArray impulse(resource.getData()+2, resource.getSize()-2);
    FloatArray impulse((float*)resource.getData()+2, resource.getSize()/sizeof(float)+2);
    debugMessage("ir size/min/max", resource.getSize(), impulse.getMaxValue(), impulse.getMinValue());
    
    // normalise IR level
    // FloatArray impulse(ir[0], segments*blocksize);
    float scale = 1.0f/max(impulse.getMaxValue(), -impulse.getMinValue());
    // impulse.multiply(scale);
    // debugMessage("normalised", scale);

    // do forward fft into irbuf
    for(int i=0; i<segments; ++i){
      // allocate fft buffers
      irbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i].clear();
      // load IR and zero-pad to segment size
      input.clear();
      input.copyFrom(impulse.subArray(i*blocksize, blocksize));
      fft.fft(input, irbuf[i]);
      // fft.fft(impulse.subArray(i*blocksize, blocksize), irbuf[i]);
    }
    registerParameter(PARAMETER_D, "Wet/Dry");
  }

  void processAudio(AudioBuffer &buffer) {
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0f - wet;
    wet *= 1.0f/segments; // gain adjust

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
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
      left[i] = wet*(input[i]+overlap[i]) + dry*left[i];

    // save overlap
    overlap.copyFrom(input.subArray(blocksize, blocksize));
    current = (current > 0) ? (current - 1) : (segments - 1);
  }
};

#endif   // __ConvolutionPatch_hpp__
