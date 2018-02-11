#include "StompBox.h"

#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "SmoothValue.h"
#include "Oscillators.hpp"

/*
 * Oscillator that generates a cluster of sine waves
 */
class AuditoryDistortionPatch : public Patch {
private:
  static const int TONES = 10;
  float scale[TONES];
  SineOscillator sine[TONES];
  VoltsPerOctave hz;
public:
  AuditoryDistortionPatch() {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Tones");
    registerParameter(PARAMETER_C, "Spread");
    registerParameter(PARAMETER_D, "Amplitude");
    registerParameter(PARAMETER_E, "Portamento");
    for(int i=0; i<TONES; i++){
      sine[i].setSampleRate(getSampleRate());
      scale[i] = 15+i; // start from 15th harmonic
    }
  }
  ~AuditoryDistortionPatch(){
  }
  void processAudio(AudioBuffer& buf){
    float freq = getParameterValue(PARAMETER_A)*8.0-4.0;
    // float fraction = getParameterValue(PARAMETER_B)*(TONES-1)+1.0;
    // float spread = getParameterValue(PARAMETER_C)*2.0+1.0;
    float amp = getParameterValue(PARAMETER_D)*4.0/TONES;
    // float portamento = getParameterValue(PARAMETER_E)*0.19+0.8;
    // int tones = (int)fraction;
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(freq);
    float fundamental = hz.getFrequency(left[0]);
    left.clear();
    for(int i=0; i<TONES; ++i){
      sine[i].setFrequency(fundamental*scale[i]);
      sine[i].getSamples(right);
      left.add(right);
    }
    left.multiply(amp);
    right.copyFrom(left);
  }
};
