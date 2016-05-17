#ifndef __DualDrumPatch_hpp__
#define __DualDrumPatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"
#include "Cymbal.hpp"
#include "BassDrum.hpp"

class SampleTrigger {
private:
  VoltsPerOctave volts;
  Envelope* env;
  bool state;
  const float threshold = 2.0f; // 2v trigger threshold
public:
  SampleTrigger(Envelope* envelope) : 
    state(false){}
  void trigger(FloatArray samples){
    for(int i=0; i<samples.getSize(); ++i)
      if(volts.sampleToVolts(samples[i]) > threshold && !state){
	state = true;
	env->trigger(i);
      }else{
	state = false;
      }
  }
  void gate(FloatArray samples){
    for(int i=0; i<samples.getSize(); ++i){
      if(volts.sampleToVolts(samples[i]) > threshold){
	if(!state){
	  env->gate(true, i);
	  state = true;
	}
      }else{
	if(state){
	  env->gate(false, i);
	  state = false;
	}
      }
    }
  }
};

class DrumTrigger {
private:
  VoltsPerOctave volts;
  Drum* drum;
  bool state;
  const float threshold = 2.0f; // 2v trigger threshold
public:
  DrumTrigger(Drum* drm) : 
    drum(drm), state(false){}
  void getSamples(FloatArray samples){
    for(int i=0; i<samples.getSize(); ++i){
      if(volts.sampleToVolts(samples[i]) > threshold){
	if(!state){
	  state = true;
	  drum->trigger();
	}
      }else{
	state = false;
      }
      samples[i] = drum->getNextSample();
    }
  }
};

class DualDrumPatch : public Patch {
private:
  CymbalVoice* hat;
  BassDrumVoice* kick;
  DrumTrigger* drum[2];
public:
  DualDrumPatch(){
    registerParameter(PARAMETER_A, "Hat Tone");
    registerParameter(PARAMETER_B, "Hat Decay");
    registerParameter(PARAMETER_C, "Kick Tone");
    registerParameter(PARAMETER_D, "Kick Decay");
    registerParameter(PARAMETER_E, "Accent");
    hat = new CymbalVoice(getSampleRate());
    kick = new BassDrumVoice(getSampleRate());
    drum[0] = new DrumTrigger(hat);
    drum[1] = new DrumTrigger(kick);
    kick->setSnap(0.2);
  }
  ~DualDrumPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float tone = 120*powf(2, getParameterValue(PARAMETER_A)*4);
    float decay = getParameterValue(PARAMETER_B);
    float accent = getParameterValue(PARAMETER_E);
    hat->setFrequency(tone);
    hat->setFilter(getParameterValue(PARAMETER_A)*0.3 + 0.5);
    hat->setDecay(decay);
    hat->setAccent(accent);
    tone = 20*powf(2, getParameterValue(PARAMETER_C)*4);
    decay = getParameterValue(PARAMETER_D);
    kick->setFrequency(tone);
    kick->setDecay(decay);
    kick->setAccent(accent);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    drum[0]->getSamples(left);
    drum[1]->getSamples(right);
  }
};

#endif   // __DualDrumPatch_hpp__
