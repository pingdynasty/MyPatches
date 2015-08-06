#ifndef __PingPongDelayPatch_hpp__
#define __PingPongDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

class PingPongDelayPatch : public Patch {
private:
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  int delayL, delayR;
public:
  PingPongDelayPatch() : delayL(0), delayR(0) {
    registerParameter(PARAMETER_A, "Ping");
    registerParameter(PARAMETER_B, "Pong");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Dry/Wet");
    delayBufferL = CircularBuffer::create(64*1024);  // 1.36s
    delayBufferR = CircularBuffer::create(128*1024); // 2.73s
  }
  ~PingPongDelayPatch(){
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
  }
  void processAudio(AudioBuffer &buffer){
    float ping = 0.01+0.99*getParameterValue(PARAMETER_A);
    float pong = 0.01+0.99*getParameterValue(PARAMETER_B);
    float feedback = getParameterValue(PARAMETER_C);
    int newDelayL = ping*(delayBufferL->getSize()-1);
    int newDelayR = pong*(delayBufferR->getSize()-1);
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
  }
};

#endif   // __PingPongDelayPatch_hpp__
