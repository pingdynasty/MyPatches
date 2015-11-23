#include "StompBox.h"
#include <stdlib.h>
class ShepardTonesPatch : public Patch {
private:
  const static int DDS_LENGTH = 4096;
  const static int WIN_LENGTH = 4096;
  const static int TONES = 12;
  const float NYQUIST;
  float acc[TONES];
  float inc;
  Window amp;
  FloatArray sine;
public:
  ShepardTonesPatch() : NYQUIST(getSampleRate()/2.0) {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Range");
    registerParameter(PARAMETER_C, "Spread");
    registerParameter(PARAMETER_D, "Rate");
    sine = FloatArray::create(DDS_LENGTH);
    for(int i=0; i<DDS_LENGTH; ++i)
      sine[i] = sin(2*M_PI*i/(DDS_LENGTH-1)); // + drand48()/(DDS_LENGTH*2);
      // todo: DDS class. add 1/2 LSB dither / noise
    // amp = Window::create(Window::HannWindow, WIN_LENGTH);
    amp = Window::create(Window::TriangularWindow, WIN_LENGTH);
    amp.multiply(1.0/TONES);
    for(int i=0; i<TONES; i++)
      acc[i] = 0.0;
    inc = 0.0;
  }
  ~ShepardTonesPatch(){
    FloatArray::destroy(sine);
    Window::destroy(amp);
  }
  float getAmplitude(float inc){
    inc = max(0.0, min(1.0, inc));
    return amp[inc*(WIN_LENGTH-1)];
  }
  /*
  uint32_t freqToInc(float hz){
    //    return (uint32_t)(hz * NYQUIST)/ ((DDS_LENGTH << TRUNCATE)-1);
    return (uint32_t)((hz / NYQUIST) * DDS_PERIOD);
    // f = inc * fs / (DDS_LENGTH << TRUNCATE)
    // todo: interpolating lookup table to convert semitones 0-128 to hz (or direct to phase increments)

      // FOUT = (M (REFCLK)) /2^N
  }
  */
  float getWave(float phase){
    phase = max(0.0, min(1.0, phase));
    return sine[phase*(DDS_LENGTH-1)];
  }
  /*
  float getWave(uint32_t phase){
    phase &= DDS_PERIOD;
    phase >>= TRUNCATE;
    ASSERT(phase < DDS_LENGTH, "invalid phase");
    // phase = max(0, min(DDS_LENGTH-1, phase));
    return sine[phase];
    //    return sine[phase >> TRUNCATE];
  }
  */
  void processAudio(AudioBuffer& buf){
    float minf = getParameterValue(PARAMETER_A)*0.2+0.01;
    float maxf = min(0.4, minf + getParameterValue(PARAMETER_B)*0.2);
    float spread = getParameterValue(PARAMETER_C);
    //    float mul = 1.0 + (spread*(maxf-minf));
    float mul = 1.0 + 2*spread/TONES;
    float rate = 1.0 + (getParameterValue(PARAMETER_D) - 0.5) * 0.00001;
    int size = buf.getSize();
    FloatArray out = buf.getSamples(LEFT_CHANNEL);
    for(int i=0; i<size; ++i){
      float tinc = inc;
      float amp = getAmplitude((tinc-minf)/(maxf-minf));
      out[i] = amp * getWave(acc[0]);
      //      debugMessage("tinc/inc", tinc, (tinc-minf)/(maxf-minf));
      acc[0] += tinc;
      if(acc[0] > 1.0)
	acc[0] -= 1.0;
      for(int t=1; t<TONES; ++t){
        tinc *= mul;
        if(tinc > maxf)
	  tinc = minf;
	  // tinc = tinc - maxf + minf;
	//	  tinc = minf*mul;
	amp = getAmplitude((tinc-minf)/(maxf-minf));
	out[i] += amp * getWave(acc[t]);
        acc[t] += tinc;
	if(acc[t] > 1.0)
	  acc[t] -= 1.0;
      }
      inc *= rate;
      /*
      if(inc > maxf)
        inc = inc - maxf + minf;
      if(inc < minf)
        inc = inc - minf + maxf;
      */
      if(inc > maxf)
        inc = minf;
      if(inc < minf)
        inc = maxf;
    }
  }
};
