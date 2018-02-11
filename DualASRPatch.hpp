#ifndef __DualASRPatch_hpp__
#define __DualASRPatch_hpp__

#include "StompBox.h"
#include "Envelope.h"

class DualASRPatch : public Patch {
public:
  AdsrEnvelope eg1, eg2;
  DualASRPatch() : eg1(getSampleRate()), eg2(getSampleRate()) {
    registerParameter(PARAMETER_A, "Attack");
    registerParameter(PARAMETER_B, "Release");
    registerParameter(PARAMETER_C, "Attack");
    registerParameter(PARAMETER_D, "Release");
    eg1.setSustain(1.0);
    eg1.setDecay(0.0);
    eg2.setSustain(1.0);
    eg2.setDecay(0.0);
  }
  ~DualASRPatch(){
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON){
      eg1.gate(value, samples);
      eg2.gate(value, samples);
    }
  }
  void processAudio(AudioBuffer &buffer){
    eg1.setAttack(getParameterValue(PARAMETER_A)*2);
    eg1.setRelease(getParameterValue(PARAMETER_B)*2);
    eg2.setAttack(getParameterValue(PARAMETER_C)*2);
    eg2.setRelease(getParameterValue(PARAMETER_D)*2);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    eg1.getEnvelope(left);
    eg2.getEnvelope(right);
    left.multiply(-1);
    right.multiply(-1);
  }
};

#endif // __DualASRPatch_hpp__
