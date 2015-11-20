#ifndef __BesselFunctionPatch_hpp__
#define __BesselFunctionPatch_hpp__

#include "StompBox.h"

/*
 * Bessel function of the first kind.
 * Outputs Bessel function J of order 1 on left channel.
 * Right channel is multiplied by the same function.
 * http://mathworld.wolfram.com/BesselFunctionoftheFirstKind.html
 * http://www.boost.org/doc/libs/1_38_0/libs/math/doc/sf_and_dist/html/math_toolkit/special/bessel/bessel.html
 */
class BesselFunctionPatch : public Patch {
private:
  float x;
  float y0;
public:
  BesselFunctionPatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Range");
    registerParameter(PARAMETER_D, "Gain");
    x = 0;
    y0 = 0;
  }
  void processAudio(AudioBuffer &buffer){
    float rate = (getParameterValue(PARAMETER_A))*0.8;
    float range = getParameterValue(PARAMETER_B)*10000+10;
    float gain = getParameterValue(PARAMETER_D)*10;
    if(isButtonPressed(RED_BUTTON))
      rate /= 10;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    for(int i=0; i<size; i+=4){
      float f = gain*j1f(x);
      right[i] = f*0.25 + y0*0.75;
      right[i+1] = f*0.5 + y0*0.5;
      right[i+2] = f*0.75 + y0*0.25;
      right[i+3] = f;
      y0 = f;
      x += rate;
      if(x > range && abs(f) < 0.01)
	x *= -1;
    }
    left.multiply(right);
  }
};

#endif   // __BesselFunctionPatch_hpp__
