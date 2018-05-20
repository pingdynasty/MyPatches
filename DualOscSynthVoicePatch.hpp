#ifndef __DualOscSynthVoicePatch_hpp__
#define __DualOscSynthVoicePatch_hpp__

#include "StompBox.h"
#include "VoltsPerOctave.h"
#include "PolyBlepOscillator.h"
#include "BiquadFilter.h"
#include "SmoothValue.h"
#include "Control.h"

/*
 * A: Pitch
 * C: Fc exp
 * D: Resonance
 * E: Waveshape
 * Left: Pitch
 * Right: Fc lin
 */
class DualOscSynthVoicePatch : public Patch {
private:
  PolyBlepOscillator osc1, osc2;
  VoltsPerOctave hz;
  BiquadFilter* filter;
  IntParameter tune, detune;
  FloatParameter cutoff, q, shape;
  int basenote = 0;
  Control<PARAMETER_F> mix1;
  Control<PARAMETER_G> mix2;
public:
  DualOscSynthVoicePatch() : osc1(getSampleRate()), osc2(getSampleRate()) {
    tune = getIntParameter("Tune", -3*12, 14, 0, 0, 0.5);
    detune = getIntParameter("Detune", -12, 13, 0, 0, 0.5);
    cutoff = getFloatParameter("Cutoff", 0, 1);
    q = getFloatParameter("Resonance", 0.6, 2.5);
    shape = getFloatParameter("Waveshape", 0, 2);
    registerParameter(PARAMETER_F, "Mix1");
    registerParameter(PARAMETER_G, "Mix2");
    filter = BiquadFilter::create(4); // 8-pole filter 48dB
    resetPatch();
    // hz.offset = -0.0577;
    // hz.multiplier = -4.32958;
    hz.offset = -0.0565f;
    hz.multiplier = -4.348f;
  }
  ~DualOscSynthVoicePatch(){
    BiquadFilter::destroy(filter);
  }
  void resetPatch(){
    mix1 = 0.5;
    mix2 = 0.5;    
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid >= MIDI_NOTE_BUTTON){
      uint8_t note = bid - MIDI_NOTE_BUTTON;
      if(value)
	basenote = note - 60;
    }else if(bid == PUSHBUTTON){
      resetPatch();
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float pw = 0.5;
    if(shape > 1.0){
      pw += 0.49*(shape-1.0); // pw 0.5 to 0.99
      osc1.setShape(1.0); // square wave
      osc1.setPulseWidth(pw);
      osc2.setShape(1.0);
      osc2.setPulseWidth(pw);
    }else{
      osc1.setShape(shape);
      osc2.setShape(shape);
    }
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    // vco1
    hz.setTune((basenote+tune)/12.0);
    hz.getFrequency(left, left);
    osc1.getSamples(left, left);
    left.multiply(mix1);
    // vco2
    hz.setTune((basenote+tune+detune)/12.0);
    hz.getFrequency(right, right);
    osc2.getSamples(right, right);
    right.multiply(mix2);
    right.add(left);
    // vcf
    float fc = cutoff*cutoff*0.9+0.01;
    filter->setLowPass(fc, q);
    filter->process(right);
    right.multiply(.5*(1-q*0.3)); // gain compensation for high q
    left.multiply(.5);
  }
};

#endif   // __DualOscSynthVoicePatch_hpp__
