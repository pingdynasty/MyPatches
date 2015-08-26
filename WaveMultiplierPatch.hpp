/**
 * Dual wave multiplier: maps the incoming signal
 */
#define WAVEMULTIPLIER_WAVETABLE_SIZE 1024
#define WAVEMULTIPLIER_MAX_MULT       31

class WaveMultiplierPatch : public Patch {
private:
  float wave[WAVEMULTIPLIER_WAVETABLE_SIZE];
public:
  WaveMultiplierPatch() {
    registerParameter(PARAMETER_A, "Multiply L");
    registerParameter(PARAMETER_B, "Multiply R");
    registerParameter(PARAMETER_C, "Gain L");
    registerParameter(PARAMETER_D, "Gain R");
    for(int i=0; i<WAVEMULTIPLIER_WAVETABLE_SIZE; ++i)
      // wave[i] = 2.0f*i/WAVEMULTIPLIER_WAVETABLE_SIZE - 1.0f; // tri
      wave[i] = sinf(2.0f*M_PI*i/WAVEMULTIPLIER_WAVETABLE_SIZE); // sine
  }

  void processAudio(AudioBuffer& buffer){
    float mull = getParameterValue(PARAMETER_A)*WAVEMULTIPLIER_MAX_MULT+0.5;
    float mulr = getParameterValue(PARAMETER_B)*WAVEMULTIPLIER_MAX_MULT+0.5;
    float gainl = getParameterValue(PARAMETER_C);
    float gainr = getParameterValue(PARAMETER_D);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    for(int i=0; i<buffer.getSize(); ++i){
      int index = (uint16_t)((left[i]+1.0f)*mull*WAVEMULTIPLIER_WAVETABLE_SIZE);
      left[i] = wave[index & 0x3ff]*gainl;
      index = (uint16_t)((right[i]+1.0f)*mulr*WAVEMULTIPLIER_WAVETABLE_SIZE);
      right[i] = wave[index & 0x3ff]*gainr;
    }
  }
};
