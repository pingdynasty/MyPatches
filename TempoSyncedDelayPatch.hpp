#ifndef __TempoSyncedDelayPatch_hpp__
#define __TempoSyncedDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define TAP_THRESHOLD     256 // 78Hz at 20kHz sampling rate, or 16th notes at 293BPM
// #define TRIGGER_LIMIT     262144 // 2^18
#define TRIGGER_LIMIT     131072 // 2^17
// (48000*(60/10)) // min tempo 10 bpm (8 beats per second)
 // max tempo 480 bpm (8 beats per second)

class TapTempo {
private:
  uint32_t limit;
  uint32_t trig;
  uint16_t speed;
  bool ison;
public:
  TapTempo(uint32_t tempo) : 
    limit(tempo), trig(TRIGGER_LIMIT), 
    speed(2048), ison(false) {}
  void trigger(bool on){
    if(trig < TAP_THRESHOLD)
      return;
    if(on && !ison){
      if(trig < TRIGGER_LIMIT){
	limit = trig;
	trig = 0;
      }else{
	trig = 0;
      }
    }
    ison = on;
  }
  void setSpeed(int16_t s){
    if(abs(speed-s) > 16){
      int64_t delta = (int64_t)limit*(s-speed)/2048;
      limit = max(1, limit+delta);
      speed = s;
    }
  }
  float getPeriod(){
    return float(limit)/TRIGGER_LIMIT;
  }
  void clock(){
    if(trig < TRIGGER_LIMIT)
      trig++;
  }
  void clock(uint32_t steps){
    trig += steps;
    if(trig > TRIGGER_LIMIT)
      trig = TRIGGER_LIMIT;
  }
};

static const float ratios[] = { 1.0/8, 
				1.0/6, 
				1.0/4, 
				1.0/3, 
				1.0/2, 
				3.0/4, 
				1.0, 
				3.0/2, 
				2.0, 
				3.0, 
				4.0, 
				6.0, 
				8.0 };

class TempoSyncedDelayPatch : public Patch {
private:
  static const int RATIOS = 13;
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  int delayL, delayR;
  TapTempo tempo;
public:
  TempoSyncedDelayPatch() : 
    delayL(0), delayR(0), tempo(getSampleRate()*60/120) {
    registerParameter(PARAMETER_A, "Time");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Ratio");
    registerParameter(PARAMETER_D, "Dry/Wet");
    delayBufferL = CircularBuffer::create(TRIGGER_LIMIT);
    delayBufferR = CircularBuffer::create(TRIGGER_LIMIT/2);
  }
  ~TempoSyncedDelayPatch(){
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
  }
  float delayTime(int ratio){
    float time = tempo.getPeriod() * ratios[ratio];
    time = max(0.01, min(1.0, time));
    return time;
  }
  void processAudio(AudioBuffer& buffer){
    int speed = getParameterValue(PARAMETER_A)*4096;
    float feedback = getParameterValue(PARAMETER_B);
    int ratio = (int)(getParameterValue(PARAMETER_C) * RATIOS);
    tempo.trigger(isButtonPressed(PUSHBUTTON));
    tempo.setSpeed(speed);
    float time = delayTime(ratio);
    int newDelayL = time*(delayBufferL->getSize()-1);
    int newDelayR = time*(delayBufferR->getSize()-1);
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    int size = buffer.getSize();
    for(int n = 0; n < size; n++){
      float sample = left[n];
      float x0 = (size-n)/(float)size;
      float x1 = n/(float)size;
      float ldly = delayBufferL->read(delayL)*x0 + delayBufferL->read(newDelayL)*x1;
      delayBufferR->write(feedback*ldly + sample);
      left[n] = ldly*wet + sample*dry;
      float rdly = delayBufferR->read(delayR)*x0 + delayBufferR->read(newDelayR)*x1;
      delayBufferL->write(feedback*rdly + sample);
      right[n] = rdly*wet + sample*dry;
    }
    delayL = newDelayL;
    delayR = newDelayR;
    tempo.clock(size);
  }
};

#endif   // __TempoSyncedDelayPatch_hpp__
