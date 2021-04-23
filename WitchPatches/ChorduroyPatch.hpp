#ifndef __ChorduroyPatch_hpp__
#define __ChorduroyPatch_hpp__

#include "Patch.h"
#include "OpenWareLibrary.h"

// #define USE_SVF
#define VOICES 2

/**
 * Triple tone generator that produces a triad chord that tracks the frequency of the input audio signal.
 * Select between three inversions of each triad: Diminished, Minor, Major and Augmented.
 * ref: https://en.wikipedia.org/wiki/Triad_(music)
 */
#define SEMI    (1.0/12)
#define WHOLE   (2.0/12)
#define TRIADS 5

#if VOICES == 1
#define GAINFACTOR 0.5
#elif VOICES == 2
#define GAINFACTOR 0.3
#elif VOICES == 3
#define GAINFACTOR 0.3
#elif VOICES == 4
#define GAINFACTOR 0.2
#endif

static const float TRIAD_SEMITONES[TRIADS][5] = {
   {0, 1, 6, 12, 12+1}, // Viennese trichord
   {0, 3, 6, 12, 12+3}, // Diminishied triad
   {0, 3, 7, 12, 12+3}, // Minor triad
   {0, 4, 7, 12, 12+4}, // Major triad
   {0, 4, 8, 12, 12+4}  // Augmented triad
};

class ChordSignalGenerator : public AbstractSynth {
private:
  PolyBlepOscillator* osc[3];
  SineOscillator* sine;
#ifdef USE_SVF
  StateVariableFilter* vcf;
#else
  BiquadFilter* vcf;
#endif
  ExponentialDecayEnvelope* env;
  FloatArray buffer;
  float subosc = 1.0;
  float fc = 18000;
  float q = 4;
  int triad = 0;
  int inversion = 0;
  float gain = GAINFACTOR;
public:
  ChordSignalGenerator(FloatArray buffer, float sr): buffer(buffer) {
#ifdef USE_SVF
    vcf = StateVariableFilter::create(sr);
#else
    vcf = BiquadFilter::create(sr, 4);
#endif
    env = ExponentialDecayEnvelope::create(sr);
    for(int i=0; i<3; ++i){
      osc[i] = PolyBlepOscillator::create(sr);
      osc[i]->setShape(0.0f); //  0 for saw, 1 for square wave
      // osc[i]->setPulseWidth(pwm);
    }
    sine = SineOscillator::create(sr);
  }
  ~ChordSignalGenerator(){
    #ifdef USE_SVF
    StateVariableFilter::destroy(vcf);
#else
    BiquadFilter::destroy(vcf);
#endif
    for(int i=0; i<3; ++i)
      PolyBlepOscillator::destroy(osc[i]);
    SineOscillator::destroy(sine);
    FloatArray::destroy(buffer);
    ExponentialDecayEnvelope::destroy(env);
  }
  void setGain(float gain){
    this->gain = gain*GAINFACTOR;
  }
  void trigger(){
    env->trigger();
  }
  void gate(bool state){
    if(state)
      env->trigger();
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
    env->setDecay(decay);
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
  float generate() {
    float y = sine->generate()*subosc;
    for(int i=0; i<3; ++i)
      y += osc[i]->generate();
    float cutoff = env->generate()*fc;
    vcf->setLowPass(cutoff, q);
    return vcf->process(y)*gain;
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
    env->generate(buffer);
    // vcf
    buffer.multiply(fc); // buffer holds our enveloped cutoff values
    vcf->processLowPass(out, buffer, q, out);
    out.multiply(gain);
  }
  void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case 1:
      setChord(value);
      break;
    case 2:
      setDecay(value*14);
      break;
    case 3:
      setShape(value*2);
      break;
    case 4:
      setFilter(value*8000+10, q);
      break;
    case 5:
      setFilter(fc, value*2+0.1);
      break;
    case 6:
      setSub(value*2);
      break;
    }
  }
  static ChordSignalGenerator* create(size_t blocksize, float sr){
    return new ChordSignalGenerator(FloatArray::create(blocksize), sr);
  }
  static void destroy(ChordSignalGenerator* obj){
    FloatArray::destroy(obj->buffer);
    delete obj;
  }
};

#if VOICES == 1
typedef MonophonicSignalGenerator<ChordSignalGenerator> ChordVoices;
#else
typedef PolyphonicSignalGenerator<ChordSignalGenerator, VOICES> ChordVoices;
#endif

class ChorduroyPatch : public Patch {
private:
  ChordVoices* voices;
  int basenote = 60;
public:
  ChorduroyPatch(){
    voices = ChordVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, ChordSignalGenerator::create(getBlockSize(), getSampleRate()));
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Cutoff");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Envelope");
    registerParameter(PARAMETER_E, "Waveshape");
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.8);
    setParameterValue(PARAMETER_C, 0.2);
    setParameterValue(PARAMETER_D, 0.4);
    setParameterValue(PARAMETER_E, 0.25);
  }
  ~ChorduroyPatch(){
    for(int i=0; i<VOICES; ++i)
      ChordSignalGenerator::destroy(voices->getVoice(i));
    ChordVoices::destroy(voices);
  }
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(value){
      switch(bid){
      case PUSHBUTTON:
      case BUTTON_A:
	voices->setParameter(1, 0);
	voices->noteOn(MidiMessage::note(0, basenote, 100));
	break;
      case BUTTON_B:
	voices->setParameter(1, 1);
	voices->noteOn(MidiMessage::note(0, basenote, 100));
	break;
      case BUTTON_C:
	voices->setParameter(1, 2);
	voices->noteOn(MidiMessage::note(0, basenote, 100));
	break;
      case BUTTON_D:      
	voices->setParameter(1, 3);
	voices->noteOn(MidiMessage::note(0, basenote, 100));
	break;
      }
    }
  }

  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    basenote = getParameterValue(PARAMETER_A)*12*5+20;    
    voices->setParameter(4, getParameterValue(PARAMETER_B));
    voices->setParameter(5, getParameterValue(PARAMETER_C));
    voices->setParameter(2, getParameterValue(PARAMETER_D));
    voices->setParameter(6, getParameterValue(PARAMETER_E));
    voices->generate(left);
    left.tanh();
    right.copyFrom(left);
  }
};

#endif   // __ChorduroyPatch_hpp__
