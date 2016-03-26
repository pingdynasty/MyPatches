#ifndef __EuclideanArpeggioPatch_hpp__
#define __EuclideanArpeggioPatch_hpp__

#include "StompBox.h"
#include "SmoothValue.h"
#include "Envelope.h"
#include "Sequence.h"
#include "PolyBlepOscillator.h"

class EuclideanArpeggioPatch : public Patch {
private:
  AdsrEnvelope env;
  PolyBlepOscillator osc[3];
  FloatArray envelope;
  SmoothFloat freq;
  SmoothFloat gain;
  bool buttonstate;
  Sequence<uint16_t> seq;
  float frequency;
public:
  EuclideanArpeggioPatch() :
    env(getSampleRate()),
    freq(0.9, 440.0),
    gain(0.9) {
    frequency = 880;
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Steps");
    registerParameter(PARAMETER_C, "Fills");
    registerParameter(PARAMETER_D, "Divs");
    for(int i=0; i<3; ++i){
      osc[i].setSampleRate(getSampleRate());
      osc[i].setShape(0.5);
      osc[i].setPulseWidth(0.5);
      osc[i].setFrequency(frequency);
    }
    env.setSustain(1.0);
    env.setDecay(0.1);
    env.setRelease(0.1);
    envelope = FloatArray::create(getBlockSize());
    buttonstate = 0;
  }
  ~EuclideanArpeggioPatch(){
    FloatArray::destroy(envelope);
  }
  void processAudio(AudioBuffer &buffer) {
    frequency = getParameterValue(PARAMETER_A)*1705+55;
    int steps = getParameterValue(PARAMETER_B)*16;
    int fills = getParameterValue(PARAMETER_C)*steps+1;
    int divs = getParameterValue(PARAMETER_D)*12+1;
    seq.calculate(steps, fills);    
    if(isButtonPressed(PUSHBUTTON) != buttonstate){
      buttonstate = isButtonPressed(PUSHBUTTON);
      if(buttonstate){ // leading edge
	bool on = seq.next();
	int pos = seq.getPosition()+1;
	osc[0].setFrequency(frequency*pos/divs);
	osc[1].setFrequency(frequency*divs/pos);
	env.gate(on, getSamplesSinceButtonPressed(PUSHBUTTON));
      }else{
	env.gate(false);
      }
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    osc[0].getSamples(left);
    osc[1].getSamples(right);
    env.getEnvelope(envelope);
    left.multiply(envelope);
    right.multiply(envelope);
  }
};

#endif   // __EuclideanArpeggioPatch_hpp__
