#include "StompBox.h"
#include <stdlib.h>
class ShepardTonesPatch : public Patch {
private:
  const static int DDS_LENGTH = 1<<14;
  const static int WIN_LENGTH = 1<<14;
  const static int TONES = 12;
  float acc[TONES];
  float inc[TONES];
  Window amp;
  FloatArray sine;
public:
  ShepardTonesPatch(){
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Range");
    registerParameter(PARAMETER_C, "Tones");
    registerParameter(PARAMETER_D, "Rate");
    sine = FloatArray::create(DDS_LENGTH);
    for(int i=0; i<DDS_LENGTH; ++i)
      sine[i] = sin(2*M_PI*i/(DDS_LENGTH-1));
    amp = Window::create(Window::HannWindow, WIN_LENGTH);
    amp.multiply(1.0/TONES);
    for(int i=0; i<TONES; i++)
      acc[i] = 0.0;
    inc[0] = 0.001;
    for(int i=1; i<TONES; i++)
      inc[i] = inc[i-1]*2;
  }
  ~ShepardTonesPatch(){
    FloatArray::destroy(sine);
    Window::destroy(amp);
  }
  float getAmplitude(float inc){
    uint32_t index = inc*(WIN_LENGTH-1);
    index = min(index, WIN_LENGTH-1);
    return amp[index];
  }
  float getWave(float phase){
    uint32_t index = phase*(DDS_LENGTH-1);
    index = min(index, WIN_LENGTH-1);
    return sine[index];
  }
  void processAudio(AudioBuffer& buf){
    float minf = getParameterValue(PARAMETER_A)*0.1+0.002;
    float maxf = min(0.4, 2*minf + getParameterValue(PARAMETER_B)*0.2);
    int tones = getParameterValue(PARAMETER_C)*(TONES-1)+1;
    float rate = 1.0 + (getParameterValue(PARAMETER_D) - 0.5)*0.00002;
    int size = buf.getSize();
    FloatArray out = buf.getSamples(LEFT_CHANNEL);
    float amp;
    for(int i=0; i<size; ++i){
      for(int t=0; t<tones; ++t){
	amp = getAmplitude((inc[t]-minf)/(maxf-minf));
	out[i] += amp * getWave(acc[t]);
        acc[t] += inc[t];
	if(acc[t] > 1.0)
	  acc[t] -= 1.0;
	else if(acc[t] < 0.0)
	  acc[t] += 1.0;
        inc[t] *= rate;
        if(inc[t] > maxf)
	  inc[t] = inc[t] - maxf + minf;
        else if(inc[t] < minf)
	  inc[t] = inc[t] - minf + maxf;
      }
    }
  }
};
