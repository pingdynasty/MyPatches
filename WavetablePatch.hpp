/*
* Two-dimensional wavetable navigable by x and y position, implemented with interpolated DDS.
* Left in: 1v/oct cv and exp fm
* Right in: linear fm
*/
#define WAVETABLE_SIZE 1024
class WavetablePatch : public Patch {
private:
  uint32_t acc = 0;
  float waves[3][3][WAVETABLE_SIZE];
  const float M_SQRT1_2;
  const float fs;

int32_t calculateIncrement(float exp, float lin){
  float freq = 440*powf(2, exp)+lin*200;
  int32_t inc = (int32_t)(freq*WAVETABLE_SIZE*(1<<22)/fs);
  //  inc <<= 22;
  return inc;
}
public:
  WavetablePatch() : M_SQRT1_2(1.0f/sqrtf(2.0)), fs(getSampleRate()) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "X");
    registerParameter(PARAMETER_C, "Y");
    registerParameter(PARAMETER_D, "Gain");
    for(int y=0; y<3; ++y)
      for(int i=0; i<WAVETABLE_SIZE; ++i)
	waves[0][y][i] = i/WAVETABLE_SIZE;
    for(int y=0; y<3; ++y)
      for(int i=0; i<WAVETABLE_SIZE; ++i)
	waves[1][y][i] = sinf(2.0*M_PI*i/(float)WAVETABLE_SIZE);
    for(int y=0; y<3; ++y)
      for(int i=0; i<WAVETABLE_SIZE; ++i)
	waves[2][y][i] = (WAVETABLE_SIZE-i)/WAVETABLE_SIZE;
  }

  void processAudio(AudioBuffer& buffer){
    float tune = getParameterValue(PARAMETER_A);
    float x = getParameterValue(PARAMETER_B)*3;
    float y = getParameterValue(PARAMETER_C)*3;
    float gain = getParameterValue(PARAMETER_D);
    int xLow = floor(x);
    int xHigh = ceil(x);
    int yLow = floor(y);
    int yHigh = ceil(y);
    // calculate distance multipliers
    float x0y0 = 1.0 - sqrtf((x-xLow)*(x-xLow) + (y-yLow)*(y-yLow))*M_SQRT1_2;
    float x0y1 = 1.0 - sqrtf((x-xLow)*(x-xLow) + (yHigh-y)*(yHigh-y))*M_SQRT1_2;
    float x1y0 = 1.0 - sqrtf((xHigh-x)*(xHigh-x) + (y-yLow)*(y-yLow))*M_SQRT1_2;
    float x1y1 = 1.0 - sqrtf((xHigh-x)*(xHigh-x) + (yHigh-y)*(yHigh-y))*M_SQRT1_2;
    float* cvExp = buffer.getSamples(0);
    float* cvLin = buffer.getSamples(1);
    float* out = buffer.getSamples(0);
    for(int i=0; i<buffer.getSize(); ++i){
      int inc = calculateIncrement(cvExp[i]+tune, cvLin[i]);
      int idx = acc >> 22;
      out[i] = x0y0*waves[xLow][yLow][idx] + x0y1*waves[xLow][yHigh][idx] +
	x1y0*waves[xHigh][yLow][idx] + x1y1*waves[xHigh][yHigh][idx];
      out[i] *= gain;
      acc += inc;
    }
  }
};
