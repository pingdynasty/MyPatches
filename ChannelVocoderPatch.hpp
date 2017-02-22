#ifndef __ChannelVocoderPatch_hpp__
#define __ChannelVocoderPatch_hpp__

#include "Patch.h"
#include "FastFourierTransform.h"
#include "Window.h"

class ChannelVocoderPatch : public Patch {
private:
  int blocksize;
  ComplexFloatArray carrier;
  ComplexFloatArray modulator;
  FastFourierTransform fft;
  Window window;
public:
  ChannelVocoderPatch() : blocksize(getBlockSize()) {
    carrier = ComplexFloatArray::create(blocksize);
    modulator = ComplexFloatArray::create(blocksize);
    window = Window::create(Window::HannWindow, blocksize);
    registerParameter(PARAMETER_A, "Bands");
    registerParameter(PARAMETER_B, "Smooth");
    registerParameter(PARAMETER_C, "Gain");
    registerParameter(PARAMETER_D, "Dry/Wet");
    fft.init(blocksize);
  }

  void vocode(ComplexFloatArray modulator, ComplexFloatArray carrier, int bands, float smooth){
    int bandlength = (blocksize-2) / bands;
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
    while(idx < blocksize)
      carrier[idx++].setMagnitude(mag);
  }

  void processAudio(AudioBuffer &buffer) {
    int bands = getParameterValue(PARAMETER_A)*(blocksize/4)+2;
    float smooth = getParameterValue(PARAMETER_B);
    float gain = getParameterValue(PARAMETER_C)*4; // Hann window processing loss is 1.76dB
    float wet = getParameterValue(PARAMETER_D);

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    window.apply(left);
    window.apply(right);

    // forward fft
    fft.fft(left, modulator); // voice
    fft.fft(right, carrier);  // synth

    vocode(modulator, carrier, bands, smooth);

    // inverse fft
    fft.ifft(carrier, left);

    // dry/wet
    left.multiply(wet*gain);
    right.multiply(1.0f - wet);
    right.add(left);
  }
};

#endif   // __ChannelVocoderPatch_hpp__
