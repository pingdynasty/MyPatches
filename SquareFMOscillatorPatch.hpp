#ifndef __SquareFMOscillatorPatch_hpp__
#define __SquareFMOscillatorPatch_hpp__

#include "StompBox.h"
#include "Oscillators.hpp"
#include "VoltsPerOctave.h"

class SquareFMOscillatorPatch : public Patch {
private:
  SquareFMOscillator osc;
  VoltsPerOctave hz;
public:
  SquareFMOscillatorPatch(): osc(getSampleRate()) {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "FM Frequency");
    registerParameter(PARAMETER_C, "FM Index");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*10.0 - 2.0;
    float index = getParameterValue(PARAMETER_C)*0.2;
    float gain = getParameterValue(PARAMETER_D);
    float fm = getParameterValue(PARAMETER_B);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    hz.setTune(tune);
    float freq = hz.getFrequency(left[0]);
    fm *=  0.5 * freq;
    osc.setFrequency(freq);
    osc.setGain(gain);
    osc.setModulatorFrequency(fm);
    osc.setModulatorGain(index);
    debugMessage("freq/fm/index", freq, fm, index);
    if(isButtonPressed(PUSHBUTTON))
      osc.reset();
    osc.getSamples(left);
  }
};

#endif   // __SquareFMOscillatorPatch_hpp__
