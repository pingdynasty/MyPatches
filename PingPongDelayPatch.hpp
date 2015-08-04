////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 
 LICENSE:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */


/* created by the OWL team 2013, modified by Silvere Letellier for stereo capabilities */


////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PingPongDelayPatch_hpp__
#define __PingPongDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define SIMPLE_STEREO_DELAY_REQUEST_BUFFER_SIZE (64*1024)

class PingPongDelayPatch : public Patch {
private:
  CircularBuffer delayBufferL;
  CircularBuffer delayBufferR;
  int delayL, delayR;
public:
  PingPongDelayPatch() : delayL(0), delayR(0) {
    registerParameter(PARAMETER_A, "Ping");
    registerParameter(PARAMETER_B, "Pong");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Dry/Wet");
    AudioBuffer* buffer = createMemoryBuffer(2, SIMPLE_STEREO_DELAY_REQUEST_BUFFER_SIZE);
    delayBufferL.initialise(buffer->getSamples(0), buffer->getSize());
    delayBufferR.initialise(buffer->getSamples(1), buffer->getSize());
  }
  void processAudio(AudioBuffer &buffer){
    float ping = 0.05+0.95*getParameterValue(PARAMETER_A);
    float pong = 0.05+0.95*getParameterValue(PARAMETER_B);
    float feedback = getParameterValue(PARAMETER_C);
    int newDelayL = ping*(delayBufferL.getSize()-1);
    int newDelayR = pong*(delayBufferR.getSize()-1);
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    int size = buffer.getSize();
    for(int n = 0; n < size; n++){
      float sample = left[n];
      float x0 = (size-n)/(float)size;
      float x1 = n/(float)size;
      float ldly = delayBufferL.read(delayL)*x0 + delayBufferL.read(newDelayL)*x1;
      delayBufferR.write(feedback*ldly + sample);
      left[n] = ldly*wet + sample*dry;
      float rdly = delayBufferR.read(delayR)*x0 + delayBufferR.read(newDelayR)*x1;
      delayBufferL.write(feedback*rdly + sample);
      right[n] = rdly*wet + sample*dry;
    }
    delayL = newDelayL;
    delayR = newDelayR;
  }
};

#endif   // __PingPongDelayPatch_hpp__
