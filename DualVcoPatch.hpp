#ifndef __DualVcoPatch_hpp__
#define __DualVcoPatch_hpp__

#include "StompBox.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"

/**
 * Two oscillators. Osc2 can FM osc1 and they both can take FM 
 * from audio inputs
 * A: Pitch1
 * B: Pitch2
 * C: Amount of frequency modulation osc2->osc1
 * D: Waveform
 * Left: Pitch1
 * Right: Pitch2
 * 
 */
class DualVcoPatch : public Patch {
private:
  PolyBlepOscillator osc1;
  PolyBlepOscillator osc2;
  VoltsPerOctave hz1;
  VoltsPerOctave hz2;
  FloatArray temp;
public:
  DualVcoPatch() : osc1(getSampleRate()), osc2(getSampleRate()) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Detune");
    registerParameter(PARAMETER_C, "Frequency modulation");
    registerParameter(PARAMETER_D, "Waveform");
    temp = FloatArray::create(getBlockSize());
  }
  ~DualVcoPatch(){
    FloatArray::destroy(temp);
  }
  void processAudio(AudioBuffer &buffer) {
    float tune1 = getParameterValue(PARAMETER_A)*10.0 - 6.0;
    float tune2 = tune1 * getParameterValue(PARAMETER_B)*2.0f;
    float modulationIndex = (getParameterValue(PARAMETER_C));
    modulationIndex = modulationIndex < 0.1 ? 0 : (modulationIndex - 0.1) / 0.9;
    float shape = getParameterValue(PARAMETER_D) * 2.0;
    float pw = 0.5;
    if(shape > 1.0){
      pw += 0.49*(shape-1.0); // pw 0.5 to 0.99
      shape = 1.0; // square wave
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    hz1.setTune(tune1);
    osc1.setShape(shape);
    osc1.setPulseWidth(pw);

    hz2.setTune(tune2);
    osc2.setShape(shape);
    osc2.setPulseWidth(pw);
    
    hz2.getFrequency(right, temp);
    osc2.getSamples(right, temp);
    
    // osc1's frequency is modulated by the left input
    // and by osc2's output
    right.multiply(2.5 * modulationIndex, temp); 
    temp.add(left);
    hz1.getFrequency(temp);
    osc1.getSamples(left, temp);
      
    right.add(left);
    right.multiply(0.5);
  }
};

#endif   // __DualVcoPatch_hpp__
