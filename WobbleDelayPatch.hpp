#ifndef __WobbleDelayPatch_hpp__
#define __WobbleDelayPatch_hpp__

#include "StompBox.h"
#include "BiquadFilter.h"
#include "CircularBuffer.hpp"
#include "TapTempo.hpp"

class WobbleDelayPatch : public Patch {
private:
  static const int TRIGGER_LIMIT = (1<<16);
  const float mul;
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  TapTempo<TRIGGER_LIMIT> tempo;
  int delay;
  float lfophase;
  StereoBiquadFilter* highpass;
  StereoBiquadFilter* lowpass;
public:
  WobbleDelayPatch() : 
    mul(2*M_PI/getSampleRate()),
    delay(0), 
    tempo(getSampleRate()*60/120),
    lfophase(0)
  {
    registerParameter(PARAMETER_A, "Feedback");
    registerParameter(PARAMETER_B, "Rate");
    registerParameter(PARAMETER_C, "Depth");
    registerParameter(PARAMETER_D, "Dry/Wet");
    delayBufferL = CircularBuffer::create(TRIGGER_LIMIT);
    delayBufferR = CircularBuffer::create(TRIGGER_LIMIT);
    highpass = StereoBiquadFilter::create(1);
    highpass->setHighPass(40/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q); // dc filter
    lowpass = StereoBiquadFilter::create(1);
    lowpass->setLowPass(8000/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
  }
  ~WobbleDelayPatch() {
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
    StereoBiquadFilter::destroy(highpass);
    StereoBiquadFilter::destroy(lowpass);
  }
  float process(float sample, CircularBuffer* buf, float delay, float feedback, float wet){
    delay = max(0.001, min(buf->getSize(), delay));
    float dly = buf->interpolate(delay);
    buf->write(feedback * dly + sample);
    return dly*wet + sample*(1.0f-wet);
  }
  void processAudio(AudioBuffer &buffer){
    float feedback = getParameterValue(PARAMETER_A);
    float rate = getParameterValue(PARAMETER_B) * 10 * mul;
    float depth = getParameterValue(PARAMETER_C) * 60.0;
    float wet = getParameterValue(PARAMETER_D);
    float time = tempo.getPeriod();
    float delayL = time*(delayBufferL->getSize()-1);
    float delayR = time*(delayBufferR->getSize()-1);
    highpass->process(buffer);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    for (int n = 0; n < size; n++){
      float lfo = sinf(lfophase);
      left[n] = process(left[n], delayBufferL, delayL+lfo*depth, feedback, wet);
      right[n] = process(right[n], delayBufferR, delayR-lfo*depth, feedback, wet);
      lfophase += rate;
      if(lfophase > 2*M_PI)
	lfophase -= 4*M_PI;
    }
    lowpass->process(buffer);
  }
};

#endif   // __WobbleDelayPatch_hpp__
