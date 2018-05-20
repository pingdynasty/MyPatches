#ifndef __SpectralReverbPatch_hpp__
#define __SpectralReverbPatch_hpp__

#include "StompBox.h"
#include "FastFourierTransform.h"

class SpectralReverbPatch : public Patch {
private:
  static const int MAX_SEGMENTS = 16;
  int segments = MAX_SEGMENTS;
  int current;
  int segsize;
  int blocksize;
  FloatArray noise;
  ComplexFloatArray kernel;
  ComplexFloatArray cinbuf[MAX_SEGMENTS];
  ComplexFloatArray coutbuf;
  ComplexFloatArray cmulbuf;
  FloatArray input;
  FloatArray overlap;
  FastFourierTransform fft;
public:
  SpectralReverbPatch() : current(0), blocksize(getBlockSize()), segsize(2*getBlockSize()) {
    input = FloatArray::create(segsize);
    coutbuf = ComplexFloatArray::create(segsize);
    cmulbuf = ComplexFloatArray::create(segsize);
    overlap = FloatArray::create(blocksize);
    overlap.clear();
    fft.init(segsize);
    noise = FloatArray::create(segsize*2);
    kernel = ComplexFloatArray((ComplexFloat*)noise.getData(), segsize);
    for(int i=0; i<segments; ++i){
      // allocate fft buffers
      cinbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i].clear();
    }
    registerParameter(PARAMETER_A, "Amount");
    registerParameter(PARAMETER_D, "Wet/Dry");
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray samples = buffer.getSamples(LEFT_CHANNEL);
    float wet = getParameterValue(PARAMETER_D);
    segments = getParameterValue(PARAMETER_B)*15+2;
    float dry = 1.0f - wet;
    wet *= 1.0f/(segments-1); // gain adjust

    // forward fft
    input.clear();
    input.copyFrom(samples, blocksize);
    fft.fft(input, cinbuf[current]);

    float amount = getParameterValue(PARAMETER_A)*4;
    noise.noise(amount, amount);
    // complex multiply and accumulate
    kernel.complexByComplexMultiplication(cinbuf[current], coutbuf);
    for(int i=1; i<segments; ++i){
      int seg = (current+i) % segments;
      noise.noise(amount, amount);
      kernel.complexByComplexMultiplication(cinbuf[seg], cmulbuf);
      coutbuf.add(cmulbuf);
    }
    // inverse fft
    fft.ifft(coutbuf, input);

    for(int i=0; i<blocksize; ++i)
      samples[i] = wet*(input[i]+overlap[i]) + dry*samples[i];

    // save overlap
    overlap.copyFrom(input.subArray(blocksize, blocksize));
    current = (current > 0) ? (current - 1) : (segments - 1);
  }
};

#endif   // __SpectralReverbPatch_hpp__
