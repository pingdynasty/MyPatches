#ifndef __KickBoxPatch_hpp__
#define __KickBoxPatch_hpp__

#include "MonochromeScreenPatch.h"
#include "VoltsPerOctave.h"
#include "OpenWareLibrary.h"
#include "Cymbal.hpp"
#include "BassDrum.hpp"
#include "Sequence.h"
#include "SynthVoice.hpp"
#include "MonochromeAudioDisplay.hpp"

class KickBoxPatch : public MonochromeScreenPatch {
private:
  SynthVoice* voice;
  CymbalVoice* hat;
  BassDrumVoice* kick;
  Sequence<uint32_t> seq[5];
  uint8_t midinote;
public:
  KickBoxPatch(){
    // OSC
    registerParameter(PARAMETER_A, "Base Note");
    registerParameter(PARAMETER_B, "Envelope");
    registerParameter(PARAMETER_AA, "Root");
    registerParameter(PARAMETER_AB, "Envelope>");

    setParameterValue(PARAMETER_A, 0.3);
    setParameterValue(PARAMETER_B, 0.3);
    setParameterValue(PARAMETER_AA, 0.126);

    // FILT
    registerParameter(PARAMETER_C, "Cutoff");
    registerParameter(PARAMETER_D, "Resonance");
    registerParameter(PARAMETER_AC, "3rds");
    registerParameter(PARAMETER_AD, "5ths");

    setParameterValue(PARAMETER_C, 0.7);
    setParameterValue(PARAMETER_D, 0.2);

    // HAT
    registerParameter(PARAMETER_E, "Hat Tone");
    registerParameter(PARAMETER_F, "Hat Decay");
    registerParameter(PARAMETER_AE, "Hat Beats");
    registerParameter(PARAMETER_AF, "LFO1>");

    setParameterValue(PARAMETER_E, 0.8);
    setParameterValue(PARAMETER_F, 0.25);
    setParameterValue(PARAMETER_AE, 0.51);

    // KICK
    registerParameter(PARAMETER_G, "Kick Tone");
    registerParameter(PARAMETER_H, "Kick Decay");
    registerParameter(PARAMETER_AG, "Kick Beats");
    registerParameter(PARAMETER_AH, "LFO2>");

    setParameterValue(PARAMETER_G, 0.2);
    setParameterValue(PARAMETER_H, 0.25);
    setParameterValue(PARAMETER_AG, 0.26);

    registerParameter(PARAMETER_BA, "Waveshape");
    // registerParameter(PARAMETER_BB, "Kick Snap");
    registerParameter(PARAMETER_BB, "Tempo");
    registerParameter(PARAMETER_BC, "LFO1");
    registerParameter(PARAMETER_BD, "LFO2");

    setParameterValue(PARAMETER_BB, 0.5);
    setParameterValue(PARAMETER_BC, 0.2);
    setParameterValue(PARAMETER_BD, 0.4);
    
    hat = new CymbalVoice(getSampleRate());
    kick = new BassDrumVoice(getSampleRate());
    kick->setSnap(0.2);

    voice = SynthVoice::create(getSampleRate());
    midinote = 69;
  }

  ~KickBoxPatch(){
    SynthVoice::destroy(voice);
    delete hat;
    delete kick;
  }

  void noteOn(uint8_t note, uint16_t velocity, uint16_t samples){
    midinote = note;
    voice->setGain(velocity/(4095.0f*2));
    voice->setGate(true, samples);
  }

  void noteOff(uint8_t note, uint16_t samples){
    voice->setGate(false, samples);
  }

  void setParameters(float shape, float fc, float q, float attack, float release, float pb){
    float freq = 440.0f*exp2f((midinote-69 + pb*2)/12.0);
    voice->setFrequency(freq);
    voice->setWaveshape(shape);
    voice->setFilter(fc, q);
    voice->setEnvelope(attack, release);
  }

  void processMidi(MidiMessage msg){
    switch (msg.getStatus()) {
    case NOTE_OFF:
      break;
    case NOTE_ON:
      setParameterValue(PARAMETER_A, (msg.getNote()-20)/80.0); // set basenote parameter
      break;
    }
  }

  // void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
  //   if(bid >= MIDI_NOTE_BUTTON){
  //     uint8_t note = bid-MIDI_NOTE_BUTTON;
  //     // if(value)
  //     // 	noteOn(note, value, samples);
  //     // else
  //     // 	noteOff(note, samples);
  //     if(value)
  // 	setParameterValue(PARAMETER_A, (note-20)/80.0); // set basenote parameter
  //   }
  // }

  void processAudio(AudioBuffer& buffer){
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    // synth voice
    float shape = getParameterValue(PARAMETER_BA)*2;
    float df = getParameterValue(PARAMETER_B)*4;
    float cutoff = getParameterValue(PARAMETER_C)*0.48 + 0.01;
    float q = getParameterValue(PARAMETER_D)*3+0.75;
    cutoff += getParameterValue(PARAMETER_F)*0.25; // MIDI CC1/Modulation
    float pitchbend = 0;//getParameterValue(PARAMETER_G); // MIDI Pitchbend
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
    setParameters(shape, cutoff, q, attack, release, pitchbend);
    voice->generate(left);
    display.update(left, 2, 0.0, 3.0, 0.0);
    
    // hat + kick
    float tone = 120*exp2f(getParameterValue(PARAMETER_E)*4);
    float decay = getParameterValue(PARAMETER_F);
    float accent = 0.5;//getParameterValue(PARAMETER_BA);
    hat->setFrequency(tone);
    hat->setFilter(getParameterValue(PARAMETER_E)*0.3 + 0.5);
    hat->setDecay(decay);
    hat->setAccent(accent);
    tone = 20*exp2f(getParameterValue(PARAMETER_G)*4);
    decay = getParameterValue(PARAMETER_H);
    kick->setFrequency(tone);
    kick->setDecay(decay);
    kick->setAccent(accent);
    // kick->setSnap(getParameterValue(PARAMETER_BB));
    hat->generate(right);
    left.add(right);
    kick->generate(right);
    left.add(right);
    left.multiply(0.2);
    right.copyFrom(left);
    setParameterValue(PARAMETER_AB, voice->getEnvelopeLevel()*0.5);
  
    // debugMessage("avg/max", right.getMean(), right.getMaxValue());

    int steps = 16;
    int fills;
    // steps = getParameterValue(PARAMETER_D)*31;
    fills = getParameterValue(PARAMETER_AA)*steps;
    seq[0].calculate(steps, fills);    
    fills = getParameterValue(PARAMETER_AC)*steps;
    seq[1].calculate(steps, fills);
    fills = getParameterValue(PARAMETER_AD)*steps;
    seq[2].calculate(steps, fills);
    fills = getParameterValue(PARAMETER_AE)*steps;
    seq[3].calculate(steps, fills);
    fills = getParameterValue(PARAMETER_AG)*steps;
    seq[4].calculate(steps, fills);

    uint8_t basenote = getParameterValue(PARAMETER_A)*80+20;
    float tempo = getParameterValue(PARAMETER_BB)*16 + 0.5;
    static float lfo0 = 0;
    if(lfo0 > 1.0){
      lfo0 = 0;
      int note = basenote;
      if(seq[1].next())
	note += 5;
      if(seq[2].next())
	note += 7;
      if(seq[0].next() || note != basenote)
      	noteOn(note, 2047, 0);
      else
      	noteOff(note, 0);
      if(seq[3].next())
      	hat->trigger(true, 0);
      if(seq[4].next())
      	kick->trigger(true, 0);
    }
    lfo0 += tempo * getBlockSize() / getSampleRate();

    static float lfo1 = 0;
    if(lfo1 > 1.0){
      lfo1 = 0;
    }else{
    }
    // lfo1 += 2 * getBlockSize() / getSampleRate();
    tempo = getParameterValue(PARAMETER_BC)*4+0.01;
    lfo1 += tempo * getBlockSize() / getSampleRate();
    setParameterValue(PARAMETER_AF, lfo1*0.4);

    static float lfo2 = 0;
    if(lfo2 > 1.0){
      lfo2 = 0;
    }else{
    }
    // lfo2 += 1 * getBlockSize() / getSampleRate();
    tempo = getParameterValue(PARAMETER_BD)*4+0.01;
    lfo2 += tempo * getBlockSize() / getSampleRate();
    setParameterValue(PARAMETER_AH, lfo2*0.4);

  }

  MonochromeAudioDisplay display;

  void processScreen(MonochromeScreenBuffer& screen){
    display.draw(screen, WHITE);
   }
};

#endif   // __KickBoxPatch_hpp__
