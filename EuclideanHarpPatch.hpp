#ifndef __EuclideanHarpPatch_hpp__
#define __EuclideanHarpPatch_hpp__

#include "StompBox.h"
#include "SmoothValue.h"
#include "Sequence.h"
// #include "PolyBlepOscillator.h"
#include "KarplusStrongOscillator.hpp"

class EuclideanHarpPatch : public Patch {
private:
  KarplusStrongOscillator* osc[2];
  SmoothFloat freq;
  SmoothFloat gain;
  bool buttonstate;
  Sequence<uint16_t> seq;
  float frequency;
public:
  EuclideanHarpPatch() :
    freq(0.9, 440.0),
    gain(0.9) {
    frequency = 880;
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Steps");
    registerParameter(PARAMETER_C, "Fills");
    registerParameter(PARAMETER_D, "Divs");
    registerParameter(PARAMETER_E, "Decay");
    for(int i=0; i<2; ++i){
      osc[i] = KarplusStrongOscillator::create(getSampleRate(), 1024);
      // osc[i]->setSampleRate(getSampleRate());
      osc[i]->setFrequency(frequency);
    }
    buttonstate = 0;
  }
  ~EuclideanHarpPatch(){
  }
  void processAudio(AudioBuffer &buffer) {
    frequency = getParameterValue(PARAMETER_A)*1705+55;
    int steps = getParameterValue(PARAMETER_B)*16;
    int fills = getParameterValue(PARAMETER_C)*steps+1;
    int divs = getParameterValue(PARAMETER_D)*12+1;
    float decay = getParameterValue(PARAMETER_E);
    osc[0]->setDecay(decay);
    osc[1]->setDecay(decay);
    seq.calculate(steps, fills);
    if(isButtonPressed(PUSHBUTTON) != buttonstate){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate){ // leading edge
	bool on = seq.next();
	int pos = seq.getPosition()+1;
	if(on){
	  osc[0]->setFrequency(frequency*pos/divs);
	  osc[1]->setFrequency(frequency*divs/pos);
	  osc[0]->trigger();
	  osc[1]->trigger();
	}
      }
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    osc[0]->getSamples(left);
    osc[1]->getSamples(right);
    // env.getEnvelope(envelope);
    // left.multiply(envelope);
    // right.multiply(envelope);
  }
};

#endif   // __EuclideanHarpPatch_hpp__
