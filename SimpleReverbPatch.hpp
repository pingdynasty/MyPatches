#ifndef __SimpleReverbPatch_hpp__
#define __SimpleReverbPatch_hpp__

/*

reverb.c
Effect engine for a simple reverb plugin

Copyright (c) 2015 Gordon JC Pearce <gordonjcp@gjcp.net>

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright notice
and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.

  Ref: https://github.com/gordonjcp/reverb

*/

/*
  Ported to the OWL by Martin Klang Feb 2016
  Ref: http://github.com/marsus/MyPatches
*/

#include "StompBox.h"

namespace SimpleReverb {
  extern "C"{
#include "reverb.h"
  }
}

class SimpleReverbPatch : public Patch {
private:  
  SimpleReverb::reverb_t lp;
  SimpleReverb::reverb_t rp;
  FloatArray lbuf, rbuf;
public:
  SimpleReverbPatch(){
    bzero(lp.comb, sizeof(float) * COMB_SIZE * NUM_COMBS);
    bzero(rp.comb, sizeof(float) * COMB_SIZE * NUM_COMBS);
    registerParameter(PARAMETER_A, "Size");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Colour");
    registerParameter(PARAMETER_D, "Dry/Wet");
    lbuf = FloatArray::create(getBlockSize());
    rbuf = FloatArray::create(getBlockSize());
  }      
  void processAudio(AudioBuffer &audio) {
    lp.size = rp.size = getParameterValue(PARAMETER_A);
    lp.decay = rp.decay = getParameterValue(PARAMETER_B);
    lp.colour = rp.colour = getParameterValue(PARAMETER_C);
    float wet = getParameterValue(PARAMETER_D);
    float* left = audio.getSamples(LEFT_CHANNEL);
    float* right = audio.getSamples(RIGHT_CHANNEL);
    int size = audio.getSize();
    reverb(left, lbuf, size, &lp);
    reverb(right, rbuf, size, &rp);
    for(int i=0; i<size; ++i){
      left[i] = lbuf[i]*wet + left[i]*(1-wet);
      right[i] = rbuf[i]*wet + right[i]*(1-wet);
    }
  }
};

#endif   // __SimpleReverbPatch_hpp__
