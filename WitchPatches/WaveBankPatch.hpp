#ifndef __WaveBankPatch_hpp__
#define __WaveBankPatch_hpp__

#include "OpenWareLibrary.h"
#include "TapTempo.hpp"
#include "MidiPolyphonicExpressionProcessor.h"

// #define USE_MPE
#define DDS_INTERPOLATE

#define VOICES 2
#if VOICES == 1
#define GAINFACTOR 0.8
#elif VOICES == 2
#define GAINFACTOR 0.7
#elif VOICES == 4
#define GAINFACTOR 0.6
#else
#define GAINFACTOR 0.4
#endif

#include "WaveBank.h"

#define SAMPLE_LEN 256
#define NOF_X_WF 8
#define NOF_Y_WF 8
#define NOF_Z_WF 7
#include "WaveBankSynth.h"

static const int TRIGGER_LIMIT = (1<<17);

#define BUTTON_VELOCITY 100

#if defined USE_MPE
typedef MidiPolyphonicExpressionMultiSignalGenerator<MorphStereoGenerator, VOICES> MorphVoices;
#elif VOICES == 1
typedef MonophonicMultiSignalGenerator<MorphStereoGenerator> MorphVoices;
#else
typedef PolyphonicMultiSignalGenerator<MorphStereoGenerator, VOICES> MorphVoices;
#endif

class WaveBankPatch : public Patch {
private:
  MorphVoices* voices;
  int basenote = 60;
  MorphBank* bank1;
  MorphBank* bank2;

  SineOscillator* lfo1;
  SineOscillator* lfo2;
  TapTempo<TRIGGER_LIMIT> tempo1;
  TapTempo<TRIGGER_LIMIT> tempo2;

  FloatArray createWavebank(const char* name){
    Resource* resource = getResource(name);
    WavFile wav(resource->getData());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
    FloatArray bank = wav.createFloatArray();
    Resource::destroy(resource);
    return bank;
  }
public:
  WaveBankPatch() :
    tempo1(getSampleRate()*0.5), tempo2(getSampleRate()*0.25) {

    FloatArray wt1 = createWavebank("wavetable1.wav");
    MorphBank* bank1 = MorphBank::create(wt1);
    FloatArray::destroy(wt1);
    FloatArray wt2 = createWavebank("wavetable2.wav");
    MorphBank* bank2 = MorphBank::create(wt2);
    FloatArray::destroy(wt2);

    voices = MorphVoices::create(2, getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, MorphStereoGenerator::create(bank1, bank2, getSampleRate(), getBlockSize()));
    
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Morph X");
    registerParameter(PARAMETER_C, "Morph Y");
    registerParameter(PARAMETER_D, "Envelope");
    setParameterValue(PARAMETER_A, 0.8);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0.5);

    registerParameter(PARAMETER_AA, "Decay");
    registerParameter(PARAMETER_AB, "Sustain");
    setParameterValue(PARAMETER_AA, 0.0);
    setParameterValue(PARAMETER_AB, 0.9);

    // lfo
    lfo1 = SineOscillator::create(getBlockRate());
    lfo2 = SineOscillator::create(getBlockRate());
    registerParameter(PARAMETER_F, "LFO1>");
    registerParameter(PARAMETER_G, "LFO2>");
  }

  ~WaveBankPatch(){
    MorphBank::destroy(bank1);
    MorphBank::destroy(bank2);
    SineOscillator::destroy(lfo1);
    SineOscillator::destroy(lfo2);
    for(int i=0; i<VOICES; ++i)
      MorphStereoGenerator::destroy(voices->getVoice(i));
    MorphVoices::destroy(voices);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    int note = 0;
    static int lastnote[4];
    switch(bid){
    case BUTTON_1:
      if(value){
	note = basenote;
	lastnote[0] = note;
	voices->noteOn(MidiMessage::note(1, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(1, lastnote[0], 0));
      }
      break;
    case BUTTON_2:
      if(value){
	note = basenote+3;
	lastnote[1] = note;
	voices->noteOn(MidiMessage::note(2, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(2, lastnote[1], 0));
      }
      break;
    case BUTTON_3:
      tempo1.trigger(value, samples);
      if(value)
	lfo1->reset();
      // note = basenote+7;
      break;
    case BUTTON_4:
      tempo2.trigger(value, samples);
      if(value)
	lfo2->reset(); // todo: instead of hard reset, calculate to sync on next edge
      // note = basenote+12;
      break;
    }
  }

  void processMidi(MidiMessage msg){
    voices->process(msg);
  }
    
  void processAudio(AudioBuffer &buffer) {
    basenote = getParameterValue(PARAMETER_A)*48+40;
    float x = getParameterValue(PARAMETER_B);  
    float y = getParameterValue(PARAMETER_C); 
    float env = getParameterValue(PARAMETER_D);
    voices->setParameter(MorphSynth::PARAMETER_ENV, env);
    voices->setParameter(MorphSynth::PARAMETER_X, x);
    voices->setParameter(MorphSynth::PARAMETER_Y, y);
    voices->generate(buffer);
    buffer.getSamples(LEFT_CHANNEL).tanh();
    buffer.getSamples(RIGHT_CHANNEL).tanh();
    
    // lfo
    tempo1.clock(getBlockSize());
    tempo2.clock(getBlockSize());
    float rate = getSampleRate()/tempo1.getSamples();
    lfo1->setFrequency(rate);
    setParameterValue(PARAMETER_F, lfo1->generate()*0.5+0.5);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    rate = getSampleRate()/tempo2.getSamples();
    lfo2->setFrequency(rate);
    setParameterValue(PARAMETER_G, lfo2->generate()*0.5+0.5);
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
  }
};

#endif   // __WaveBankPatch
