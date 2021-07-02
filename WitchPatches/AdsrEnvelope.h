#include "Envelope.h"

class AdsrEnvelope {
protected:
  enum Stage {
    ATTACK_STAGE, SUSTAIN_STAGE, RELEASE_STAGE, END_STAGE
  };
  enum OperatingMode {
    CYCLE_MODE, GATE_MODE, TRIGGER_MODE
  };
  enum EnvelopeTrigger { NO_TRIGGER, GATE_TRIGGER, TRIGGER_TRIGGER, CYCLE_TRIGGER };

public:
  float sr;
  float level;
  float linear;
  OperatingMode mode;
  EnvelopeTrigger trigger;
  float attack;
  float release;
  float attack_skew;
  float release_skew;
  float sustain;
  Stage stage;
  
  AdsrEnvelope(float sr) :
  sr(sr), level(0), linear(-MID_LEVEL), mode(GATE_MODE), trigger(NO_TRIGGER), attack(0), release(0), attack_skew(0), release_skew(0), stage(END_STAGE) {}
  void setAttack(float value, float period){
    attack = 1/(sampleRate*value);
  }
  void setRelease(float value, float period){
    release = 1/(sampleRate*value);
  }
  void setSkew(float skew){
    release_skew = release*skew;
    attack_skew = attack*skew;
    factor = skew > 0 ? -COMPENSATION_FACTOR : COMPENSATION_FACTOR;
  }
  float factor = 0.0;
  constexpr float COMPENSATION_FACTOR = 2.5;
  constexpr float MID_LEVEL = 0.5;
  /* compensate for skew to ensure that level will reach target in the right time */
  float compensateNonLinear(float lin, float exp){
    return clamp(lin - factor*(lin + MID_LEVEL - exp), -MID_LEVEL, MID_LEVEL);
  }
  float generate(){
    switch(stage){
    case ATTACK_STAGE:
      level += max(0.0f, attack + linear*attack_skew);
      linear += attack;
      if(linear >= MID_LEVEL){
	linear = MID_LEVEL;
	stage = SUSTAIN_STAGE;
      /* }else if(mode == GATE_MODE && isTriggerLow()){ */
      }else if(trigger == NO_TRIGGER){
	// if attack is interrupted
	linear = compensateNonLinear(linear, level);
	stage = RELEASE_STAGE;
      }
      break;
    case SUSTAIN_STAGE:
      if(trigger != GATE_TRIGGER)
      /* if(mode != GATE_MODE || isTriggerLow()) */
	stage = RELEASE_STAGE;
      break;
    case RELEASE_STAGE:
      level -= max(0.0f, release - linear*release_skew);
      linear -= release;
      if(linear <= -MID_LEVEL){
	linear = -MID_LEVEL;
	level = 0;
	stage = END_STAGE;
      }
      break;
    case END_STAGE:
      if(trigger == CYCLE_TRIGGER)
	stage = ATTACK_STAGE;
      else
	trigger = NO_TRIGGER;
    }
    level = clamp(level, 0.0f, 1.0f);
    return level;
  }

  void trigger(){
    if(stage == RELEASE_STAGE)
      linear = compensateNonLinear(linear, level);
    stage = ATTACK_STAGE;
    trigger = TRIGGER_TRIGGER;
  }

  void gate(bool state){
    if(state){
      if(stage == RELEASE_STAGE)
	linear = compensateNonLinear(linear, level);
      stage = ATTACK_STAGE;
      trigger = GATE_TRIGGER;
    }else{
      trigger = NO_TRIGGER;
    }
  }
};
