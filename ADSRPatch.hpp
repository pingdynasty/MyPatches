#ifndef __ADSRPatch_hpp__
#define __ADSRPatch_hpp__

#include "StompBox.h"
#include "Envelope.hpp"

class ADSRPatch : public Patch {
public:
  AdsrEnvelope env;
  FloatArray envBuffer;
  ADSRPatch() : env(getSampleRate()) {
    registerParameter(PARAMETER_A, "Attack");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Sustain");
    registerParameter(PARAMETER_D, "Release");
    envBuffer = FloatArray::create(getBlockSize());
  }
  ~ADSRPatch(){
    FloatArray::destroy(envBuffer);
  }
  void processAudio(AudioBuffer &buffer){
    //    debugMessage("button", isButtonPressed(PUSHBUTTON));
    env.setAttack(getParameterValue(PARAMETER_A)*4);
    env.setDecay(getParameterValue(PARAMETER_B)*4);
    env.setSustain(getParameterValue(PARAMETER_C));
    env.setRelease(getParameterValue(PARAMETER_D)*4);
    env.gate(isButtonPressed(PUSHBUTTON));
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    env.getSamples(envBuffer);
    left.multiply(envBuffer);
    right.multiply(envBuffer);
  }
};

#endif // __ADSRPatch_hpp__
