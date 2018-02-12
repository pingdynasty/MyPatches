#ifndef __PolyOpPatch_hpp__
#define __PolyOpPatch_hpp__

#include "Patch.h"
#include "Envelope.h"
#include "VoltsPerOctave.h"
#include "BiquadFilter.h"
#include "SineOscillator.h"
// #include "Oscillators.hpp"
// #include "MidiVoiceAllocator.hpp"

// semitones = 12*log(ratio)/log(2)
// ratio = pow(2, semitones/12)
				       
#define MIDI_VOICES 4

class MidiVoice {
public:
  virtual void noteOn(uint8_t note, uint8_t velocity, uint16_t delay){}
  virtual void noteOff(uint8_t note, uint16_t delay){}
  virtual void pitchBend(int16_t bend, uint16_t delay){}
  virtual void controlChange(uint8_t cc, uint8_t value, uint16_t delay){}
};

class Operator : public Oscillator {
private:
public:
  AdsrEnvelope env;
  SineOscillator osc;
public:
  float index = 1.0;
  float ratio = 1.0;
  float offset = 0.0;
  Operator() : env(48000), osc(48000) {
    env.setSustain(1.0);
    env.setDecay(0.0);
    env.setRelease(0.0);
  }
  void setSampleRate(float value){
    osc.setSampleRate(value);
    env.setSampleRate(value);
  }
  void setFrequency(float f){
    static SmoothFloat freq;
    freq = f;
    osc.setFrequency(freq*ratio+offset);
  }
  void setEnvelope(float a, float d, float s, float r){
    env.setAttack(a);
    env.setDecay(d);
    env.setSustain(s);
    env.setRelease(r);
  }
  float getNextSample(){
    return env.getNextSample()*index*osc.getNextSample();
  }
  float getNextSample(float fm){
    return env.getNextSample()*index*osc.getNextSample(fm);
  }
  void gate(bool state, uint16_t samples){
    env.gate(state, samples);
  }
  void setEnvelope(float shape){
    float df = shape*4;
    int di = (int)df;
    float attack, release;
    switch(di){
      // a/d
    case 0: // l/s
      attack = 1.0-df;
      release = 0.0;
      break;
    case 1: // s/s
      attack = 0.0;
      release = df-1;
      break;
    case 2: // s/l
      attack = df-2;
      release = 1.0;
      break;
    case 3: // l/l
      attack = 1.0;
      release = 1.0;
      break;
    }
    env.setAttack(attack);
    env.setRelease(release);
  }
};

  const float RATIO_DEFAULT = 0.25;
  const float RATIO_MIN = 0.5;
  const float RATIO_RANGE = 7.5;
  const float OFFSET_DEFAULT = 0.0;
  const float INDEX_DEFAULT = 0.5;
  const float ENVELOPE_DEFAULT = 0.5;

class FourOpVoice : public Oscillator, MidiVoice {
public:
  Operator ops[4];
  int algo;
  float pb, gain;
  uint8_t note;
public:
  FourOpVoice(){
    reset();
  }
  void reset(){
    algo = 0;
    note = 60;
    pb = 0;
    gain = 0.5;
    for(int i=0; i<4; ++i){
      ops[i].index = INDEX_DEFAULT;
      ops[i].ratio = RATIO_MIN + RATIO_DEFAULT*RATIO_RANGE;
      ops[i].offset = OFFSET_DEFAULT;
      ops[i].setEnvelope(ENVELOPE_DEFAULT);
    }
  }
  void gate(bool state, uint16_t samples){
    ops[0].gate(state, samples);
    ops[1].gate(state, samples);
    ops[2].gate(state, samples);
    ops[3].gate(state, samples);
  }
  void setSampleRate(float value){
    ops[0].setSampleRate(value);
    ops[1].setSampleRate(value);
    ops[2].setSampleRate(value);
    ops[3].setSampleRate(value);
  }  
  void setParameters(uint8_t op, float index, float ratio, float offset, float env){
    ops[op].index = index;
    ops[op].ratio = RATIO_MIN + ratio*RATIO_RANGE;
    ops[op].offset = offset;
    ops[op].setEnvelope(env);
  }
  void setFrequency(float freq){
    ops[0].setFrequency(freq);
    ops[1].setFrequency(freq);
    ops[2].setFrequency(freq);
    ops[3].setFrequency(freq);
  }
  float getNextSample(){
    return 0.0f;
  }
  void addSamples(FloatArray samples){
    updateFrequency(note);
    switch(algo){
    case 0:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo1();
      break;
    case 1:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo2();
      break;
    case 2:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo3();
      break;
    case 3:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo4();
      break;
    case 4:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo5();
      break;
    case 5:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo6();
      break;
    case 6:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo7();
      break;
    case 7:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo8();
      break;
    case 8:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo9();
      break;
    case 9:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo10();
      break;
    case 10:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo11();
      break;
    case 11:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo12();
      break;
    case 12:
      for(int i=0; i<samples.getSize(); ++i)
	samples[i] += algo13();
      break;
    }
    samples.multiply(gain);
  }

  float algo1(){
    // 0>1>2>3>
    float sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    sample = ops[2].getNextSample(sample);
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo2(){
    // 0>
    // 1>2>3>
    float sample = ops[0].getNextSample();
    sample += ops[1].getNextSample();
    sample = ops[2].getNextSample(sample);
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo3(){
    // 1>2>
    //   0>3>
    float sample = ops[1].getNextSample();
    sample = ops[2].getNextSample(sample);
    sample += ops[0].getNextSample();
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo4(){
    // 0>1
    //  >2>3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    out += ops[2].getNextSample(sample);
    out = ops[3].getNextSample(out);
    return out;
  }
  float algo5(){
    // 0>1>2>
    //    >3>
    float sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    float out = ops[2].getNextSample(sample);
    out += ops[3].getNextSample(sample);
    return out;
  }
  float algo6(){
    // 0>1>2>
    //     3>
    float sample = ops[0].getNextSample();
    sample = ops[1].getNextSample(sample);
    float out = ops[2].getNextSample(sample);
    out += ops[3].getNextSample();
    return out;
  }
  float algo7(){
    // 0>
    // 1>
    // 2>3>
    float sample = ops[0].getNextSample();
    sample += ops[1].getNextSample();
    sample += ops[2].getNextSample();
    sample = ops[3].getNextSample(sample);
    return sample;
  }
  float algo8(){
    // 0>1>
    // 2>3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    sample = ops[2].getNextSample();
    out += ops[3].getNextSample(sample);
    return out;
  }
  float algo9(){
    // 0>1>
    //  >2>
    //  >3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    out += ops[2].getNextSample(sample);
    out += ops[3].getNextSample(sample);
    return out;
  }
  float algo10(){
    // 0>1>
    //   2>
    //   3>
    float sample = ops[0].getNextSample();
    float out = ops[1].getNextSample(sample);
    out += ops[2].getNextSample();
    out += ops[3].getNextSample();
    return out;
  }
  float algo11(){
    // 0>
    // 1>
    // 2>
    // 3>
    float out = ops[0].getNextSample();
    out += ops[1].getNextSample();
    out += ops[2].getNextSample();
    out += ops[3].getNextSample();
    return out;
  }
  float algo12(){
    // 3>3>
    // 0>1>2>
    static float fb = 0.0f;
    fb = ops[3].getNextSample(fb);
    // tx81z appears to have global feedback(op4) parameter
    float out = ops[0].getNextSample();
    out = ops[1].getNextSample(out);
    out = ops[2].getNextSample(out+fb);
    return out;
  }
  float algo13(){
    // 3>3>2>
    //     0>1>
    static float fb = 0.0f;
    float out = ops[0].getNextSample();
    fb = ops[3].getNextSample(fb);
    out += ops[2].getNextSample(fb);
    out = ops[1].getNextSample(out);
    return out;
  }
  void noteOn(uint8_t note, uint8_t velocity, uint16_t samples){
    pb = 0;
    updateFrequency(note);
    gain = (velocity*velocity)/16129.0f + 0.2;
    // setFrequency(freq);
    gate(true, samples);    
    // debugMessage("note on", note, velocity);
  }
  void noteOff(uint8_t note, uint16_t samples){
    gate(false, samples);
    // debugMessage("note off", note);
  }
  void updateFrequency(uint8_t n){
    note = n;
    float freq = 440.0f*exp2f((note-69 + pb*2)/12.0);    
    setFrequency(freq); // update frequency in case ratio or offset has changed
  }
  void pitchBend(int16_t bend, uint16_t delay){
    // pb = bend/8192.0f;
    pb = bend/2048.0f;
    // debugMessage("pb", pb);
  }
  void controlChange(uint8_t cc, uint8_t value, uint16_t delay){
  }
  static FourOpVoice* create(float sr){
    FourOpVoice* voice = new FourOpVoice();
    voice->setSampleRate(sr);
    return voice;
  }
  static FourOpVoice* destroy(FourOpVoice* voice){
    delete voice;
  }
};

class FourOpVoiceAllocator { // : public MidiVoiceAllocator {
public:
  FourOpVoice* voice[MIDI_VOICES];

  void getSamples(FloatArray samples){
    samples.clear();
    for(int i=0; i<MIDI_VOICES; ++i)
      voice[i]->addSamples(samples);
      // voice[i]->getSamples(samples);
    samples.multiply(1.0/MIDI_VOICES);
  }
  void setParameters(uint8_t op, float index, float ratio, float offset, float env){
    for(int i=0; i<MIDI_VOICES; ++i)
      voice[i]->setParameters(op, index, ratio, offset, env);
  }
  void setFrequency(float freq){
    for(int i=0; i<MIDI_VOICES; ++i)
      voice[i]->setFrequency(freq);
  }
  void setAlgorithm(uint8_t algo){
    algo = max(0, min(12, algo));
    for(int i=0; i<MIDI_VOICES; ++i)
      voice[i]->algo = algo;
  }

#ifdef MIDI_CHANNEL_ALLOCATION
  FourOpVoiceAllocator(){}
  ////
  void processMidi(MidiMessage& msg){
    uint16_t samples = 0;
    uint8_t ch = msg.getChannel();
    if(msg.isNoteOn()){
      if(ch < MIDI_VOICES)
	voice[ch]->noteOn(msg.getNote(), (uint16_t)msg.getVelocity()<<5, samples);
    }else if(msg.isNoteOff()){
      if(ch < MIDI_VOICES)
	voice[ch]->noteOff(msg.getNote(), samples);
    }else if(msg.isPitchBend()){
      if(ch < MIDI_VOICES)
	voice[ch]->pitchBend(msg.getPitchBend(), samples);
    }else if(msg.isControlChange()){
      switch(msg.getControllerNumber()){
      case MIDI_ALL_NOTES_OFF:
	allNotesOff(samples);
	break;
      default:
	if(ch < MIDI_VOICES)
	  voice[ch]->controlChange(msg.getControllerNumber(), msg.getControllerValue(), samples);
      }
    }
  }

#else

  static const uint8_t EMPTY = 0xff;
  static const uint16_t TAKEN = 0xffff;
  uint8_t notes[MIDI_VOICES];
  uint16_t allocation[MIDI_VOICES];
  uint16_t allocated;
  FourOpVoiceAllocator(){
      for(int i=0; i<MIDI_VOICES; ++i){
      // voice[i] = SynthVoice::create(sr);
      notes[i] = 69; // middle A, 440Hz
      allocation[i] = 0;
    }
  }
  void processMidi(MidiMessage& msg){    
    uint16_t samples = 0;
    if(msg.isNoteOn()){
      noteOn(msg.getNote(), (uint16_t)msg.getVelocity()<<5, samples);
    }else if(msg.isNoteOff()){
      noteOff(msg.getNote(), samples);
    }else if(msg.isPitchBend()){
      for(int i=0; i<MIDI_VOICES; ++i)
	voice[i]->pitchBend(msg.getPitchBend(), samples);
    // }else if(msg.isControlChange()){
    //   switch(msg.getControllerNumber()){
    //   case MIDI_ALL_NOTES_OFF:
    // 	allNotesOff(samples);
    // 	break;
    //   default:
    // 	break;
    //   }
    }
  }
	     
  void take(uint8_t ch, uint8_t note, uint16_t velocity, uint16_t samples){
    notes[ch] = note;
    allocation[ch] = TAKEN;
    voice[ch]->noteOn(note, velocity, samples);
    // voice[ch]->setGain(velocity/(4095.0f*(MIDI_VOICES/2)));
    // voice[ch]->setGate(true, samples);
  }

  void release(uint8_t ch, uint16_t samples){
    uint8_t note = notes[ch];
    // notes[ch] = EMPTY;
    allocation[ch] = ++allocated;
    // voice[ch]->setGate(false, samples);
    voice[ch]->noteOff(note, samples);
  }

  void noteOn(uint8_t note, uint16_t velocity, uint16_t samples){
    uint16_t minval = allocation[0];
    uint8_t minidx = 0;
    // take oldest free voice, to allow voices to ring out
    for(int i=1; i<MIDI_VOICES; ++i){
      if(notes[i] == note){
	minidx = i;
	break;
      }
      if(allocation[i] < minval){
	minidx = i;
	minval = allocation[i];
      }
    }
    // take oldest voice
    take(minidx, note, velocity, samples);
    debugMessage("idx/note/vel", minidx, note, velocity);
  }

  void noteOff(uint8_t note, uint16_t samples){
    for(int i=0; i<MIDI_VOICES; ++i)
      if(notes[i] == note)
	release(i, samples);
  }

#endif

  void allNotesOff(uint16_t samples){
    for(int i=0; i<MIDI_VOICES; ++i)
      voice[i]->noteOff(-1, samples);
  }

  //////
  static FourOpVoiceAllocator* create(float sr){
    FourOpVoiceAllocator* allocator = new FourOpVoiceAllocator();
    for(int i=0; i<MIDI_VOICES; ++i)
      allocator->voice[i] = FourOpVoice::create(sr);
    return allocator;
  }
  static FourOpVoiceAllocator* destroy(FourOpVoiceAllocator* allocator){
    delete allocator;
  }
};

class PolyOpPatch : public Patch {
private:  
  FourOpVoiceAllocator* allocator;
  // VoltsPerOctave hz;
  BiquadFilter* lp;
public:
  PolyOpPatch() {
    allocator = FourOpVoiceAllocator::create(getSampleRate());

    // algo.setSampleRate(getSampleRate());
    registerParameter(PARAMETER_A, "Op1:Semis");
    registerParameter(PARAMETER_B, "Op1:Amount");
    registerParameter(PARAMETER_AA, "Op1:Shape");
    registerParameter(PARAMETER_AB, "Op1:Env>");
    setParameterValue(PARAMETER_A, RATIO_DEFAULT);
    setParameterValue(PARAMETER_B, INDEX_DEFAULT);
    setParameterValue(PARAMETER_AA, ENVELOPE_DEFAULT);

    registerParameter(PARAMETER_C, "Op2:Semis");
    registerParameter(PARAMETER_D, "Op2:Amount");
    registerParameter(PARAMETER_AC, "Op2:Shape");
    registerParameter(PARAMETER_AD, "Op2:Env>");
    setParameterValue(PARAMETER_C, RATIO_DEFAULT*2);
    setParameterValue(PARAMETER_D, INDEX_DEFAULT);
    setParameterValue(PARAMETER_AC, ENVELOPE_DEFAULT);

    registerParameter(PARAMETER_E, "Op3:Semis");
    registerParameter(PARAMETER_F, "Op3:Amount");
    registerParameter(PARAMETER_AE, "Op3:Shape");
    registerParameter(PARAMETER_AF, "Op3:Env>");
    setParameterValue(PARAMETER_E, RATIO_DEFAULT*4);
    setParameterValue(PARAMETER_F, INDEX_DEFAULT);
    setParameterValue(PARAMETER_AE, ENVELOPE_DEFAULT);

    registerParameter(PARAMETER_G, "Op4:Semis");
    registerParameter(PARAMETER_H, "Op4:Amount");
    registerParameter(PARAMETER_AG, "Op4:Shape");
    registerParameter(PARAMETER_AH, "Op4:Env>");
    setParameterValue(PARAMETER_G, RATIO_DEFAULT*8);
    setParameterValue(PARAMETER_H, INDEX_DEFAULT);
    setParameterValue(PARAMETER_AG, ENVELOPE_DEFAULT);

    registerParameter(PARAMETER_BA, "Algorithm");
    lp = StereoBiquadFilter::create(2);
    lp->setLowPass(0.8, FilterStage::BUTTERWORTH_Q);
  }
  ~PolyOpPatch(){
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
  //   if(bid >= MIDI_NOTE_BUTTON){
  //     uint8_t note = bid - MIDI_NOTE_BUTTON;
  //     if(value)
  // 	algo.noteOn(note, value, samples);
  //     else
  // 	algo.noteOff(samples);
    if(bid == PUSHBUTTON && value == 0){
    //     algo.gate(value, samples);
      allocator->allNotesOff(samples);
    }
  }
  void processMidi(MidiMessage& msg){
    // debugMessage("port/channel/status", msg.getPort(), msg.getChannel(), msg.getStatus());
    // if(msg.getPort() != 1) // prevent double triggering on TouchKeys / Impulse
    //   return;
    allocator->processMidi(msg);
  }
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    allocator->setAlgorithm(getParameterValue(PARAMETER_BA)*13);
    float index = 0.5;
    float ratio = 2.0;
    float offset = 0.0;
    float env = 0.6;
    // ratio = getParameterValue(PARAMETER_A);
    // ratio = pow(2, semitones/12)
    ratio = exp2f(int(getParameterValue(PARAMETER_A)*36-12)/12.0);
    index = getParameterValue(PARAMETER_B);
    env = getParameterValue(PARAMETER_AA);
    allocator->setParameters(0, index, ratio, offset, env);
    ratio = exp2f(int(getParameterValue(PARAMETER_C)*36-12)/12.0);
    index = getParameterValue(PARAMETER_D);
    env = getParameterValue(PARAMETER_AC);
    allocator->setParameters(1, index, ratio, offset, env);
    ratio = exp2f(int(getParameterValue(PARAMETER_E)*36-12)/12.0);
    index = getParameterValue(PARAMETER_F);
    env = getParameterValue(PARAMETER_AE);
    allocator->setParameters(2, index, ratio, offset, env);
    ratio = exp2f(int(getParameterValue(PARAMETER_G)*36-12)/12.0);
    index = getParameterValue(PARAMETER_H);
    env = getParameterValue(PARAMETER_AG);
    allocator->setParameters(3, index, ratio, offset, env);
    allocator->getSamples(left);
    right.copyFrom(left);
    lp->process(left);

    setParameterValue(PARAMETER_AB, allocator->voice[0]->ops[0].env.getLevel());
    setParameterValue(PARAMETER_AD, allocator->voice[0]->ops[1].env.getLevel());
    setParameterValue(PARAMETER_AF, allocator->voice[0]->ops[2].env.getLevel());
    setParameterValue(PARAMETER_AH, allocator->voice[0]->ops[3].env.getLevel());
  }
};

#endif   // __PolyOpPatch_hpp__
