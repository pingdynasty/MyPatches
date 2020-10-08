#include "StompBox.h"

#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "SmoothValue.h"
#include "SineOscillator.h"

// Up to 16 harmonics supported
#define TONES 8
static const char* names[] = { "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8",
			       "H9", "H10", "H11", "H12", "H13", "H14", "H15", "H16" };

class HarmonicLichPatch : public Patch {
private:
  Oscillator* osc[TONES];
  float levels[TONES];
  FloatArray mix;
  FloatArray ramp;
  VoltsPerOctave hz;
  float gainadjust = 0.0f;
  StiffFloat semitone;
public:
  HarmonicLichPatch() : hz(-0.0022, -10.75){
    registerParameter(PARAMETER_A, "Semitone");
    registerParameter(PARAMETER_B, "Fine Tune");
    registerParameter(PARAMETER_C, "Centre");
    registerParameter(PARAMETER_D, "Peak");
    // registerParameter(PARAMETER_E, "FM Amount");
    // registerParameter(PARAMETER_F, "Duck>");
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.0);
    setParameterValue(PARAMETER_D, 0.25);
    setParameterValue(PARAMETER_E, 0.5);
    for(int i=0; i<TONES; i++){
      osc[i] = SineOscillator::create(getSampleRate());
      registerParameter(PatchParameterId(PARAMETER_AA+i), names[i]);
      setParameterValue(PatchParameterId(PARAMETER_AA+i), 0.25);
      levels[i] = 1;
    }
    mix = FloatArray::create(getBlockSize());
    ramp = FloatArray::create(getBlockSize());
    semitone.delta = 0.5;
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
  void processMidi(MidiMessage msg){
    if(msg.isControlChange()){
      // debugMessage("cc", msg.getPort(), msg.getControllerNumber(), msg.getControllerValue());
      uint8_t id = msg.getControllerNumber() - PATCH_PARAMETER_AA;
      if(id < TONES)
	setParameterValue(PatchParameterId(PARAMETER_AA+id), msg.getControllerValue()/127.0f);
      else if(msg.getControllerNumber() == PATCH_BUTTON)
	buttonChanged(BUTTON_A, msg.getControllerValue(), 0);
    }
  }

  void processAudio(AudioBuffer& buf){
    semitone = getParameterValue(PARAMETER_A)*32-24;
    float freq = round(semitone)/12 + getParameterValue(PARAMETER_B)/6;
    float centre = getParameterValue(PARAMETER_C)*(TONES-1);
    float a, r;
    float d = getParameterValue(PARAMETER_D);
    if(d < 0.25){       /* //.\\ */
      a = 1-d*4;
      r = 1;
    }else if(d < 0.50){ /* --.\\ */
      a = 0;
      r = 1-(d-0.25)*4;
    }else if(d < 0.75){ /* --.-- */
      a = (d-0.50)*4;
      r = 0;
    }else{              /* //.-- */      
      a = 1;
      r = (d-0.75)*4;      
    }                   /* //.\\ */
    // float fm = getParameterValue(PARAMETER_E)*0.2;
    FloatArray left = buf.getSamples(LEFT_CHANNEL);
    FloatArray right = buf.getSamples(RIGHT_CHANNEL);
    hz.setTune(freq);
    float fundamental = hz.getFrequency(left[0]);
    // right.multiply(fm);
    float newgainadjust = 0;
    left.clear();
    for(int i=0; i<TONES; i++){
      float newlevel = getParameterValue(PatchParameterId(PARAMETER_AA+i));
      float distance = abs(centre - i);
      float duck = i < centre ? a*distance : r*distance;
      newlevel = max(0, min(1, newlevel*(1-duck)));
      ramp.ramp(levels[i], newlevel);
      levels[i] = newlevel;
      newgainadjust += newlevel;
      osc[i]->setFrequency(fundamental*(i+1));
      // osc[i]->getSamples(mix, right);
      osc[i]->getSamples(mix);
      mix.multiply(ramp);
      left.add(mix);
    }
    newgainadjust = newgainadjust > 1 ? 1/newgainadjust : 1;
    ramp.ramp(gainadjust, newgainadjust);
    left.multiply(ramp);
    gainadjust = newgainadjust;
    // setParameterValue(PARAMETER_F, gainadjust);
    // for(int i=0; i<buf.getSize(); i++)
    //   left[i] = tanh(left[i]);    
    right.copyFrom(left);
  }

};
