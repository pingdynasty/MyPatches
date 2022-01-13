#pragma once

#include "Patch.h"
#include "OpenWareLibrary.h"

#include <complex>
typedef std::complex<float> float_complex;

// typedef ComplexFloat float_complex;
// inline float real(ComplexFloat cf){
//   return cf.re;
// }

#define MAX_DELAY 50

/* Original effect and demo program:
    Alien-Wah by Nasca Octavian Paul from Tg. Mures, Romania
    e-mail:  <paulnasca@email.ro> or <paulnasca@yahoo.com>.    

    Copyright (C) 2001 Nasca Octavian Paul

    This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

    This program is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111 USA

 The algorithm was found by me by mistake(I was looking for
something else); I c alled this effect "Alien Wah" because sounds
a bit like wahwah, but more strange .  The ideea of this effect
is very simple: It is a feedback delay who uses comple x numbers.
If x[] represents the input and y[] is the output, so a simple
feedback delay l ooks like this: y[n]=y[n-delay]*fb+x[n]*(1-fb)
'fb' is a real number between 0 and 1.  If you change the fb with
a complex number who has the MODULUS smaller than 1, it will look
like this.

fb=R*(cos(alpha)+i*sin(alpha));  i^2=-1; R<1;
y[n]=y[n-delay]*R*(cos(alpha)+i*sin(alpha))+x[n]*(1-R);

 alpha is the phase of the number and is controlled by the
LFO(Low Frequency Osc illator).  If the 'delay' parameter is low,
the effect sounds more like wah-wah, but if it is big, the effect
will sound very interesting.  The input x[n] has the real part of
the samples from the wavefile and the imagi nary part is zero.
The output of this effect is the real part of y[n].
*/

class AW {
private:

#define LFO_SKIPSAMPLES 25

  /* State */
  bool inited;
  unsigned long samplerate;
  unsigned long t;
  unsigned long t2;
  unsigned long k;
  unsigned long k2;
  float_complex *delaybuf;
  float_complex *delaybuf2;
  float_complex c;
  float_complex c2;

public:
  
  /* Ports */
  float freq;
  float startphase;
  float feedback;
  unsigned int delay;
  float * i_buf;
  float * o_buf;
  float * i_buf2;
  float * o_buf2;

  /*
   * class constructor
   */
  AW(const long lSampleRate) :
    inited(false), 
    samplerate(lSampleRate),
    t(0), t2(0),
    k(0), k2(0),
    c(float_complex(0,0)),
    c2(float_complex(0,0)),
    delaybuf(NULL),
    delaybuf2(NULL) {
  }
  ~AW(){
    delete[] delaybuf;
    delete[] delaybuf2;
  }
  void initState(int chans, int max_delay) {
    delaybuf = new float_complex[max_delay+1];
    for (unsigned int i =0; i<max_delay+1; ++i) {
      delaybuf[i] = float_complex(0,0);
    }
    if (chans == 2) {
      delaybuf2 = new float_complex[max_delay+1];
      for (unsigned int i =0; i<max_delay+1; ++i) {
	delaybuf2[i] = float_complex(0,0);
      }
    }
  }

  /*
   * Mono effect
   * Do the effect. 'i_buf' is transformed into 'o_buf'
   */
  void runAW_Mono(unsigned long samplecount) {
    AW * me = this;
    float lfo;
    float_complex outc;
    float lfoskip = me->freq * 2 * M_PI / me->samplerate;

    for(unsigned int i=0; i<samplecount; ++i) {
      if ((me->t++ % LFO_SKIPSAMPLES) == 0) {
	lfo = 1 + cos(me->t * lfoskip + me->startphase);
	me->c = float_complex(cos(lfo) * me->feedback,
			      sin(lfo) * me->feedback);
      }
      outc = me->c * me->delaybuf[me->k] + (1 - me->feedback) * 
	me->i_buf[i];
      me->delaybuf[me->k] = outc;
      if (++(me->k) >= me->delay) me->k=0;
      me->o_buf[i] = real(outc)*3;
    }
  }

  /*
   * Stereo effect
   */
  void runAW_Stereo(unsigned long samplecount) {
    AW * me = (AW *)this;
    float lfo;
    float_complex outc;
    float lfoskip = me->freq * 2 * M_PI / me->samplerate;

    for(unsigned int i=0; i<samplecount; ++i) {
      if ((me->t++ % LFO_SKIPSAMPLES) == 0) {
	lfo = 1 + cos(me->t * lfoskip + me->startphase);
	me->c = float_complex(cos(lfo) * me->feedback,
			      sin(lfo) * me->feedback);
      }
      outc = me->c * me->delaybuf[me->k] + (1 - me->feedback) * 
	me->i_buf[i];
      me->delaybuf[me->k] = outc;
      if (++(me->k) >= me->delay) me->k=0;
      me->o_buf[i] = real(outc)*3;
    }

    for(unsigned int i=0; i<samplecount; ++i) {
      if ((me->t2++ % LFO_SKIPSAMPLES) == 0) {
	lfo = 1 + cos(me->t2 * lfoskip);
	me->c2 = float_complex(cos(lfo) * me->feedback,
			       sin(lfo) * me->feedback);
      }
      outc = me->c2 * me->delaybuf2[me->k2] + (1 - me->feedback) * 
	me->i_buf2[i];
      me->delaybuf2[me->k2] = outc;
      if (++(me->k2) >= me->delay) me->k2=0;
      me->o_buf2[i] = real(outc)*3;
    }
  }
};

class AlienWahPatch : public Patch {
private:
  AW aw;
  SmoothFloat freq;
  SmoothFloat delay;
  SmoothFloat feedback;
  SmoothFloat phase;
  FloatArray bufl, bufr;
public:
  AlienWahPatch() : aw(getSampleRate()) {
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Delay");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Mix");
    registerParameter(PARAMETER_E, "Phase");
    aw.initState(getNumberOfChannels() > 1 ? 2 : 1, MAX_DELAY);
    bufl = FloatArray::create(getBlockSize());
    bufr = FloatArray::create(getBlockSize());
  }
  ~AlienWahPatch(){
  }

  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    freq = getParameterValue(PARAMETER_A);
    delay = getParameterValue(PARAMETER_B)*(MAX_DELAY-5) + 5; // 5 to 50
    feedback = getParameterValue(PARAMETER_C)*0.9;
    float mix = getParameterValue(PARAMETER_D);
    phase = getParameterValue(PARAMETER_E);
    aw.startphase = phase;
    aw.freq = freq;
    aw.feedback = feedback;
    aw.delay = delay;
    
    aw.i_buf = left;
    aw.o_buf = bufl;
    aw.i_buf2 = right;
    aw.o_buf2 = bufr;
    aw.runAW_Stereo(left.getSize());

    // dry/wet mix
    bufl.multiply(mix);
    left.multiply(1-mix);
    left.add(bufl);
    bufr.multiply(mix);
    right.multiply(1-mix);
    right.add(bufr);    
  }
};

