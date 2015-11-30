#ifndef __TrickySineOscPatch_hpp__
#define __TrickySineOscPatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"

/*
 * Digitally controlled non-linear oscillator based on the function
 * sin(x)^2 + sin(x^2), x=-8.953 to 8.953
 * Left input: pitch
 * Right input: amplitude
 * Left/right output: tricky wave
 */
class TrickySineOscPatch : public Patch {
private:
  const float mul;
  float pos = 0.0f;
  VoltsPerOctave hz;
  const float HALFPERIOD = 8.953;
  float wave(float x){
    x *= HALFPERIOD;
    float sx = sinf(x);
    return sx*sx+sinf(x*x);
  }
public:
  TrickySineOscPatch() : mul(1.0/getSampleRate()) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Octave");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*2.0 - 1.0;
    int octave = round(getParameterValue(PARAMETER_B)*8)-6;
    float gain = getParameterValue(PARAMETER_D);
    gain = gain*gain*0.8;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune+octave);
    for(int n = 0; n<buffer.getSize(); n++){
      float frequency = hz.getFrequency(left[n]);
      float amp = hz.sampleToVolts(right[n]);
      amp = gain + amp*amp*0.5;
      left[n] = amp*wave(pos);
      right[n] = left[n];
      pos += frequency*mul;
      if(pos > 1.0)
	pos -= 2.0;
    }
  }
};

#endif   // __TrickySineOscPatch_hpp__
