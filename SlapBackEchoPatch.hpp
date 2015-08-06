#ifndef __SlapBackEchoPatch_hpp__
#define __SlapBackEchoPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

/**
 * Short slap back echo with stereo output separation.
 * 1 input, 2 output channels.
 * Adjust delay time from 17 to 170ms.
 * Set Mix to 0 for dry signal in the left output and echo in the right.
 * Set Mix to 50% for dry and wet (echo) in both channels.
 * Set Mix to 100% for echo in the left channel and dry in the left.
 */
class SlapBackEchoPatch : public Patch {
private:
  CircularBuffer* delayBuffer;
  int delay;
public:
  SlapBackEchoPatch() : delay(0) {
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_D, "Mix");
    delayBuffer = CircularBuffer::create(8*1024);  // 170ms
  }
  ~SlapBackEchoPatch(){
    CircularBuffer::destroy(delayBuffer);
  }
  void processAudio(AudioBuffer &buffer){
    float time = 0.1+0.9*getParameterValue(PARAMETER_A);
    int newDelay = time*(delayBuffer->getSize()-1);
    float mix = getParameterValue(PARAMETER_D);
    float nmix = 1.0-mix;
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    int size = buffer.getSize();
    for(int n = 0; n < size; n++){
      float sample = left[n];
      float x0 = (size-n)/(float)size;
      float x1 = n/(float)size;
      delayBuffer->write(sample);
      float echo = delayBuffer->read(delay)*x0 + delayBuffer->read(newDelay)*x1;
      left[n] = nmix*sample + mix*echo;
      right[n] = mix*sample + nmix*echo;
    }
    delay = newDelay;
  }
};

#endif   // __SlapBackEchoPatch_hpp__
