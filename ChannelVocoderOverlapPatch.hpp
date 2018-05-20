#ifndef __ChannelVocoderPatch_hpp__
#define __ChannelVocoderPatch_hpp__

#include "Patch.h"
#include "FastFourierTransform.h"
#include "Window.h"

class ChannelVocoderOverlapPatch : public Patch {
private:
  int blocksize, fftsize, offset;
  ComplexFloatArray carrier;
  ComplexFloatArray modulator;
  FloatArray left, right, output;

  FastFourierTransform fft;
  Window window;
public:
  ChannelVocoderOverlapPatch() : blocksize(getBlockSize()), fftsize(blocksize*2), offset(blocksize/2) {
    carrier = ComplexFloatArray::create(fftsize);
    modulator = ComplexFloatArray::create(fftsize);
    left = FloatArray::create(fftsize);
    right = FloatArray::create(fftsize);
    output = FloatArray::create(fftsize);
    window = Window::create(Window::HannWindow, blocksize);
    registerParameter(PARAMETER_A, "Bands");
    registerParameter(PARAMETER_B, "Smooth");
    registerParameter(PARAMETER_C, "Gain");
    registerParameter(PARAMETER_D, "Dry/Wet");
    fft.init(fftsize);
  }

  void vocode(ComplexFloatArray modulator, ComplexFloatArray carrier, int bands, float smooth){
    int bandlength = (fftsize-2) / bands;
    int idx = 1; // skip first bin
    float mag, prev;
    for(int band=0; band<bands; ++band){
      mag = 0.0f;
      for(int i=0; i<bandlength; ++i)
	mag += modulator[idx+i].getMagnitude();
      mag /= bandlength;
      prev = mag * (1.0f - smooth) + prev * smooth;
      for(int i=0; i<bandlength; ++i)
	carrier[idx+i].setMagnitude(prev*(bandlength-i)/bandlength + mag*i/bandlength);
      idx += bandlength;
      prev = mag;
    }
    while(idx < fftsize)
      carrier[idx++].setMagnitude(mag);
  }

  void processAudio(AudioBuffer &buffer) {
    int bands = getParameterValue(PARAMETER_A)*(blocksize/4)+2;
    float smooth = getParameterValue(PARAMETER_B);
    float gain = getParameterValue(PARAMETER_C)*4; // Hann window processing loss is 1.76dB
    float wet = getParameterValue(PARAMETER_D);

    FloatArray input = buffer.getSamples(RIGHT_CHANNEL);
    window.apply(input);
    right.subArray(offset, blocksize).add(input);
    fft.fft(right, carrier);  // synth
    input = buffer.getSamples(LEFT_CHANNEL);
    window.apply(input);
    left.subArray(offset, blocksize).add(input);
    fft.fft(left, modulator); // voice

    vocode(modulator, carrier, bands, smooth);

    // inverse fft
    fft.ifft(carrier, output);

    input.copyFrom(output.subArray(offset, blocksize)); // copy over first `blocksize` samples
    input.multiply(gain);
    buffer.getSamples(RIGHT_CHANNEL).copyFrom(input);

    // prepare buffers for next round
    left.move(offset, 0, blocksize);
    window.apply(left); // applies window to first `blocksize` samples
    left.subArray(blocksize, fftsize-blocksize).clear();

    right.move(offset, 0, blocksize);
    window.apply(right);
    right.subArray(blocksize, fftsize-blocksize).clear();
  }
};

#endif   // __ChannelVocoderPatch_hpp__
