#ifndef __ChorduroyPatch_hpp__
#define __ChorduroyPatch_hpp__

#include "Patch.h"
#include "OpenWareLibrary.h"

// add sub oscillator
// exponential decay envelope

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

#define USE_SVF

class ChorduroyPatch : public Patch {
private:
  PolyBlepOscillator* osc[3];
  SineOscillator* sine;
  SmoothFloat freq;
  FloatArray output;
#ifdef USE_SVF
  StateVariableFilter* vcf;
#else
  BiquadFilter* vcf;
#endif
  VoltsPerOctave hz;
  AdsrEnvelope env;
public:
  ChorduroyPatch() :
    freq(0.9, 440.0) {
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
#ifdef USE_SVF
    vcf = StateVariableFilter::create(getSampleRate());
#else
    vcf = BiquadFilter::create(getSampleRate(), 4);
#endif
    output = FloatArray::create(getBlockSize());
    for(int i=0; i<3; ++i)
      osc[i] = PolyBlepOscillator::create(getSampleRate());

    sine = SineOscillator::create(getSampleRate());
  }      
  ~ChorduroyPatch(){
    FloatArray::destroy(output);
#ifdef USE_SVF
    StateVariableFilter::destroy(vcf);
#else
    BiquadFilter::destroy(vcf);
#endif
    for(int i=0; i<3; ++i)
      PolyBlepOscillator::destroy(osc[i]);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
      // if(bid == PUSHBUTTON)
      // 	env.trigger(isButtonPressed(PUSHBUTTON), samples);
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      setParameterValue(PARAMETER_F, 0.0);
      env.trigger(value, samples);
      break;
    case BUTTON_B:
      setParameterValue(PARAMETER_F, 1.0/TRIADS);
      env.trigger(value, samples);
      break;
    case BUTTON_C:
      setParameterValue(PARAMETER_F, 2.0/TRIADS);
      env.trigger(value, samples);
      break;
    case BUTTON_D:      
      setParameterValue(PARAMETER_F, 3.0/TRIADS);
      env.trigger(value, samples);
      break;
    }
    env.setAttack(0);
    env.setSustain(0);
    env.setRelease(0);
  }

  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    // vco
    float shape = getParameterValue(PARAMETER_E)*2;
    float pwm = shape > 1.0 ? 0.48*shape : 0.48;
    shape = shape > 1.0 ? 1.0 : shape;
    int triad = getParameterValue(PARAMETER_F)*TRIADS;
    int inversion = (getParameterValue(PARAMETER_F)*TRIADS-triad)*3;
    // debugMessage("triad/inversion", triad, inversion);
    float tune = getParameterValue(PARAMETER_A)*6.0 - 4.0;
    hz.setTune(tune);
    freq = hz.getFrequency(left[0]);
    sine->setFrequency(freq*0.5);
    sine->generate(left);
    left.multiply(getParameterValue(PARAMETER_H));    
    for(int i=0; i<3; ++i){
      float frequency = freq * (1.0+TRIAD_SEMITONES[triad][i+inversion]*1/12.0);
      osc[i]->setShape(shape);
      osc[i]->setPulseWidth(pwm);
      osc[i]->setFrequency(frequency);
      osc[i]->generate(output);
      output.multiply(0.20);
      left.add(output);
    }

    // envelope
    float decay = getParameterValue(PARAMETER_D);
    env.setDecay(decay);
    env.generate(output);

    // vcf
    float fc = getParameterValue(PARAMETER_B)*8000+10;
    float q = getParameterValue(PARAMETER_C)*5+0.1;
    output.multiply(fc);
    vcf->processLowPass(left, output, q, left);

    left.tanh();
    right.copyFrom(left);
  }
};

#endif   // __ChorduroyPatch_hpp__
