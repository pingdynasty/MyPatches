#ifndef __FFTShiftPatch_hpp__
#define __FFTShiftPatch_hpp__

#include "Patch.h"
#include "FastFourierTransform.h"
#include "Window.h"

class FFTShiftPatch : public Patch {
private:
  int blocksize;
  int fftsize;
  FastFourierTransform fft;
  FloatArray timedomain;
  ComplexFloatArray freqdomain;
  FloatArray overlap;
  Window inwin;
  Window outwin;
  FloatArray magnitudes;
  SmoothFloat shift;
  const int M = 4;
public:
  FFTShiftPatch(){
    blocksize = getBlockSize();
    fftsize = blocksize*M;
    timedomain = FloatArray::create(fftsize);
    freqdomain = ComplexFloatArray::create(fftsize); 
    overlap = FloatArray::create(blocksize);
    fft.init(fftsize);
    inwin = Window::create(Window::HannWindow, blocksize);
    outwin = Window::create(Window::TriangularWindow, blocksize*2);
    timedomain.clear();
    overlap.clear();
    magnitudes = FloatArray::create(fftsize/2);
  }
  void processFreq(ComplexFloatArray bins){
    // do something with the phase and magnitude
    int idx = bins.getMaxMagnitudeIndex();
    // debugMessage("max index/value/freq", (float)idx, bins.mag(idx), (float)idx*getSampleRate()/fftsize);
    bins.getMagnitudeValues(magnitudes);
    shift = getParameterValue(PARAMETER_A)*fftsize/8;
    magnitudes.move(0, shift, magnitudes.getSize()-shift);
    magnitudes.subArray(0, shift).clear();
    bins.setMagnitude(magnitudes);
  }
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    inwin.apply(left);
    timedomain.copyFrom(left);
    timedomain.subArray(blocksize, fftsize-blocksize).clear();
    fft.fft(timedomain, freqdomain);
    processFreq(freqdomain.subArray(0, fftsize/2));
    fft.ifft(freqdomain, timedomain);
    outwin.apply(timedomain);
    left.copyFrom(timedomain); // use first half of output
    left.add(overlap);
    overlap.copyFrom(timedomain.subArray(blocksize, blocksize)); // save second half
  }
};

#endif   // __FFTShiftPatch_hpp__
