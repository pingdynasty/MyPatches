#ifndef __AgnesiEnvelopePatch_h__
#define __AgnesiEnvelopePatch_h__

#include "StompBox.h"
#include "SmoothValue.h"

// Witch of Agnesi
// http://mathworld.wolfram.com/WitchofAgnesi.html
//  y=(8a^3)/(x^2+4a^2), 

class AgnesiEnvelopePatch : public Patch {
private:
  FloatArray envelope;
  float x;
  float y;
  bool triggered;
public:
  AgnesiEnvelopePatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Radius");
    registerParameter(PARAMETER_C, "Offset");
    envelope = FloatArray::create(getBlockSize());
    x = 0;
  }
  float agnesi(float x, float a){
    return (8*a*a*a) / (x*x + 4*a*a);
  }
  void processAudio(AudioBuffer &buffer){
    float rate = getParameterValue(PARAMETER_A);
    rate = rate*rate*0.005;
    float a = getParameterValue(PARAMETER_B)*2 - 1;
    if(!triggered && isButtonPressed(PUSHBUTTON)){
      x = -6+getParameterValue(PARAMETER_C)*6;
      triggered = true;
    }else{
      triggered = false;
    }
    int size = buffer.getSize();
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    for(int i=0; i<size; ++i){
      y = agnesi(x, a);
      left[i] = y;
      right[i] *= y;
      x += rate;
    }
  }
};

#endif // __AgnesiEnvelopePatch_h__
