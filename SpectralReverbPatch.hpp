#ifndef __SpectralFreezePatch_hpp__
#define __SpectralFreezePatch_hpp__

#include "StompBox.h"
#include "FastFourierTransform.h"
// ./wav2float MyPatches/ir/EMT\ 244\ \(A.\ Bernhard\)/0\,4s\ Low\*2\ \ High_2.wav > MyPatches/impulse-response.h

class SpectralFreezePatch : public Patch {
private:
  enum PatchMode {
    RECORD,
    FREEZE
  };
  PatchMode mode = RECORD;
  static const int segments = 4;
  int current;
  int segsize;
  int blocksize;
  FloatArray noise;
  ComplexFloatArray irbuf;//[segments];
  // FloatArray noise;
  ComplexFloatArray cinbuf[segments];
  ComplexFloatArray coutbuf;
  ComplexFloatArray cmulbuf;
  FloatArray input;
  FloatArray overlap;
  FastFourierTransform fft;
public:
  SpectralFreezePatch() : current(0), blocksize(getBlockSize()), segsize(2*getBlockSize()) {
    input = FloatArray::create(segsize);
    
    coutbuf = ComplexFloatArray::create(segsize);
    cmulbuf = ComplexFloatArray::create(segsize);
    overlap = FloatArray::create(blocksize);
    overlap.clear();
    fft.init(segsize);

    noise = FloatArray::create(segsize*2);
    irbuf = ComplexFloatArray((ComplexFloat*)noise.getData(), noise.getSize());

    // do forward fft into irbuf
    for(int i=0; i<segments; ++i){
      // allocate fft buffers
      cinbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i].clear();
    }
    registerParameter(PARAMETER_D, "Wet/Dry");
  }

    void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON && value)
      mode = mode == FREEZE ? RECORD : FREEZE;
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray samples = buffer.getSamples(LEFT_CHANNEL);
    switch(mode){
    case RECORD:
      record(samples);
      break;
    case FREEZE:
      freeze(samples);
      break;
    }
    // float wet = getParameterValue(PARAMETER_D);
    // for(int n=0; n<samples.getSize(); ++n)
    //   samples[n] = samples[n]*(1-wet) + dest[n]*wet;

    void record(FloatArray samples){
      input.clear();
      input.copyFrom(samples, blocksize);
    }
    void freeze(FloatArray samples){

    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0f - wet;
    wet *= 1.0f/segments; // gain adjust

    // forward fft
    fft.fft(input, cinbuf[current]);

    // complex multiply and accumulate
    noise.noise();
    irbuf.complexByComplexMultiplication(cinbuf[current], coutbuf);
    for(int i=1; i<segments; ++i){
      int seg = (current+i) % segments;
      noise.noise();
      irbuf.complexByComplexMultiplication(cinbuf[seg], cmulbuf);
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

#endif   // __SpectralFreezePatch_hpp__