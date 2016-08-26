#ifndef __PerlinPatch_hpp__
#define __PerlinPatch_hpp__

#include "StompBox.h"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

class PerlinPatch : public Patch {
private:
public:
  PerlinPatch(){
  }
  ~PerlinPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float x = getParameterValue(PARAMETER_A);
    float y = getParameterValue(PARAMETER_B);
    float z = getParameterValue(PARAMETER_C);
    int wrap = getParameterValue(PARAMETER_D)*8;
    wrap = 1<<wrap;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    for(int i=0; i<buffer.getSize(); ++i){
      left[i] = stb_perlin_noise3(x*wrap+left[i], y*wrap+right[i], z*wrap, wrap, wrap, wrap);
      right[i] = stb_perlin_noise3(x*wrap+right[i], y*wrap+left[i], z*wrap, wrap, wrap, wrap);
    }
  }
};

#endif   // __PerlinPatch_hpp__
