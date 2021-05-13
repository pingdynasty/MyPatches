#ifndef __KordsPatch_hpp__
#define __KordsPatch_hpp__

#include "OpenWareLibrary.h"
#include "TapTempo.hpp"

// #define USE_SVF
#define VOICES 1

// #define FRAC_DELAY

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
   {0, 3, 7, 12, 12+3}, // Minor triad
   {0, 4, 7, 12, 12+4}, // Major triad
   {0, 3, 6, 12, 12+3}, // Diminished triad
   {0, 4, 8, 12, 12+4}, // Augmented triad
   {0, 1, 6, 12, 12+1}, // Viennese trichord
};

static const int TRIGGER_LIMIT = (1<<17);

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
  enum Parameters {
		   PARAMETER_CHORD = 1,
		   PARAMETER_DECAY,
		   PARAMETER_WAVESHAPE,
		   PARAMETER_FILTER_FC,
		   PARAMETER_FILTER_Q,
		   PARAMETER_SUBOSC
  };
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
    case PARAMETER_CHORD:
      setChord(value);
      break;
    case PARAMETER_DECAY:
      setDecay(value*14);
      break;
    case PARAMETER_WAVESHAPE:
      setShape(value*2);
      break;
    case PARAMETER_FILTER_FC:
      setFilter(value*8000+10, q);
      break;
    case PARAMETER_FILTER_Q:
      setFilter(fc, value*2+0.1);
      break;
    case PARAMETER_SUBOSC:
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

#ifdef FRAC_DELAY
typedef FractionalDelayProcessor FracDelayProcessor;
#else
typedef CrossFadingDelayProcessor FracDelayProcessor;
#endif

class KordsPatch : public Patch {
private:
  ChordVoices* voices;
  int basenote;

  // RampOscillator* lfo1;
  SineOscillator* lfo1;
  SineOscillator* lfo2;
  FracDelayProcessor* left_delay;
  FracDelayProcessor* right_delay;
  TapTempo<TRIGGER_LIMIT> tempo;

  FloatArray left_mix;
  FloatArray right_mix;
  FloatArray left_fb;
  FloatArray right_fb;
  SmoothFloat delay_samples;

  VoltsPerOctave hz;

public:
  KordsPatch() : tempo(getSampleRate()*60/120) {
    voices = ChordVoices::create(getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, ChordSignalGenerator::create(getBlockSize(), getSampleRate()));
    registerParameter(PARAMETER_A, "Decay");
    registerParameter(PARAMETER_B, "Tempo");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Dry/Wet");

    registerParameter(PARAMETER_AA, "Pitch");
    registerParameter(PARAMETER_AB, "Cutoff");
    registerParameter(PARAMETER_AC, "Resonance");
    registerParameter(PARAMETER_AD, "Waveshape");
    registerParameter(PARAMETER_AE, "Sub");

    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.8);
    setParameterValue(PARAMETER_C, 0.2);
    setParameterValue(PARAMETER_D, 0.4);
    setParameterValue(PARAMETER_AA, 0.0);
    setParameterValue(PARAMETER_AB, 0.99);
    setParameterValue(PARAMETER_AC, 0.2);
    setParameterValue(PARAMETER_AD, 0.0);
    setParameterValue(PARAMETER_AE, 0.5);

    // delay
#ifdef FRAC_DELAY
    left_delay = FracDelayProcessor::create(TRIGGER_LIMIT);
    right_delay = FracDelayProcessor::create(TRIGGER_LIMIT*2);
#else
    left_delay = FracDelayProcessor::create(TRIGGER_LIMIT, getBlockSize());
    right_delay = FracDelayProcessor::create(TRIGGER_LIMIT*2, getBlockSize());
#endif
    left_mix = FloatArray::create(getBlockSize());
    right_mix = FloatArray::create(getBlockSize());
    left_fb = FloatArray::create(getBlockSize());
    right_fb = FloatArray::create(getBlockSize());
    left_delay->setDelay(getSampleRate()*0.25);
    right_delay->setDelay(getSampleRate()*0.5);
    delay_samples.lambda = 0.96;

    // lfos
    // lfo1 = RampOscillator::create(getBlockRate());
    lfo1 = SineOscillator::create(getBlockRate());
    lfo2 = SineOscillator::create(getBlockRate());
    registerParameter(PARAMETER_F, "LFO1>");
    registerParameter(PARAMETER_G, "LFO2>");
  }
  ~KordsPatch(){
    for(int i=0; i<VOICES; ++i)
      ChordSignalGenerator::destroy(voices->getVoice(i));
    ChordVoices::destroy(voices);
    FloatArray::destroy(left_mix);
    FloatArray::destroy(right_mix);
    SineOscillator::destroy(lfo1);
    SineOscillator::destroy(lfo2);
    FracDelayProcessor::destroy(left_delay);
    FracDelayProcessor::destroy(right_delay);
  }
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_A:
      if(value){
	voices->setParameter(ChordSignalGenerator::PARAMETER_CHORD, 0);
	voices->noteOn(MidiMessage::note(0, basenote, 100));
      }
      break;
    case BUTTON_B:
      tempo.trigger(value, samples);
      if(value)
      	lfo1->reset();
      break;
    case BUTTON_C:
      if(value){
	voices->setParameter(ChordSignalGenerator::PARAMETER_CHORD, 2);
	voices->noteOn(MidiMessage::note(0, basenote, 100));
      }
      break;
    case BUTTON_D:      
      if(value){
	voices->setParameter(ChordSignalGenerator::PARAMETER_CHORD, 3);
	voices->noteOn(MidiMessage::note(0, basenote, 100));
      }
      break;
    }
  }

  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setMidiNote(getParameterValue(PARAMETER_AA)*12*5+42);
    basenote = round(hz.hertzToNote(hz.getFrequency(left[0])));
    voices->setParameter(ChordSignalGenerator::PARAMETER_FILTER_FC, getParameterValue(PARAMETER_AB));
    voices->setParameter(ChordSignalGenerator::PARAMETER_FILTER_Q, getParameterValue(PARAMETER_AC));
    voices->setParameter(ChordSignalGenerator::PARAMETER_DECAY, getParameterValue(PARAMETER_A));
    voices->setParameter(ChordSignalGenerator::PARAMETER_SUBOSC, getParameterValue(PARAMETER_AE));
    voices->setParameter(ChordSignalGenerator::PARAMETER_WAVESHAPE, getParameterValue(PARAMETER_AD));
    voices->generate(left);
    left.tanh();
    right.copyFrom(left);

    // delay
    tempo.clock(getBlockSize());
    tempo.setSpeed(getParameterValue(PARAMETER_B)*4096);
    float feedback = getParameterValue(PARAMETER_C)*1.1;
    float mix = getParameterValue(PARAMETER_D);
    delay_samples = tempo.getSamples();
    left_fb.multiply(feedback);
    right_fb.multiply(feedback);
    left_fb.add(left);
    right_fb.add(right);
    left_delay->smooth(left_fb, left_mix, delay_samples);
    right_delay->smooth(right_fb, right_mix, delay_samples*1.5);
    left.multiply(1-mix);
    right.multiply(1-mix);
    left_fb.copyFrom(right_mix);
    right_fb.copyFrom(left_mix);
    left_mix.multiply(mix);
    right_mix.multiply(mix);
    left.add(left_mix);
    right.add(right_mix);

    // Tempo synced LFO
    float lfoFreq = getSampleRate()/delay_samples;
    lfo1->setFrequency(lfoFreq);
    lfo2->setFrequency(lfoFreq);
    setParameterValue(PARAMETER_F, lfo1->generate()*0.5+0.5);
    setParameterValue(PARAMETER_G, lfo2->generate()*0.5+0.5);
    setButton(PUSHBUTTON, lfo1->getPhase() < 0.5);
}
};

#endif   // __KordsPatch_hpp__
