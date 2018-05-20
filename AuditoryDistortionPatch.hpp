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
  static const int TONES = 11;
  float scale[TONES];
  SineOscillator sine[TONES];
  VoltsPerOctave hz;
public:
  AuditoryDistortionPatch() {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Fundamental");
    registerParameter(PARAMETER_C, "Tones");
    registerParameter(PARAMETER_D, "Amplitude");
    for(int i=0; i<TONES; i++){
      sine[i].setSampleRate(getSampleRate());
      scale[i] = 14+i; // start from 15th harmonic
    }
  }
  ~AuditoryDistortionPatch(){
  }
  void processAudio(AudioBuffer& buf){
    float freq = getParameterValue(PARAMETER_A)*6.0-3.0;
    float fundamental = getParameterValue(PARAMETER_B)*2;
    float fraction = getParameterValue(PARAMETER_B)*(TONES-1)+1;
    int tones = (int)fraction;
    fraction -= tones;
    // int tones = (int)(getParameterValue(PARAMETER_C)*TONES+1);
    float amp = getParameterValue(PARAMETER_D)*4.0/TONES;
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(freq);
    float f0 = hz.getFrequency(left[0]);
    sine[0].setFrequency(f0);
    sine[0].getSamples(left);
    left.multiply(fundamental);
    for(int i=1; i<=tones; ++i){
      sine[i].setFrequency(f0*scale[i]);
      sine[i].getSamples(right);
      if(i == tones)
	right.multiply(fraction);
      left.add(right);
    }
    left.multiply(amp);
    right.copyFrom(left);
  }
};
