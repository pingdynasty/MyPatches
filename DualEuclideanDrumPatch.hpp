#ifndef __DualEuclideanDrumPatch_hpp__
#define __DualEuclideanDrumPatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"
#include "Cymbal.hpp"
#include "BassDrum.hpp"
#include "Sequence.h"

class DrumTrigger {
private:
  VoltsPerOctave volts;
  Drum* drum;
  bool state;
  const float threshold = 0.1f;
public:
  DrumTrigger(Drum* drm) : 
    drum(drm), state(false){}
  void update(float value){
    if(value > threshold){
      if(!state){
	state = true;
	drum->trigger();
      }
    }else{
      state = false;
    }
  }
  void getSamples(FloatArray samples){
    // for(int i=0; i<samples.getSize(); ++i){
    //   if(volts.sampleToVolts(samples[i]) > threshold){
    // 	if(!state){
    // 	  state = true;
    // 	  drum->trigger();
    // 	}
    //   }else{
    // 	state = false;
    //   }
    //   samples[i] = drum->getNextSample();
    // }
    for(int i=0; i<samples.getSize(); ++i)
      samples[i] = drum->getNextSample();
  }
};

class DualEuclideanDrumPatch : public Patch {
private:
  CymbalVoice* hat;
  BassDrumVoice* kick;
  DrumTrigger* drum[2];
  bool triggered[2];
  Sequence<uint16_t> seq[2];
public:
  DualEuclideanDrumPatch(){
    triggered[0] = triggered[1] = false;

    // registerParameter(PARAMETER_A, "Hat Tone");
    // registerParameter(PARAMETER_B, "Hat Decay");
    // registerParameter(PARAMETER_AA, "Envelope");

    // registerParameter(PARAMETER_C, "Hat Beats");
    // registerParameter(PARAMETER_D, "Hat Steps");
    // registerParameter(PARAMETER_AC, "Hat Ramp");
    // registerParameter(PARAMETER_AD, "Hat Trigger");
    
    // registerParameter(PARAMETER_E, "Kick Tone");
    // registerParameter(PARAMETER_F, "Kick Decay");
    // registerParameter(PARAMETER_AE, "Envelope");

    // registerParameter(PARAMETER_G, "Kick Beats");
    // registerParameter(PARAMETER_H, "Kick Steps");
    // registerParameter(PARAMETER_AG, "Kick Ramp");
    // registerParameter(PARAMETER_AH, "Kick Trigger");

    // registerParameter(PARAMETER_BA, "Accent");
    // registerParameter(PARAMETER_BB, "Kick Snap");
    hat = new CymbalVoice(getSampleRate());
    kick = new BassDrumVoice(getSampleRate());
    kick->setSnap(0.2);
  }
  ~DualEuclideanDrumPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float tone = 120*exp2f(getParameterValue(PARAMETER_A)*4);
    float decay = getParameterValue(PARAMETER_B);
    float accent = 0.5;//getParameterValue(PARAMETER_BA);
    hat->setFrequency(tone);
    hat->setFilter(getParameterValue(PARAMETER_A)*0.3 + 0.5);
    hat->setDecay(decay);
    hat->setAccent(accent);
    tone = 20*exp2f(getParameterValue(PARAMETER_C)*4);
    decay = getParameterValue(PARAMETER_D);
    kick->setFrequency(tone);
    kick->setDecay(decay);
    kick->setAccent(accent);
    // kick->setSnap(getParameterValue(PARAMETER_BB));
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    // drum[0]->update(getParameterValue(PARAMETER_AD));
    // drum[1]->update(getParameterValue(PARAMETER_AH));
    drum[0]->getSamples(left);
    drum[1]->getSamples(right);
    right.multiply(0.8);

    // int steps;
    // int fills;
    // steps = getParameterValue(PARAMETER_C)*16;
    // fills = getParameterValue(PARAMETER_D)*steps+1;
    // seq[0].calculate(steps, fills);    
    // steps = getParameterValue(PARAMETER_G)*16;
    // fills = getParameterValue(PARAMETER_H)*steps+1;
    // seq[1].calculate(steps, fills);

    // static float lfo1 = 0;
    // if(lfo1 > 1.0){
    //   lfo1 = 0;
    //   if(seq[0].next())
    // 	setParameterValue(PARAMETER_AD, 1.0);
    //   else
    // 	setParameterValue(PARAMETER_AD, 0.0);
    // }
    // lfo1 += 2 * getBlockSize() / getSampleRate();
    // setParameterValue(PARAMETER_AC, lfo1);

    // static float lfo2 = 0;
    // if(lfo2 > 1.0){
    //   lfo2 = 0;
    //   if(seq[1].next())
    // 	setParameterValue(PARAMETER_AH, 1.0);
    //   else
    // 	setParameterValue(PARAMETER_AH, 0.0);
    // }
    // lfo2 += 0.5 * getBlockSize() / getSampleRate();
    // setParameterValue(PARAMETER_AG, lfo2);
  }
};

#endif   // __DualEuclideanDrumPatch_hpp__
