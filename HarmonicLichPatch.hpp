#include "StompBox.h"

#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "SmoothValue.h"
#include "SineOscillator.h"
#include "PolyBlepOscillator.h"

#define TONES 8
static const char* names[] = { "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9" };

class HarmonicLichPatch : public Patch {
private:
  Oscillator* osc[TONES];
  SmoothFloat levels[TONES];
  FloatArray mix;
  VoltsPerOctave hz;
public:
  HarmonicLichPatch(){
    registerParameter(PARAMETER_A, "FineTune");
    registerParameter(PARAMETER_B, "Semitone");
    registerParameter(PARAMETER_C, "Detune");
    registerParameter(PARAMETER_D, "Amplitude");
    registerParameter(PARAMETER_E, "FM");
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.0);
    setParameterValue(PARAMETER_D, 0.25);
    setParameterValue(PARAMETER_E, 0.5);
    for(int i=0; i<TONES; i++){
      osc[i] = SineOscillator::create(getSampleRate());
      registerParameter(PatchParameterId(PARAMETER_AA+i), names[i]);
      setParameterValue(PatchParameterId(PARAMETER_AA+i), 0.25);
      levels[i].reset(0.5);
    }
    mix = FloatArray::create(getBlockSize());
  }

  ~HarmonicLichPatch(){
    for(int i=0; i<TONES; i++)
      SineOscillator::destroy((SineOscillator*)osc[i]);
    FloatArray::destroy(mix);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    float level = value ? 0.0 : 1.0;
    switch(bid){
    case BUTTON_A:
      for(int i=0; i<TONES; i+= 2)
	setParameterValue(PatchParameterId(PARAMETER_AA+i), level);
      break;
    case BUTTON_B:
      for(int i=1; i<TONES; i+= 2)
	setParameterValue(PatchParameterId(PARAMETER_AA+i), level);
      break;
    }
  }

  void processAudio(AudioBuffer& buf){
    float freq = getParameterValue(PARAMETER_A)/6.0f;
    freq += round(getParameterValue(PARAMETER_B)*32-24)/12;
    float detune = getParameterValue(PARAMETER_C)/12.0f - 1/6.0f;
    float amp = getParameterValue(PARAMETER_D)/TONES;
    float fm = getParameterValue(PARAMETER_E)*0.2;
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(freq);
    // freq = hz.getFrequency(0);
    // float fundamental = freq;
    float fundamental = hz.getFrequency(left[0]);
    right.multiply(fm);
    float gain;
    left.clear();
    for(int i=0; i<TONES; i++){
      levels[i] = getParameterValue(PatchParameterId(PARAMETER_AA+i));
      gain = levels[i];
      osc[i]->setFrequency(fundamental*(i+1) + fundamental*detune);
      osc[i]->getSamples(mix, right);
      mix.multiply(gain);
      left.add(mix);
    }
    left.multiply(amp);
    right.copyFrom(left);
  }
};
