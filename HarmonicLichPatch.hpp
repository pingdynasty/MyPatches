#include "StompBox.h"

#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "SmoothValue.h"
#include "SineOscillator.h"
#include "PolyBlepOscillator.h"

// Up to 16 harmonics supported
#define TONES 9
static const char* names[] = { "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8",
			       "H9", "H10", "H11", "H12", "H13", "H14", "H15", "H16" };

class HarmonicLichPatch : public Patch {
private:
  Oscillator* osc[TONES];
  float levels[TONES];
  FloatArray mix;
  FloatArray ramp;
  VoltsPerOctave hz;
public:
  HarmonicLichPatch(){
    registerParameter(PARAMETER_A, "Fine Tune");
    registerParameter(PARAMETER_B, "Semitone");
    registerParameter(PARAMETER_C, "Centre");
    registerParameter(PARAMETER_D, "Peak");
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
      levels[i] = 0.5;
    }
    mix = FloatArray::create(getBlockSize());
    ramp = FloatArray::create(getBlockSize());
  }

  ~HarmonicLichPatch(){
    for(int i=0; i<TONES; i++)
      SineOscillator::destroy((SineOscillator*)osc[i]);
    FloatArray::destroy(mix);
    FloatArray::destroy(ramp);
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
    float centre = getParameterValue(PARAMETER_C)*(TONES-1);
    float a, r;
    float d = getParameterValue(PARAMETER_D);
    if(d < 0.25){
      /* --.-- */
      a = 0.0f;
      r = d*4;
      /* --.\\ */
    }else if(d < 0.50){
      a = (d-0.25)*4;
      r = 1.0f;
      /* //.\\ */
    }else if(d < 0.75){
      a = 1.0f;
      r = 1.0f - (d-0.50)*4;
      /* //.-- */
    }else{
      a = 1.0f - (d-0.75)*4;
      r = 0.0f;
      /* --.-- */
    }
    float fm = getParameterValue(PARAMETER_E)*0.2;
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(freq);
    // freq = hz.getFrequency(0);
    // float fundamental = freq;
    float fundamental = hz.getFrequency(left[0]);
    right.multiply(fm);
    float gainadjust = 0;
    left.clear();
    for(int i=0; i<TONES; i++){
      float newlevel = getParameterValue(PatchParameterId(PARAMETER_AA+i));
      // gainadjust += (newlevel+levels[i])/2;
      float distance = abs(centre - i);
      float duck = i < centre ? a*distance : r*distance;
      newlevel = max(0.0f, min(1.0f, newlevel*(1-duck)));
      ramp.ramp(levels[i], newlevel);
      levels[i] = newlevel;
      gainadjust += newlevel;
      osc[i]->setFrequency(fundamental*(i+1));
      osc[i]->getSamples(mix, right);
      mix.multiply(ramp);
      left.add(mix);
    }
    if(gainadjust > 1.0f)
      left.multiply(1.0f/gainadjust);
    setParameterValue(PARAMETER_F, gainadjust/TONES);
    // left.multiply(1.0f/TONES);
    for(int i=0; i<buf.getSize(); i++)
      left[i] = tanh(left[i]);
    //   left[i] = nonlinear(left[i]);

    /*
      angle := 0 (flat) to 1
      distance := 0 to TONES
      reduction := angle*distance
      0 angle: no reduction, 0 distance, no reduction
      multiplier = 1-reduction
     */
    
    right.copyFrom(left);
  }

  float nonlinear(float x){ // tanh approx
    // tanh approx
    if (x<-3)
        return -1;
    else if (x>3)
      return 1;
    else
      return x * ( 27 + x*x ) / ( 27 + 9*x*x );
  }

};
