#ifndef __ParallelPatch_hpp__
#define __ParallelPatch_hpp__

#include "StompBox.h"
#include "SineOscPatch.hpp"
#include "TrickySineOscPatch.hpp"
#define LEFT_PATCH SineOscPatch
#define RIGHT_PATCH TrickySineOscPatch

class ParallelPatch : public Patch {
private:
  LEFT_PATCH leftPatch;
  RIGHT_PATCH rightPatch;
  AudioBuffer* leftBuffer;
  AudioBuffer* rightBuffer;
public:
  MorphPatch(){
    registerParameter(MORPH_PARAMETER, "Morph");
    leftBuffer = AudioBuffer::create(1, getBlockSize());
    rightBuffer = AudioBuffer::create(1, getBlockSize());
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    leftBuffer->copyFrom(left);
    rightBuffer->copyFrom(right);
    leftPatch.processAudio(*leftBuffer);
    rightPatch.processAudio(*rightBuffer);
    left.copyFrom(*leftBuffer);
    right.copyFrom(*rightBuffer);
  }
};

#endif   // __ParallelPatch_hpp__
