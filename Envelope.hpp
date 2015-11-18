#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP
#include "message.h"

class AdsrEnvelope {
private:
  enum EnvelopeStage { kAttack, kDecay, kSustain, kRelease, kIdle };
  enum EnvelopeTrigger { kGate, kTrigger, kRetrigger };

public:
  AdsrEnvelope(float newSampleRate);
  void setAttack(float newAttack);
  void setDecay(float newDecay);
  void setRelease(float newRelase);
  void setSustain(float newSustain);
  void trigger();
  void trigger(int triggerDelay);
  void retrigger();
  void retrigger(int triggerDelay);
  void gate(bool state);
  void gate(bool state, int gateDelay);
  void setLevel(float newLevel);
  //  void setstage(stage); jump to the given stage, optionally with delay
  float getNextSample(); // increments envelope one step
  void getSamples(FloatArray &output); // increments envelope by output buffer length
  void attenuate(FloatArray &buf); // increments envelope by buffer length
private:
  static constexpr float minTime = 0.005;
  const float samplePeriod;
  EnvelopeStage stage;
  EnvelopeTrigger trig;
  float level;
  float attackIncrement;
  float decayIncrement;
  float releaseIncrement;
  float sustain;
  bool gateState;
  int gateTime;
};

AdsrEnvelope::AdsrEnvelope(float sampleRate) : 
  samplePeriod(1.0/sampleRate),
  stage(kIdle),
  trig(kGate),
  level(0.0),
  gateState(false),
  gateTime(0) {
  setAttack(0.0);
  setDecay(0.0);
  setSustain(1.0);
  setRelease(0.0);
}

void AdsrEnvelope::setAttack(float newAttack){
  newAttack = newAttack > minTime ? newAttack : minTime;
  attackIncrement = samplePeriod / newAttack;
}

void AdsrEnvelope::setDecay(float newDecay){
  newDecay = newDecay > minTime ? newDecay : minTime;
  decayIncrement = - samplePeriod / newDecay;
}

void AdsrEnvelope::setRelease(float newRelease){
  newRelease = newRelease > minTime ? newRelease : minTime;
  releaseIncrement = - samplePeriod / newRelease;
}

void AdsrEnvelope::setSustain(float newSustain){
  sustain = newSustain;
 // TODO: in the real world, you would probably glide to the new sustain level at a rate determined by either decay or attack
}

void AdsrEnvelope::retrigger(){
  retrigger(0);
}

void AdsrEnvelope::retrigger(int delay){
  gate(true, delay);
  trig = kRetrigger;
}

void AdsrEnvelope::trigger(){
  trigger(0);
}

void AdsrEnvelope::trigger(int delay){
  gate(true, delay);
  trig = kTrigger;
}

void AdsrEnvelope::gate(bool state){
  gate(state, 0);
}

void AdsrEnvelope::gate(bool state, int delay){
  if(gateState != state){
    gateTime = delay;
    gateState = state;
    if(gateTime == 0)
      stage = gateState ? kAttack : kRelease;
  }
  trig = kGate;
}

void AdsrEnvelope::setLevel(float newLevel){
  level = newLevel;
}

void AdsrEnvelope::attenuate(FloatArray &output){
  for(int n = 0; n < output.getSize(); n++)
    output[n] *= getNextSample();
}

void AdsrEnvelope::getSamples(FloatArray &output){
  for(int n = 0; n < output.getSize(); n++)
    output[n] = getNextSample();
  /*
  static int count = 0;
  count++;
  if(stage == kAttack){
    debugMessage("ATTACK, flag, susLevel, level", (float)(stage+((int)flag)*10), sustain, (float)level);
  }
  if(stage == kDecay){
    debugMessage("DECAY, flag, susLevel, level", (float)(stage+((int)flag)*10), sustain, (float)level);
  }
  if(stage == kSustain){
    debugMessage("SUSTAIN, flag, susLevel, level", (float)(stage+((int)flag)*10), sustain, (float)level);
  }
  if(stage == kRelease){
    debugMessage("RELEASE, flag, susLevel, level", (float)(stage+((int)flag)*10), sustain, (float)level);
  }
  */
}

float AdsrEnvelope::getNextSample(){
  if(gateTime && --gateTime == 0)
    stage = gateState ? kAttack : kRelease;
  //  update the state variable
  switch (stage) {
  case kAttack:
    // attack ramp
    level += attackIncrement;
    if(gateState == false){
      stage = kRelease;
    }else if(level >= 1.0){
      level = 1;
      stage = trig == kGate ? kDecay : kRelease;
    }
    break;
  case kDecay:
    // decay ramp
    level += decayIncrement;
    if(gateState == false){
      stage = kRelease;
    }else if(level <= sustain){
      level = sustain;
      stage = kSustain;
    }
    break;
  case kSustain:
    level = sustain;
    if(gateState == false)
      stage = kRelease;
    break;
  case kRelease:
    // release ramp
    level += releaseIncrement;
    if(gateState == true){
      stage = kAttack;
    }else if(level <= 0){
      stage = trig == kRetrigger ? kAttack : kIdle;
      level = 0;
    }
    break;
  case kIdle:
    level = 0;
    if(gateState == true)
      stage = kAttack;
    break;
  }
  // return level < 0 ? 0 : level;
  return level;
}

#endif /* ENVELOPE_HPP */
