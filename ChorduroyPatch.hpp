#ifndef __ChorduroyPatch_hpp__
#define __ChorduroyPatch_hpp__

#include "Patch.h"
#include "OpenWareLibrary.h"

// #define USE_SVF

/**
 * Triple tone generator that produces a triad chord that tracks the frequency of the input audio signal.
 * Select between three inversions of each triad: Diminished, Minor, Major and Augmented.
 * ref: https://en.wikipedia.org/wiki/Triad_(music)
 */
#define SEMI    (1.0/12)
#define WHOLE   (2.0/12)
#define TRIADS 5

static const float TRIAD_SEMITONES[TRIADS][5] = {
   {0, 1, 6, 12, 12+1}, // Viennese trichord
   {0, 3, 6, 12, 12+3}, // Diminishied triad
   {0, 3, 7, 12, 12+3}, // Minor triad
   {0, 4, 7, 12, 12+4}, // Major triad
   {0, 4, 8, 12, 12+4}  // Augmented triad
};

class Voice {
private:
  PolyBlepOscillator* osc[3];
  SineOscillator* sine;
#ifdef USE_SVF
  StateVariableFilter* vcf;
#else
  BiquadFilter* vcf;
#endif
  ExponentialDecayEnvelope env;
  FloatArray buffer;
  float subosc = 1.0;
  float fc = 18000;
  float q = 4;
  int triad = 0;
  int inversion = 0;
public:
  Voice(float sr){
#ifdef USE_SVF
    vcf = StateVariableFilter::create(sr);
#else
    vcf = BiquadFilter::create(sr, 4);
#endif
    for(int i=0; i<3; ++i){
      osc[i] = PolyBlepOscillator::create(sr);
      osc[i]->setShape(0.0f); //  0 for saw, 1 for square wave
      // osc[i]->setPulseWidth(pwm);
    }
    sine = SineOscillator::create(sr);
  }
  ~Voice(){
    #ifdef USE_SVF
    StateVariableFilter::destroy(vcf);
#else
    BiquadFilter::destroy(vcf);
#endif
    for(int i=0; i<3; ++i)
      PolyBlepOscillator::destroy(osc[i]);
    SineOscillator::destroy(sine);
    FloatArray::destroy(buffer);
  }
  void processMidi(MidiMessage msg){
    
  }
  void trigger(bool ison, size_t samples){
    env.trigger(ison, samples);
  }
  void setFrequency(float freq){
    for(int i=0; i<3; ++i){
      float frequency = freq * (1.0+TRIAD_SEMITONES[triad][i+inversion]*1/12.0);
      osc[i]->setFrequency(frequency);
    }
    sine->setFrequency(freq*0.5);
  }
  void setChord(int triad, int inversion=0){
    this->triad = triad;
    this->inversion = inversion;
  }
  void setDecay(float decay){
    env.setDecay(decay);
  }
  void setSub(float sub){
    subosc = sub;
  }
  void setFilter(float ffc, float fq){
    fc = ffc;
    q = fq;
  }

  void setShape(float shape){
    float pwm = shape > 1.0 ? 0.48*shape : 0.48;
    shape = shape > 1.0 ? 1.0 : shape;
    for(int i=0; i<3; ++i){
      osc[i]->setShape(shape);
      osc[i]->setPulseWidth(pwm);      
    }
  }
		
  // decay 150ms to 5s
  void generate(FloatArray out) {
    // vco
    sine->generate(out);
    out.multiply(subosc);
    for(int i=0; i<3; ++i){
      osc[i]->generate(buffer);
      out.add(buffer);
    }
    // envelope
    env.generate(buffer);
    // vcf
    buffer.multiply(fc);
    vcf->processLowPass(out, buffer, q, out);
    out.multiply(0.3);
  }
};

class ChorduroyPatch : public Patch {
private:
  SmoothFloat freq;
  VoltsPerOctave hz;
  Voice voice;
public:
  ChorduroyPatch() :
    freq(0.9, 440.0), voice(getSampleRate()) {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Cutoff");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_E, "Waveshape");
    registerParameter(PARAMETER_F, "Chord");
    registerParameter(PARAMETER_G, "Sub");
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.8);
    setParameterValue(PARAMETER_C, 0.2);
    setParameterValue(PARAMETER_D, 0.4);
    setParameterValue(PARAMETER_E, 0.25);
    setParameterValue(PARAMETER_F, 0.5);
    setParameterValue(PARAMETER_G, 0.5);
  }

  ~ChorduroyPatch(){
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
      // if(bid == PUSHBUTTON)
      // 	env.trigger(isButtonPressed(PUSHBUTTON), samples);
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      voice.setChord(0);
      voice.trigger(value, samples);
      break;
    case BUTTON_B:
      voice.setChord(1);
      voice.trigger(value, samples);
      break;
    case BUTTON_C:
      voice.setChord(2);
      voice.trigger(value, samples);
      break;
    case BUTTON_D:      
      voice.setChord(3);
      voice.trigger(value, samples);
      break;
    }
  }

  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    float shape = getParameterValue(PARAMETER_E)*2;
    float tune = getParameterValue(PARAMETER_A)*6.0 - 4.0;
    hz.setTune(tune);
    freq = hz.getFrequency(left[0]);
    float decay = getParameterValue(PARAMETER_D)*10;

    voice.setFrequency(freq);
    voice.setDecay(decay);
    voice.setShape(shape);

    // float fc = getParameterValue(PARAMETER_B)*8000+10;
    // float q = getParameterValue(PARAMETER_C)*5+0.1;
    // voice.setFilter(fc, q);

    voice.generate(left);
    left.tanh();
    right.copyFrom(left);
  }
};

#endif   // __ChorduroyPatch_hpp__
