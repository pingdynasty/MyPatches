#ifndef __SineOscPatch_hpp__
#define __SineOscPatch_hpp__

#include "StompBox.h"
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
  const float mul;
  float pos;
  VoltsPerOctave hz;
public:
  SineOscPatch(): mul(2*M_PI/getSampleRate()), pos(0) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Octave");
    registerParameter(PARAMETER_D, "Amplitude");
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*2.0-1.0;
    int octave = round(getParameterValue(PARAMETER_B)*8)-4;
    float gain = getParameterValue(PARAMETER_D);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune+octave);
    for(int n = 0; n<buffer.getSize(); n++){
      float frequency = hz.getFrequency(left[n]);
      float amp = hz.sampleToVolts(right[n]);
      amp = gain + amp*amp*0.5;
      left[n] = amp*sinf(pos);
      pos += frequency*mul;
    }
    if(pos > 2*M_PI)
      pos -= 4*M_PI;
    right.copyFrom(left);
  }
};

#endif   // __SineOscPatch_hpp__
