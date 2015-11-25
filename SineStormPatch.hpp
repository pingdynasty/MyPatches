#include "StompBox.h"

/*
 */
class SineStormPatch : public Patch {
private:
  const static int DDS_LENGTH = 1<<14;
  const static int TONES = 18;
  float acc[TONES];
  float inc[TONES];
  float gain[TONES];
  FloatArray sine;
  const float VOLTAGE_MULTIPLIER = -4.40f;
  const float VOLTAGE_OFFSET = -0.0585f;
  const float mul;
  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2hz(float v){
    return 440.f * powf(2, v);
  }
public:
  SineStormPatch():mul(1.0/getSampleRate()){
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Tones");
    registerParameter(PARAMETER_C, "Spread");
    registerParameter(PARAMETER_D, "Gain");
    sine = FloatArray::create(DDS_LENGTH);
    for(int i=0; i<DDS_LENGTH; ++i)
      sine[i] = sin(2*M_PI*i/(DDS_LENGTH-1));
    for(int i=0; i<TONES; i++)
      acc[i] = 0.0;
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
    float freq = getParameterValue(PARAMETER_A)*6.0 - 2.0;
    float fraction = getParameterValue(PARAMETER_B)*(TONES-1)+1;
    float spread = getParameterValue(PARAMETER_C) + 1.0;
    float amp = getParameterValue(PARAMETER_D)*2.0 / fraction;
    int tones = (int)fraction;
    fraction -= tones;
    FloatArray out = buf.getSamples(LEFT_CHANNEL);
    inc[0] = volts2hz(sample2volts(out[0]) + freq) * mul;
    gain[0] = amp;
    for(int t=1; t<tones; ++t){
      inc[t] = inc[t-1]*spread;
      if(inc[t] >= 0.4)
	gain[t] = 0.0f;
      else if(t == tones-1)
	gain[t] = fraction*amp;
      else
	gain[t] = amp;
    }
    for(int i=0; i<buf.getSize(); ++i){
      out[i] = 0;
      for(int t=0; t<tones; ++t){
	out[i] += gain[t] * getWave(acc[t]);
        acc[t] += inc[t];
	if(acc[t] > 1.0)
	  acc[t] -= 1.0;
      }
    }
  }
};
