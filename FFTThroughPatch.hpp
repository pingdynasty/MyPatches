#ifndef __FFTThroughPatch_hpp__
#define __FFTThroughPatch_hpp__

#include "Patch.h"
#include "FastFourierTransform.h"
#include "Window.h"

class FFTThroughPatch : public Patch {
private:
  int blocksize;
  int fftsize;
  FastFourierTransform fft;
  FloatArray timedomain;
  ComplexFloatArray freqdomain;
  FloatArray overlap;
  Window inwin;
  Window outwin;
public:
  FFTThroughPatch(){
    blocksize = getBlockSize();
    fftsize = blocksize*2;
    timedomain = FloatArray::create(fftsize);
    freqdomain = ComplexFloatArray::create(fftsize); 
    overlap = FloatArray::create(blocksize);
    fft.init(fftsize);
    inwin = Window::create(Window::HannWindow, blocksize);
    outwin = Window::create(Window::TriangularWindow, fftsize);
    timedomain.clear();
    overlap.clear();
  }
  void processFreq(ComplexFloatArray bins){
    // do something with the phase and magnitude
    int idx = bins.getMaxMagnitudeIndex();
    debugMessage("max index/value/freq", (float)idx, bins.mag(idx), (float)idx*getSampleRate()/fftsize);
  }
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    inwin.apply(left);
    timedomain.copyFrom(left);
    fft.fft(timedomain, freqdomain);
    processFreq(freqdomain.subArray(0, blocksize));
    fft.ifft(freqdomain, timedomain);
    outwin.apply(timedomain);
    left.copyFrom(timedomain); // use first half of output
    left.add(overlap);
    overlap.copyFrom(timedomain.subArray(blocksize, blocksize)); // save second half
  }
};

#endif   // __FFTThroughPatch_hpp__
