#ifndef __ADSRPatch_hpp__
#define __ADSRPatch_hpp__

#include "StompBox.h"
#include "Envelope.h"

class ADSRPatch : public Patch {
public:
  AdsrEnvelope adsr;
  FloatArray envelope;
  ADSRPatch() : adsr(getSampleRate()) {
    registerParameter(PARAMETER_A, "Attack");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Sustain");
    registerParameter(PARAMETER_D, "Release");
    envelope = FloatArray::create(getBlockSize());
  }
  ~ADSRPatch(){
    FloatArray::destroy(envelope);
  }
  void processAudio(AudioBuffer &buffer){
    adsr.setAttack(getParameterValue(PARAMETER_A)*4);
    adsr.setDecay(getParameterValue(PARAMETER_B)*4);
    adsr.setSustain(getParameterValue(PARAMETER_C));
    adsr.setRelease(getParameterValue(PARAMETER_D)*4);
    adsr.gate(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    adsr.getEnvelope(envelope);
    left.multiply(envelope);
    right.multiply(envelope);
  }
};

#endif // __ADSRPatch_hpp__
