#ifndef __TriadPatch_hpp__
#define __TriadPatch_hpp__

#include "StompBox.h"
#include "PitchDetector.hpp"
#include "SmoothValue.h"
#include "PolyBlepOscillator.h"

/**
 * Mono patch that tracks the frequency of the audio input and produces a matching tone on the output.
 * With variable waveform shape (tri to PWM square), pitch offset, and portamento/glide.
 */
#define SEMI    (1.0/12)
#define WHOLE   (2.0/12)
static const float TRIAD_TONES[4][3] = {
  {0, 4, 7},
  {0, 3, 7},
  {0, 3, 6},
  {0, 4, 8}
}

class TriadPatch : public Patch {
private:
  PolyBlepOscillator osc;
  ZeroCrossingPitchDetector zcc;
  SmoothFloat freq;
  SmoothFloat gain;
  FloatArray output;
public:
  TriadPatch() :
    osc(getSampleRate()), 
    zcc(getSampleRate(), getBlockSize()),
    freq(0.9, 440.0),
    gain(0.9) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Glide");
    registerParameter(PARAMETER_C, "Shape");
    registerParameter(PARAMETER_D, "Dry/Wet");
    zcc.setLowPassCutoff(40);
    zcc.setHighPassCutoff(8000);
    output = FloatArray::create(getBlockSize());
  }      
  ~TriadPatch(){
    FloatArray::destroy(output);
  }
  void processAudio(AudioBuffer &buffer) {
    float portamento = getParameterValue(PARAMETER_B)*0.1+0.9;
    freq.lambda = portamento;
    float tune = getParameterValue(PARAMETER_A)*1.5+0.5; // up/down 1 octave
    float shape = getParameterValue(PARAMETER_C)*2;
    if(shape > 1.0){
      osc.setShape(1.0);
      osc.setPulseWidth(0.48*shape);
    }else{
      osc.setShape(shape);
      osc.setPulseWidth(0.48);
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    zcc.process(left);
    freq = zcc.getFrequency()*tune;
    float rms = left.getRms();
    if(rms > 0.1)
      gain = rms*1.66;
    else
      gain = 0.0;
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    left.multiply(dry);
    osc.setFrequency(freq);
    osc.getSamples(output);
    output.multiply(wet*gain);
    left.add(output);
  }
};

#endif   // __TriadPatch_hpp__
