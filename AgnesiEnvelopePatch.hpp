#ifndef __AgnesiEnvelopePatch_h__
#define __AgnesiEnvelopePatch_h__

#include "StompBox.h"
#include "SmoothValue.h"

/**
 * Envelope generator that produces two different curves:
 * Witch of Agnesi on left, Serpentine curve on right output channel.
 * https://www.desmos.com/calculator/lzwjq55uff
 * Witch of Agnesi
 * http://mathworld.wolfram.com/WitchofAgnesi.html
 *  y=(8a^3)/(x^2+4a^2), a=-0.5 to 0.5
 * Equivalent to the probability density function of the Cauchy distribution.
 * The cross-section of a single water wave has a shape similar to the Witch of Agnesi.
 * Serpentine curve
 * http://mathworld.wolfram.com/SerpentineCurve.html

 * todo: Tractrix curve
 * http://xahlee.info/SpecialPlaneCurves_dir/Tractrix_dir/tractrix.html
 */
class AgnesiEnvelopePatch : public Patch {
private:
  FloatArray envelope;
  float x;
  bool triggered;
public:
  AgnesiEnvelopePatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Radius");
    registerParameter(PARAMETER_C, "Offset");
    envelope = FloatArray::create(getBlockSize());
    x = 0;
    y = 0;
  }
  float agnesi(float x, float a){
    return (8*a*a*a) / (x*x + 4*a*a);
  }
  float serpentine(float x, float a){
    // variation on serpentine curve: x / (x^2 + 1)
    return 3.5*a*x / (x*x*x*x+1);
  }
  void processAudio(AudioBuffer &buffer){
    float rate = getParameterValue(PARAMETER_A);
    rate = rate*rate*0.005;
    float a = getParameterValue(PARAMETER_B) - 0.5;
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
      left[i] = agnesi(x, a);
      right[i] = serpentine(x, a);
      x += rate;
    }
  }
};

#endif // __AgnesiEnvelopePatch_h__
