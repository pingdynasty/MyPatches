#ifndef __OneLinePatch_hpp__
#define __OneLinePatch_hpp__

#include "StompBox.h"
#include "OneLineFormulas.h"

class OneLinerPatch : public Patch {
private:
  uint32_t time = 0;
public:
  OneLinerPatch(){
    registerParameter(PARAMETER_A, "Program");
    registerParameter(PARAMETER_B, "Rate");
    registerParameter(PARAMETER_C, "Start");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer) {
    uint8_t index = getParameterValue(PARAMETER_A)*65+1;
    int rate = 16-getParameterValue(PARAMETER_B)*16;
    uint32_t startpos = getParameterValue(PARAMETER_C)*65000;
    float gain = getParameterValue(PARAMETER_D);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    if(isButtonPressed(PUSHBUTTON))
      time = startpos;
    for(int i=0; i<buffer.getSize();){
      uint16_t out = calculate(index, time);
      if(time++ == 65532)
	time = startpos;
      out = ~out;
      uint16_t toplimit = ((out & 0x0f) << 3) + 0x84;
      toplimit <<= ((unsigned)out & 0x70) >> 4;
      toplimit = ((out & 0x80) ? (0x84 - toplimit) : (toplimit - 0x84));
      float sample = gain * ((toplimit>>8) / 255.0f) - 0.5f;
      rate = max(1, min(buffer.getSize()-i, rate));
      for(int n=0; n<rate; ++n)
	left[i++] = sample;
    }
  }
};

#endif   // __OneLinePatch_hpp__
