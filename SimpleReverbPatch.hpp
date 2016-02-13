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

*/

/*
  Ported to the OWL by Martin Klang Feb 2016
  Ref: https://github.com/gordonjcp/reverb
*/

#include "StompBox.h"

/* note that buffers need to be a power of 2 */
/* if we scale the tap sizes for higher sample rates, this will need to be larger */
#define SIMPLE_REVERB_COMB_SIZE 4096
#define SIMPLE_REVERB_COMB_MASK (SIMPLE_REVERB_COMB_SIZE-1)
#define SIMPLE_REVERB_NUM_COMBS 4
#define SIMPLE_REVERB_NUM_APS 3

namespace SimpleReverb {
  typedef struct {
    /* structure for reverb parameters */
    /* controls */
    float decay;
    float size;
    float colour;

    float lpo;

    /* internal plugin variables */
    float comb[SIMPLE_REVERB_NUM_COMBS][SIMPLE_REVERB_COMB_SIZE];   /* buffers for comb filters */
    unsigned long comb_pos;             /* position within comb filter */

    float ap[SIMPLE_REVERB_NUM_APS][SIMPLE_REVERB_COMB_SIZE];        /* lazy, reuse comb size */
    unsigned long ap_pos;               /* position within allpass filter */

  } reverb_t;

  void reverb(float *in, float *out, unsigned long sample_count, reverb_t *params);

  /* these values are based on the CCRMA article on Schroeder's original reverb */
  /* they have been scaled to correspond to 44.1kHz Fs instead of 25kHz */
  /* todo: scale dynamically to current sampling frequency */
  unsigned int tap[SIMPLE_REVERB_NUM_COMBS] = {2975, 2824, 3621, 3970};
  float tap_gain[SIMPLE_REVERB_NUM_COMBS] = {0.964, 1.0, 0.939, 0.913};

  unsigned int ap_tap[SIMPLE_REVERB_NUM_APS] = {612, 199, 113};

  void reverb(float *in, float *out, unsigned long sample_count, reverb_t *params) {
    // handle the actual processing
    unsigned long pos;
    unsigned long comb_pos = params->comb_pos;
    unsigned long ap_pos = params->ap_pos;

    int c;

    float * const input = in;
    float * const output = out;

    float in_s, in_s1, temp;

    float gl, gh;
    float tilt = params->colour;
    float decay=params->decay;

    if (tilt > 0) {
      gl = -5 * tilt;
      gh = tilt;
    } else {
      gl = -tilt;
      gh = 5 * tilt;
    }

    gl = exp(gl/8.66)-1;
    gh = exp(gh/8.66)-1;

    //printf("%f %f      ", gl, gh);

    float n = 1/(5340 + 132300.0);
    float a0 = 2 * 5340 * n;
    float b1 = (132300 - 5340) * n;


    tap[0] = (int)(2975 * params->size);
    tap[1] = (int)(2824 * (params->size/2));
    tap[2] = (int)(3621 * params->size);
    tap[3] = (int)(3970 * (params->size/1.5));

    ap_tap[2] = (int)(400 * params->size);

    /* loop around the buffer */
    for (pos = 0; pos < sample_count; pos++) {
      /* loop around the comb filters */
      temp = 0;
      in_s = input[pos]/3;

      params->lpo = a0 * in_s + b1 * params->lpo;
      in_s1 = in_s + gl * params->lpo + gh *(in_s - params->lpo);

      for (c = 0; c<SIMPLE_REVERB_NUM_COMBS; c++) {
	params->comb[c][(comb_pos + tap[c]) & SIMPLE_REVERB_COMB_MASK] =
	  in_s1 + (decay * tap_gain[c]) * params->comb[c][comb_pos];
	temp += params->comb[c][comb_pos];
      }


      /* loop around the allpass filters */
#if 1
      for (c = 0; c<SIMPLE_REVERB_NUM_APS; c++) {
	params->ap[c][(ap_pos + ap_tap[c]) & SIMPLE_REVERB_COMB_MASK] =
	  temp + (decay * -0.3535) * params->ap[c][ap_pos];
	temp = (decay * 0.3535 * temp) + params->ap[c][ap_pos];
      }
#endif

      output[pos] = temp * 0.35;
      comb_pos++;
      comb_pos &= SIMPLE_REVERB_COMB_MASK;  /* increment and wrap buffer */
      ap_pos++;
      ap_pos &= SIMPLE_REVERB_COMB_MASK;  /* increment and wrap buffer */
      params->comb_pos = comb_pos;
      params->ap_pos = ap_pos;
    }
  }

}

class SimpleReverbPatch : public Patch {
private:  
  SimpleReverb::reverb_t params;
  FloatArray buffer;
public:
  SimpleReverbPatch(){
    bzero(params.comb, sizeof(float) * SIMPLE_REVERB_COMB_SIZE * SIMPLE_REVERB_NUM_COMBS);
    registerParameter(PARAMETER_A, "Colour");
    registerParameter(PARAMETER_B, "Size");
    registerParameter(PARAMETER_C, "Decay");
    registerParameter(PARAMETER_D, "Dry/Wet");
    buffer = FloatArray::create(getBlockSize());
  }      
  void processAudio(AudioBuffer &audio) {
    params.colour = getParameterValue(PARAMETER_A);
    params.size = getParameterValue(PARAMETER_B);
    params.decay = getParameterValue(PARAMETER_C);
    float wet = getParameterValue(PARAMETER_D);
    float* left = audio.getSamples(LEFT_CHANNEL);
    // float* right = audio.getSamples(RIGHT_CHANNEL);
    int size = audio.getSize();
    reverb(left, buffer, size, &params);
    for(int i=0; i<size; ++i)
      left[i] = buffer[i]*wet + left[i]*(1-wet);
  }
};

#endif   // __SimpleReverbPatch_hpp__
