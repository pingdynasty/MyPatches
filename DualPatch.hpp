#ifndef __DualPatch_hpp__
#define __DualPatch_hpp__

#include "StompBox.h"
#include "TrickySineOscPatch.hpp"
#include "SineOscPatch.hpp"

#define GREEN_PATCH SineOscPatch
#define RED_PATCH TrickySineOscPatch

class DualPatch : public Patch {
private:
  GREEN_PATCH green;
  RED_PATCH red;
public:
  void processAudio(AudioBuffer &buffer) {
    if(isButtonPressed(RED_BUTTON))
      red.processAudio(buffer);
    else
      green.processAudio(buffer);
  }
};

#endif   // __DualPatch_hpp__
