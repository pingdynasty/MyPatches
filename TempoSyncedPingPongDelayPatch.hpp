#ifndef __TempoSyncedPingPongDelayPatch_hpp__
#define __TempoSyncedPingPongDelayPatch_hpp__

#include "StompBox.h"
#include "BiquadFilter.h"
#include "CircularBuffer.hpp"
#include "TapTempo.hpp"

static const int RATIOS_COUNT = 9;
static const float ratios[RATIOS_COUNT] = { 1.0/4, 
					    1.0/3, 
					    1.0/2, 
					    3.0/4, 
					    1.0, 
					    3.0/2, 
					    2.0,
					    3.0, 
					    4.0 };
#define P_A PARAMETER_A
#define P_B PARAMETER_B
#define P_C PARAMETER_C
#define P_D PARAMETER_D
#define P_E PARAMETER_E

class TempoSyncedPingPongDelayPatch : public Patch {
private:
  static const int TRIGGER_LIMIT = (1<<16);
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  int delayL, delayR;
  TapTempo<TRIGGER_LIMIT> tempo;
  StereoBiquadFilter* highpass;
  StereoBiquadFilter* lowpass;
public:
  TempoSyncedPingPongDelayPatch() : 
    delayL(0), delayR(0), tempo(getSampleRate()*60/120) {
    registerParameter(P_A, "Tempo");
    registerParameter(P_B, "Feedback");
    registerParameter(P_C, "Ratio");
    registerParameter(P_D, "Dry/Wet");
    registerParameter(P_E, "Drop");
    delayBufferL = CircularBuffer::create(TRIGGER_LIMIT);
    delayBufferR = CircularBuffer::create(TRIGGER_LIMIT);
    highpass = StereoBiquadFilter::create(1);
    highpass->setHighPass(40/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q); // dc filter
    lowpass = StereoBiquadFilter::create(1);
    lowpass->setLowPass(8000/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
  }
  ~TempoSyncedPingPongDelayPatch(){
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
    StereoBiquadFilter::destroy(highpass);
    StereoBiquadFilter::destroy(lowpass);
  }
  float delayTime(int ratio){
    float time = tempo.getPeriod() * ratios[ratio];
    time = max(0.0001, min(0.9999, time));
    return time;
  }
  void processAudio(AudioBuffer& buffer){
    int speed = getParameterValue(P_A)*4096;
    float feedback = getParameterValue(P_B);
    float drop = 1.0-getParameterValue(P_E);
    if(drop < 0.01)
      feedback = 1.0;
    int ratio = (int)(getParameterValue(P_C) * RATIOS_COUNT);
    int size = buffer.getSize();
    tempo.trigger(isButtonPressed(PUSHBUTTON));
    tempo.clock(size);
    tempo.setSpeed(speed);
    float time = delayTime(ratio);
    int newDelayL = time*(delayBufferL->getSize()-1);
    int newDelayR = time*(delayBufferR->getSize()-1);
    float wet = getParameterValue(P_D);
    float dry = 1.0-wet;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    highpass->process(buffer);
    for(int n=0; n<size; n++){
      float x1 = n/(float)size;
      float x0 = 1.0-x1;
      float ldly = delayBufferL->read(delayL)*x0 + delayBufferL->read(newDelayL)*x1;
      float rdly = delayBufferR->read(delayR)*x0 + delayBufferR->read(newDelayR)*x1;
      // ping pong
      delayBufferR->write(feedback*ldly + drop*left[n]);
      delayBufferL->write(feedback*rdly + drop*right[n]);
      left[n] = ldly*wet + left[n]*dry;
      right[n] = rdly*wet + right[n]*dry;
    }
    lowpass->process(buffer);
    for(int i=0; i<buffer.getSize(); i++){
      left[i] = tanh(left[i]);    
      right[i] = tanh(right[i]);
    }
    delayL = newDelayL;
    delayR = newDelayR;
  }
};

#endif   // __TempoSyncedPingPongDelayPatch_hpp__
