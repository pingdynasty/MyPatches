#include "StompBox.h"
#include "VoltsPerOctave.h"
#include "SmoothValue.h"

#define P1      1.0
#define m2      (P1+1.0/12)
#define M2      (P1+2.0/12)
#define m3      (P1+3.0/12)
#define M3      (P1+4.0/12)
#define P4      (P1+5.0/12)
#define d5      (P1+6.0/12)
#define A4      (P1+6.0/12)
#define P5      (P1+7.0/12)
#define m6      (P1+8.0/12)
#define M6      (P1+9.0/12)
#define m7      (P1+10.0/12)
#define M7      (P1+11.0/12)
#define P8      (P1+12.0/12)
#define SEMI    (1.0/12)
#define WHOLE   (2.0/12)
#define HEPTATONIC_MODES 14
// WHOLE, WHOLE, SEMI, WHOLE, WHOLE, WHOLE, SEMI     // Major scale
// WHOLE, SEMI, WHOLE, WHOLE, SEMI, WHOLE+SEMI, SEMI // Harmonic Minor scale
static const float HEPTATONIC_TONES[2][HEPTATONIC_MODES] = {
  // heptatonic scales
  {M2, m3, P4, P5, m6, M7, P8, // Harmonic Minor scale
   P1+M2, P1+m3, P1+P4, P1+P5, P1+m6, P1+M7, P1+P8},
  {M2, M3, P4, P5, M6, M7, P8, // Major scale
   P1+M2, P1+M3, P1+P4, P1+P5, P1+M6, P1+M7, P1+P8}
};
// The major and natural minor scales share the same set of notes, but start in a different place.
static const unsigned int HEPTATONIC_SEQUENCE[8] = {
  0, 2, 4, 6, 3, 5, 1, 7
};

class HeptatonicPatch : public Patch {
private:
  const static int DDS_LENGTH = 1<<14;
  const static int TONES = 8;
  float acc[TONES];
  SmoothFloat inc[TONES];
  SmoothFloat gain[TONES];
  FloatArray sine;
  VoltsPerOctave hz;
  const float mul;
public:
  HeptatonicPatch() :
    mul(1.0/getSampleRate()){
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Tones");
    registerParameter(PARAMETER_C, "Mode");
    registerParameter(PARAMETER_D, "Amplitude");
    registerParameter(PARAMETER_E, "Portamento");
    sine = FloatArray::create(DDS_LENGTH);
    for(int i=0; i<DDS_LENGTH; ++i)
      sine[i] = sin(2*M_PI*i/(DDS_LENGTH-1));
    for(int i=0; i<TONES; i++){
      acc[i] = 0.0;
      gain[i].lambda = 0.8+i*0.02;
    }
  }
  ~HeptatonicPatch(){
    FloatArray::destroy(sine);
  }
  float getWave(float phase){
    uint32_t index = phase*(DDS_LENGTH-1);
    index = min(index, DDS_LENGTH-1);
    return sine[index];
  }
  void processAudio(AudioBuffer& buf){
    float pitch = getParameterValue(PARAMETER_A)*8.0-4.0;
    float fraction = getParameterValue(PARAMETER_B)*TONES+1.0;
    int mode = getParameterValue(PARAMETER_C)*HEPTATONIC_MODES;
    int scale = 0; // minor
    if(mode > 6){
      scale = 1;
      mode = 13-mode;
    }
    float amp = getParameterValue(PARAMETER_D);
    float portamento = getParameterValue(PARAMETER_E)*0.19+0.8;
    int tones = (int)fraction;
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(pitch);
    inc[0].lambda = portamento;
    float basetone = hz.getFrequency(left[0])*mul;
    if(mode > 0)
      basetone *= HEPTATONIC_TONES[scale][mode-1];
    inc[0] = basetone;
    gain[0] = amp/tones + hz.sampleToVolts(right[0]);
    debugMessage("mode/tones", mode, tones);
    for(int t=1; t<tones; ++t){
      float tone = HEPTATONIC_TONES[scale][HEPTATONIC_SEQUENCE[t]+mode];
      inc[t].lambda = portamento;
      inc[t] = inc[0]*tone;
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
