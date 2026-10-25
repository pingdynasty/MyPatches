#ifndef __SineOscPatch_hpp__
#define __SineOscPatch_hpp__

#include "Patch.h"
#include "SmoothValue.h"
#include "SineOscillator.h"
#include "VoltsPerOctave.h"

/**
 * OWL Modular oscillator patch.
 * Tracks 1v/octave on left input channel.
 * Produces a sine wave on the left and right outputs.
 * 0v dc input generates 440Hz output signal
 * Left input: pitch
 * Right input: amplitude
 * Left/right output: sine wave
 */
class SineOscPatch : public Patch {
private:
  SineOscillator* osc;
  VoltsPerOctave hz;
  SmoothFloat tune;
  StiffInt octave;
  SmoothFloat gain;
public:
  SineOscPatch() {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Octave");
    registerParameter(PARAMETER_D, "Amplitude");
    osc = SineOscillator::create(getSampleRate());
  }
  void processAudio(AudioBuffer &buffer) {
    tune = getParameterValue(PARAMETER_A)*2.0-1.0;
    octave = round(getParameterValue(PARAMETER_B)*8)-4;
    gain = getParameterValue(PARAMETER_D);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune+octave);
    for(int n = 0; n<buffer.getSize(); n++){
      float frequency = hz.getFrequency(left[n]);
      osc->setFrequency(frequency);
      float amp = hz.sampleToVolts(right[n]);
      amp = gain + amp*amp*0.05;
      left[n] = amp*osc->generate();
    }
    right.copyFrom(left);
  }
};

#endif   // __SineOscPatch_hpp__
