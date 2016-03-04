#ifndef __GVerbPatch_hpp__
#define __GVerbPatch_hpp__
/*

        Copyright (C) 1999 Juhana Sadeharju
                       kouhia at nic.funet.fi

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

/**
 * Port of GVerb to the OWL by Martin Klang, Feb 2016
 * Ref: https://github.com/swh/lv2
 */

// make PATCH_OBJS='gverb.o gverbdsp.o' PATCHNAME=GVerb

#include "StompBox.h"

namespace GVerb {
  const static float MAX_SIZE = 300.0f;
  extern "C"{
    inline int isnan(float x){
      return std::isnan(x);
    }
#include "gverb.h"
  }
}

class GVerbPatch : public Patch {
private:
  GVerb::ty_gverb verb;
  float size;
public:
  GVerbPatch(){
    registerParameter(PARAMETER_A, "Size");
    registerParameter(PARAMETER_B, "Time");
    registerParameter(PARAMETER_C, "Damp");
    registerParameter(PARAMETER_D, "Dry/Wet");
    // registerParameter(PARAMETER_E, "Early");
    size = 50.0f;
    GVerb::gverb_init(&verb, getSampleRate(), GVerb::MAX_SIZE, size, 7.0f, 0.5f, 15.0f, 0.5f, 0.5f, 0.5f);
  }      
  void processAudio(AudioBuffer &buffer) {
    size = 0.9 * size + 0.1 * getParameterValue(PARAMETER_A)*GVerb::MAX_SIZE+1.0f;
    float time = getParameterValue(PARAMETER_B)*30.0f+0.1;
    float damp = getParameterValue(PARAMETER_C);
    float wet = getParameterValue(PARAMETER_D);
    // float early = getParameterValue(PARAMETER_E);
    GVerb::gverb_set_roomsize(&verb, size);
    GVerb::gverb_set_revtime(&verb, time);
    GVerb::gverb_set_damping(&verb, damp);
    // GVerb::gverb_set_earlylevel(&verb, early);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    float l, r, dry;
    if(isButtonPressed(PUSHBUTTON)){
      GVerb::gverb_flush(&verb);
    }else{
      for(int i=0; i<buffer.getSize(); ++i){
	GVerb::gverb_do(&verb, left[i], &l, &r);
	dry = left[i]*(1-wet);
	left[i] = l*wet + dry;
	right[i] = r*wet + dry;
      }
    }
  }
};

#endif   // __GVerbPatch_hpp__
