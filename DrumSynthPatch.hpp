#ifndef __DrumSynthPatch_hpp__
#define __DrumSynthPatch_hpp__

#include "StompBox.h"
#include "DrumSynthVoice.h"

class DrumSynthPatch : public Patch {
private:
  DrumSynthVoice voice;
public:
  DrumSynthPatch() {
    registerParameter(PARAMETER_A, "Time");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Ratio");
    registerParameter(PARAMETER_D, "Dry/Wet");
    initMidiNotes();
    voice.prepareToPlay(getSampleRate(), getBlockSize());
  }
  ~DrumSynthPatch(){
  }
  void processAudio(AudioBuffer& buffer){
    float a = getParameterValue(PARAMETER_A);
    int b = getParameterValue(PARAMETER_B)*127;
    int c = getParameterValue(PARAMETER_C)*127;
    if(isButtonPressed(PUSHBUTTON)){
      voice.startNote(b, c);
    }else if(isButtonPressed(RED_BUTTON)){
      voice.stopNote(true);
    }
    /*
    if(a > 0.8){
      voice.startNote(b, c);
    }else if(a < 0.1){
      voice.stopNote(false);
    }else if(a < 0.3){
      voice.stopNote(true);
    }
    */
    voice.renderNextBlock(buffer);
  }
};

#endif   // __DrumSynthPatch_hpp__
