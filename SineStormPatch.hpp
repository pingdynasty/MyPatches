#include "StompBox.h"

#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "SmoothFloat.hpp"

/*
 * Oscillator that generates a cluster of sine waves
 */
class SineStormPatch : public Patch {
private:
  const static int DDS_LENGTH = 1<<14;
  const static int TONES = 18;
  float acc[TONES];
  SmoothFloat inc[TONES];
  SmoothFloat gain[TONES];
  FloatArray sine;
  VoltsPerOctave hz;
  const float mul;
public:
  SineStormPatch() :
    mul(1.0/getSampleRate()){
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Tones");
    registerParameter(PARAMETER_C, "Spread");
    registerParameter(PARAMETER_D, "Amplitude");
    registerParameter(PARAMETER_E, "Portamento");
    sine = FloatArray::create(DDS_LENGTH);
    for(int i=0; i<DDS_LENGTH; ++i)
      sine[i] = sin(2*M_PI*i/(DDS_LENGTH-1));
    for(int i=0; i<TONES; i++){
      acc[i] = 0.0;
      gain[i] = 0.0;
      gain[i].lambda = 0.8;
    }
  }
  ~SineStormPatch(){
    FloatArray::destroy(sine);
  }
  float getWave(float phase){
    uint32_t index = phase*(DDS_LENGTH-1);
    index = min(index, DDS_LENGTH-1);
    return sine[index];
  }
  void processAudio(AudioBuffer& buf){
    float freq = getParameterValue(PARAMETER_A)*8.0-4.0;
    float fraction = getParameterValue(PARAMETER_B)*(TONES-1)+1.0;
    float spread = getParameterValue(PARAMETER_C)*2.0+1.0;
    float amp = getParameterValue(PARAMETER_D)*2.0;
    float portamento = getParameterValue(PARAMETER_E)*0.19+0.8;
    int tones = (int)fraction;
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(freq);
    inc[0].lambda = portamento;
    inc[0] = hz.getFrequency(left[0])*mul;
    gain[0] = amp/tones;
    for(int t=1; t<tones; ++t){
      inc[t].lambda = portamento;
      inc[t] = inc[t-1]*spread;
      if(inc[t] >= 0.4)
	gain[t] = 0.0f;
      else if(t == tones-1)
	gain[t] = ((float)fraction-tones)*gain[0];
      else
	gain[t] = gain[0];
    }
    for(int i=0; i<buf.getSize(); ++i){
      left[i] = 0;
      for(int t=0; t<tones; ++t){
	left[i] += gain[t] * getWave(acc[t]);
        acc[t] += inc[t];
	if(acc[t] > 1.0)
	  acc[t] -= 1.0;
      }
    }
    right.copyFrom(left);
  }
};
