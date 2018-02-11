#ifndef __TestTonePatch_hpp__
#define __TestTonePatch_hpp__

#include "StompBox.h"
// #include "Oscillators.hpp"

class TestPatch : public Patch {
private:
  // FloatParameter a, b, c, d, e, f, g, h;
  // FloatParameter aa, ab, ac, ad;
  // IntParameter ae, af, ag, ah;
  // SineOscillator osc;
public:
  // TestPatch() : osc(getSampleRate()) {
    // a = getParameter("Freq1", 0.1);
    // b = getParameter("Freq2", 0.2);
    // c = getParameter("Freq3", 0.3);
    // d = getParameter("Freq4", 0.4);
    // e = getParameter("Freq5", 0.5);
    // f = getParameter("Freq6", 0.6);
    // g = getParameter("Freq7", 0.7);
    // h = getParameter("Freq8", 0.8);
    // aa = getParameter("Freq9", 0.9);
    // ab = getParameter("Freq10", .0);
    // ac = getParameter("Freq11", 0.1);
    // ad = getParameter("Freq12", 0.2);
    // ae = getIntParameter("Freq13", 0, 100, 30);
    // af = getIntParameter("Freq14", 0, 100, 40);
    // ag = getIntParameter("Freq15", 0, 100, 50);
    // ah = getIntParameter("Freq16", 0, 100, 60);
  // }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      debugMessage("midi note", note, value, samples);
    }else if(bid == PUSHBUTTON){
      debugMessage("push event", bid, value, samples);
      // setButton((PatchButtonId)(MIDI_NOTE_BUTTON+60), ON);
      // setParameterValue(PARAMETER_AA, getParameterValue(PARAMETER_A));
      // setParameterValue(PARAMETER_A, getParameterValue(PARAMETER_B));
    }else{
      static int count = 0;
      debugMessage("button event", bid, value, ++count);
    }
  }

  void processAudio(AudioBuffer &buffer) {
    float a = getParameterValue(PARAMETER_A);
    float b = getParameterValue(PARAMETER_B);
    float c = getParameterValue(PARAMETER_C);
    float d = getParameterValue(PARAMETER_D);
    float e = getParameterValue(PARAMETER_E);
    float aa = getParameterValue(PARAMETER_AA);
    float ab = getParameterValue(PARAMETER_AB);
    float ac = getParameterValue(PARAMETER_AC);
    float ad = getParameterValue(PARAMETER_AD);
    // float ae = getParameterValue(PARAMETER_AE);
    // debugMessage("a/b/c", getParameterValue(PARAMETER_A), getParameterValue(PARAMETER_B), getParameterValue(PARAMETER_C));

    debugMessage("c/d/e", getParameterValue(PARAMETER_C), getParameterValue(PARAMETER_D), getParameterValue(PARAMETER_E));

    // static float previous = 0.0;
    // if(getParameterValue(PARAMETER_B) >= 0.5 && previous < 0.5){
    //   setButton((PatchButtonId)(MIDI_NOTE_BUTTON+60), ON);
    //   debugMessage("set note", getParameterValue(PARAMETER_B));
    // }
    // previous = getParameterValue(PARAMETER_B);
    // debugMessage("aa/ab/ac", aa, ab, ac);

    // debugMessage("button", isButtonPressed(PUSHBUTTON));
    // debugMessage("button A/B/C", isButtonPressed(BUTTON_A), isButtonPressed(BUTTON_A), isButtonPressed(BUTTON_C));

    // debugMessage("ae/af/ah", ae, af, ag);
    // debugMessage("aa/ab/ac", aa, ab, ac);
    // debugMessage("ac/ad/ae", ac, ad, ae);
    // osc.setFrequency(a*ab*2000);
    // osc.getSamples(buffer.getSamples(LEFT_CHANNEL));
    // setParameterValue(PARAMETER_AA, getParameterValue(PARAMETER_A));
    // static int counter = 0;
    // if(counter++ > 8){
    //   counter = 0;
    // if(isButtonPressed(PUSHBUTTON)){

    // setParameterValue(PARAMETER_AA, getParameterValue(PARAMETER_A));
    // setParameterValue(PARAMETER_AB, getParameterValue(PARAMETER_B));
    // setParameterValue(PARAMETER_AC, getParameterValue(PARAMETER_C));
    // setParameterValue(PARAMETER_AD, getParameterValue(PARAMETER_D));

    // }
    // if(getParameterValue(PARAMETER_A) > 0.5)
    //   setButton(PUSHBUTTON, true);
    // else
    //   setButton(PUSHBUTTON, false);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    float gain = getParameterValue(PARAMETER_A)*2;
    int size = buffer.getSize();
    for(int i=0; i<size; ++i){
      left[i] = gain*i/size - gain*0.5;
      right[i] = -gain*i/size + gain*0.5;
    }
    // debugMessage("gain", gain);
    // debugMessage("gain/min/max", gain, left.getMinValue(), left.getMaxValue());
  }
};

#endif   // __TestTonePatch_hpp__
