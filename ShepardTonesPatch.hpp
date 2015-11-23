// https://github.com/laurent-latorre/Synthol/blob/master/src/pitch.h
// midi note frequencies
#include "StompBox.h"
#include <stdlib.h>
class ShepardTonesPatch : public Patch {
private:
  const static int DDS_LENGTH = 4096;
  const static int WIN_LENGTH = 4096;
  const static int TONES = 6;
  const static int TRUNCATE = 12;
  const static uint32_t DDS_PERIOD = (DDS_LENGTH<<TRUNCATE)-1;
  const float NYQUIST;
  uint32_t acc[TONES];
  int32_t inc;
  Window amp;
  FloatArray freq;
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
    //    amp = Window::create(Window::HannWindow, WIN_LENGTH);
    amp = Window::create(Window::TriangularWindow, WIN_LENGTH);
    amp.multiply(2.0/TONES);
    freq = FloatArray::create(WIN_LENGTH);
    for(int i=0; i<WIN_LENGTH; i++)
      freq[i] = 440.f * powf(2, float(i)/WIN_LENGTH);
    //      freq[i] = 440.f * powf(2, 14.0f*i/WIN_LENGTH);
    //    win.multiply(1.0/(TONES-2));
  }
  float getFrequency(int inc, int mininc, int maxinc){
    uint32_t index = (inc-mininc)*(WIN_LENGTH-1)/(maxinc-mininc);
    return freq[index];
  }
  float getAmplitude(int inc, int mininc, int maxinc){
    float ratio = max(0.0, min(1.0, float(inc-mininc)/(maxinc-mininc)));
    int32_t index = ratio*(WIN_LENGTH-1);
    //    debugMessage("ratio, index", ratio, index);
    //    int32_t index = (inc-mininc)*(WIN_LENGTH-1)/(maxinc-mininc);
    //    index >>= TRUNCATE;
    //    ASSERT(index < WIN_LENGTH, "invalid window index");
    //    ASSERT(index > 0, "negative window index");
    //    debugMessage("inc-minc, index, maxinc", (int)(inc-mininc), index, maxinc);
    index = max(0, min(WIN_LENGTH-1, index));
    return amp[index];
  }
  uint32_t freqToInc(float hz){
    //    return (uint32_t)(hz * NYQUIST)/ ((DDS_LENGTH << TRUNCATE)-1);
    return (uint32_t)((hz / NYQUIST) * DDS_PERIOD);
    // f = inc * fs / (DDS_LENGTH << TRUNCATE)
    // todo: interpolating lookup table to convert semitones 0-128 to hz (or direct to phase increments)

      // FOUT = (M (REFCLK)) /2^N
  }
  float getWave(uint32_t phase){
    phase &= DDS_PERIOD;
    phase >>= TRUNCATE;
    ASSERT(phase < DDS_LENGTH, "invalid phase");
    // phase = max(0, min(DDS_LENGTH-1, phase));
    return sine[phase];
    //    return sine[phase >> TRUNCATE];
  }
  void processAudio(AudioBuffer& buf){
    uint32_t mininc = freqToInc(getParameterValue(PARAMETER_A)*2000+100);
    uint32_t maxinc = mininc + freqToInc(getParameterValue(PARAMETER_B)*8000);
    float spread = getParameterValue(PARAMETER_C);//*4800/1200.0f; // cents
    // int32_t rate = (getParameterValue(PARAMETER_D) - 0.5) * freqToInc(0.8);
    float rate = 1.0 + (getParameterValue(PARAMETER_D) - 0.5) * 0.001;
    //    float mul = 1.0 + log2f(spread*maxinc) - log2f(mininc);
    float mul = 1.0 + spread*TONES;
    int size = buf.getSize();
    FloatArray out = buf.getSamples(LEFT_CHANNEL);
    // debugMessage("rate, spread, mul", (float)rate, spread, mul);
    // debugMessage("acc, inc", acc[0]>>TRUNCATE, inc>>TRUNCATE);
    if(inc > maxinc)
      inc = mininc;
    if(inc < mininc)
      inc = maxinc;
    for(int i=0; i<size; ++i){
      int tinc = inc;
      out[i] = getAmplitude(tinc, mininc, maxinc) * getWave(acc[0]);
      // out[i] = 0.4 * getWave(acc[0]);
      acc[0] += tinc;
      for(int t=1; t<TONES; ++t){
        // tinc += freqToInc(440*powf(2, spread*i));
        //tinc += freqToInc(getFrequency(spread*i, mininc, maxinc));
        tinc *= mul;
        if(tinc > maxinc)
	  tinc = (tinc - maxinc) + mininc;
	//          tinc = mininc;
	else
	out[i] += getAmplitude(tinc, mininc, maxinc) * getWave(acc[t]);
	//        out[i] += 0.4 * getWave(acc[t]);
        acc[t] += tinc;
      }
      inc *= rate;
      if(inc > maxinc)
        inc = mininc;
      if(inc < mininc)
        inc = maxinc;
    }
  }
};
