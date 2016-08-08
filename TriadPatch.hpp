#ifndef __TriadPatch_hpp__
#define __TriadPatch_hpp__

#include "StompBox.h"
#include "PitchDetector.h"
#include "SmoothValue.h"
#include "PolyBlepOscillator.h"

/**
 * Triple tone generator that produces a triad chord that tracks the frequency of the input audio signal.
 * Select between three inversions of each triad: Diminished, Minor, Major and Augmented.
 * ref: https://en.wikipedia.org/wiki/Triad_(music)
 */
#define SEMI    (1.0/12)
#define WHOLE   (2.0/12)
#define TRIADS 5
static const float TRIAD_SEMITONES[TRIADS][5] = {
  {0, 1, 6, 12, 12+1}, // Viennese trichord
  {0, 3, 6, 12, 12+3}, // Diminishied triad
  {0, 3, 7, 12, 12+3}, // Minor triad
  {0, 4, 7, 12, 12+4}, // Major triad
  {0, 4, 8, 12, 12+4}  // Augmented triad
};

class TriadPatch : public Patch {
private:
  PolyBlepOscillator osc[3];
  ZeroCrossingPitchDetector zcc;
  SmoothFloat freq;
  SmoothFloat gain;
  FloatArray output;
public:
  TriadPatch() :
    zcc(getSampleRate(), getBlockSize()),
    freq(0.9, 440.0),
    gain(0.9) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Triad");
    registerParameter(PARAMETER_C, "Shape");
    registerParameter(PARAMETER_D, "Dry/Wet");
    zcc.setLowPassCutoff(40);
    zcc.setHighPassCutoff(8000);
    output = FloatArray::create(getBlockSize());
    for(int i=0; i<3; ++i)
      osc[i].setSampleRate(getSampleRate());
  }      
  ~TriadPatch(){
    FloatArray::destroy(output);
  }
  void processAudio(AudioBuffer &buffer) {
    int triad = getParameterValue(PARAMETER_B)*TRIADS;
    int inversion = (getParameterValue(PARAMETER_B)*TRIADS-triad)*3;
    //    debugMessage("triad/inversion", triad, inversion);
    float tune = getParameterValue(PARAMETER_A);
    tune = tune < 0.5 ? tune + 0.5 : tune * 2.0; // plus/minus one octave
    float shape = getParameterValue(PARAMETER_C)*2;
    float pwm = shape > 1.0 ? 0.48*shape : 0.48;
    shape = shape > 1.0 ? 1.0 : shape;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    zcc.process(left);
    freq = zcc.getFrequency()*tune;
    float rms = left.getRms();
    if(rms > 0.1)
      gain = rms*1.66/3.0;
    else
      gain = 0.0;
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    left.multiply(dry);
    for(int i=0; i<3; ++i){
      float frequency = freq * (1.0+TRIAD_SEMITONES[triad][i+inversion]*1/12.0);
      osc[i].setShape(shape);
      osc[i].setPulseWidth(pwm);
      osc[i].setFrequency(frequency);
      osc[i].getSamples(output);
      output.multiply(wet*gain);
      left.add(output);
    }
  }
};

#endif   // __TriadPatch_hpp__
