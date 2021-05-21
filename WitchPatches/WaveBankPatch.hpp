#ifndef __WaveBankPatch_hpp__
#define __WaveBankPatch_hpp__

#include "OpenWareLibrary.h"

// #define USE_MPE
#define VOICES 2
// #define DDS_INTERPOLATE
#define SAMPLE_LEN 256
#define NOF_X_WF 8
#define NOF_Y_WF 8
#define NOF_Z_WF 7
#include "WitchPatch.hpp"
#include "WaveBank.h"
#include "WaveBankSynth.h"

static const int TRIGGER_LIMIT = (1<<22);
#define BUTTON_VELOCITY 100

#if defined USE_MPE
typedef MidiPolyphonicExpressionMultiSignalGenerator<MorphStereoGenerator, VOICES> SynthVoices;
#elif VOICES == 1
typedef MonophonicMultiSignalGenerator<MorphStereoGenerator> SynthVoices;
#else
typedef PolyphonicMultiSignalGenerator<MorphStereoGenerator, VOICES> SynthVoices;
#endif

class WaveBankPatch : public Patch {
private:
  SynthVoices* voices;
  MorphBank* bank1;
  MorphBank* bank2;
  TapTempoSineOscillator* lfo1;
  TapTempoAgnesiOscillator* lfo2;
  CvNoteProcessor* cvnote;
  OverdriveProcessor overdrive;

  FloatArray createWavebank(const char* name){
    Resource* resource = getResource(name);
    WavFile wav(resource->getData(), resource->getSize());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
    FloatArray bank = wav.createFloatArray(0);
    Resource::destroy(resource);
    return bank;
  }
public:
  WaveBankPatch(){
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Morph X");
    registerParameter(PARAMETER_C, "Morph Y");
    registerParameter(PARAMETER_E, "Overdrive");
    registerParameter(PARAMETER_F, "Sine LFO>");
    registerParameter(PARAMETER_G, "Witch LFO>");
    setParameterValue(PARAMETER_A, 0.8);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0.5);

    registerParameter(PARAMETER_AA, "Decay");
    registerParameter(PARAMETER_AB, "Sustain");
    setParameterValue(PARAMETER_AA, 0.0);
    setParameterValue(PARAMETER_AB, 0.9);

    FloatArray wt1 = createWavebank("wavetable1.wav");
    MorphBank* bank1 = MorphBank::create(wt1);
    FloatArray::destroy(wt1);
    FloatArray wt2 = createWavebank("wavetable2.wav");
    MorphBank* bank2 = MorphBank::create(wt2);
    FloatArray::destroy(wt2);

    // voices
    voices = SynthVoices::create(2, getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, MorphStereoGenerator::create(bank1, bank2, getSampleRate(), getBlockSize()));
    
    // lfo
    lfo1 = TapTempoSineOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo2 = TapTempoAgnesiOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo1->setBeatsPerMinute(60);
    lfo2->setBeatsPerMinute(120);

    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices);

#ifdef USE_MPE
    // send MPE Configuration Message RPN
    sendMidi(MidiMessage::cc(0, 100, 5));
    sendMidi(MidiMessage::cc(0, 101, 0));
    sendMidi(MidiMessage::cc(0, 6, VOICES));
#endif
  }

  ~WaveBankPatch(){
    for(int i=0; i<VOICES; ++i)
      MorphStereoGenerator::destroy(voices->getVoice(i));
    SynthVoices::destroy(voices);
    MorphBank::destroy(bank1);
    MorphBank::destroy(bank2);
    TapTempoSineOscillator::destroy(lfo1);
    TapTempoAgnesiOscillator::destroy(lfo2);
    CvNoteProcessor::destroy(cvnote);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      cvnote->gate(value, samples);
      break;
    case BUTTON_2:
      lfo1->trigger(value, samples);
      break;
    case BUTTON_3:
      lfo2->trigger(value, samples);
      break;
    case BUTTON_4:
      static bool sustain = false;
      if(value){
	sustain = !sustain; // toggle
#ifndef USE_MPE /// todo! MPE sustain
	voices->setSustain(sustain);
#endif
	if(!sustain)
	  voices->allNotesOff();
      }
      setButton(BUTTON_4, sustain);
      break;
    }
  }
  
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }

  void processAudio(AudioBuffer &buffer) {
    cvnote->cv(getParameterValue(PARAMETER_A));
    cvnote->clock(getBlockSize());

    float x = getParameterValue(PARAMETER_B);  
    float y = getParameterValue(PARAMETER_C); 
    float env = getParameterValue(PARAMETER_D);
    overdrive.setEffect(getParameterValue(PARAMETER_E));
    voices->setParameter(MorphSynth::PARAMETER_ENV, env);
    voices->setParameter(MorphSynth::PARAMETER_X, x);
    voices->setParameter(MorphSynth::PARAMETER_Y, y);
    voices->generate(buffer);
    overdrive.process(buffer, buffer);

    // lfo
    lfo1->clock(getBlockSize());
    lfo2->clock(getBlockSize());
    float lfo = lfo1->generate()*0.5+0.5;
    overdrive.setModulation(lfo);
    setParameterValue(PARAMETER_F, lfo);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    setParameterValue(PARAMETER_G, lfo2->generate());
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
  }
};

#endif   // __WaveBankPatch
