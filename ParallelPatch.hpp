#ifndef __ParallelPatch_hpp__
#define __ParallelPatch_hpp__

#include "StompBox.h"
#include "SineOscPatch.hpp"
#include "TrickySineOscPatch.hpp"
#define LEFT_PATCH SineOscPatch
#define RIGHT_PATCH TrickySineOscPatch
#define MORPH_PARAMETER PARAMETER_C

class ParallelPatch : public Patch {
private:
  LEFT_PATCH leftPatch;
  RIGHT_PATCH rightPatch;
  AudioBuffer* leftBuffer;
  AudioBuffer* rightBuffer;
public:
  ParallelPatch(){
    registerParameter(MORPH_PARAMETER, "Morph");
    leftBuffer = AudioBuffer::create(2, getBlockSize());
    rightBuffer = AudioBuffer::create(2, getBlockSize());
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    // The sub-patches read both channels (left=pitch, right=amplitude),
    // so each sub-buffer needs two channels; feed the single input to both.
    leftBuffer->getSamples(0).copyFrom(left);
    leftBuffer->getSamples(1).copyFrom(left);
    rightBuffer->getSamples(0).copyFrom(right);
    rightBuffer->getSamples(1).copyFrom(right);
    leftPatch.processAudio(*leftBuffer);
    rightPatch.processAudio(*rightBuffer);
    left.copyFrom(leftBuffer->getSamples(0));
    right.copyFrom(rightBuffer->getSamples(0));
  }
};

#endif   // __ParallelPatch_hpp__
