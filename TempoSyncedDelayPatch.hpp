#ifndef __TempoSyncedDelayPatch_hpp__
#define __TempoSyncedDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

// #define TAP_THRESHOLD     256 // 78Hz at 20kHz sampling rate, or 16th notes at 293BPM
#define TAP_THRESHOLD     64// 256 // 78Hz at 20kHz sampling rate, or 16th notes at 293BPM
#define TRIGGER_LIMIT     (1<<17)

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
    /*
    if(trig < TAP_THRESHOLD)
      return;
    */
    if(on && !ison){
      if(trig < TRIGGER_LIMIT){
	// limit = trig-offset;
	limit = trig;
      }
      trig = 0;
      // debugMessage("limit/trig/speed ", limit, trig, speed);
    }
    // debugMessage("on/ison/ ", on, ison, on && !ison);
    ison = on;
  }
  void setSpeed(int16_t s){
    if(abs(speed-s) > 16){
      int64_t delta = (int64_t)limit*(speed-s)/2048;
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

static const float ratios[] = { 1.0/4, 
				1.0/3, 
				1.0/2, 
				3.0/4, 
				1.0, 
				3.0/2, 
				2.0,
				3.0, 
				4.0 };
static const int RATIOS_COUNT = 9;

class TempoSyncedDelayPatch : public Patch {
private:
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  int delayL, delayR;
  TapTempo tempo;
  StereoBiquadFilter* filter;
public:
  TempoSyncedDelayPatch() : 
    delayL(0), delayR(0), tempo(getSampleRate()*60/120) {
    registerParameter(PARAMETER_A, "Tempo");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Ratio");
    registerParameter(PARAMETER_D, "Dry/Wet");
    delayBufferL = CircularBuffer::create(TRIGGER_LIMIT);
    delayBufferR = CircularBuffer::create(TRIGGER_LIMIT/2);
    filter = StereoBiquadFilter::create(1);
    filter->setLowPass(8000/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
  }
  ~TempoSyncedDelayPatch(){
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
    StereoBiquadFilter::destroy(filter);
  }
  float delayTime(int ratio){
    float time = tempo.getPeriod() * ratios[ratio];
    time = max(0.01, min(1.0, time));
    return time;
  }
  void processAudio(AudioBuffer& buffer){
    int speed = getParameterValue(PARAMETER_A)*4096;
    float feedback = getParameterValue(PARAMETER_B);
    int ratio = (int)(getParameterValue(PARAMETER_C) * RATIOS_COUNT);
    int size = buffer.getSize();
    if(isButtonPressed(PUSHBUTTON)){
      uint16_t offset = getProgramVector()->parameters[5];
      // getProgramVector()->parameters[5] = 0;
      //      debugMessage("limit/trig/offset ", limit, trig, offset);
      debugMessage("offset ", offset);
      tempo.clock(offset);
      tempo.trigger(true);
      tempo.clock(size-offset);
    }else{
      tempo.trigger(false);
      tempo.clock(size);
    }
    tempo.setSpeed(speed);
    float time = delayTime(ratio);
    //     debugMessage("Time, Ratio, tempo ", time, ratios[ratio], 60*getSampleRate()/(tempo.getPeriod()*TRIGGER_LIMIT));
    //     debugMessage("button push, green, red", isButtonPressed(PUSHBUTTON), isButtonPressed(GREEN_BUTTON), isButtonPressed(RED_BUTTON));
    int newDelayL = time*(delayBufferL->getSize()-1);
    int newDelayR = time*(delayBufferR->getSize()-1);
    //    debugMessage("delay left/right", newDelayL, newDelayR);
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    for(int n=0; n<size; n++){
      float x1 = n/(float)size;
      float x0 = 1.0-x1;
      float ldly = delayBufferL->read(delayL)*x0 + delayBufferL->read(newDelayL)*x1;
      float rdly = delayBufferR->read(delayR)*x0 + delayBufferR->read(newDelayR)*x1;
      // ping pong
      delayBufferR->write(feedback*ldly + left[n]);
      delayBufferL->write(feedback*rdly + right[n]);
      left[n] = ldly*wet + left[n]*dry;
      right[n] = rdly*wet + right[n]*dry;
    }
    filter->process(buffer); // low pass filter output
    delayL = newDelayL;
    delayR = newDelayR;
  }
};

#endif   // __TempoSyncedDelayPatch_hpp__
