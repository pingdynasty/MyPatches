#ifndef __WaveBankPatch_hpp__
#define __WaveBankPatch_hpp__

#include "OpenWareLibrary.h"

// #define USE_MPE
#define VOICES 4
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
typedef MidiPolyphonicExpressionMultiSignalProcessor<MorphStereoGenerator, VOICES> SynthVoices;
#elif VOICES == 1
typedef MonophonicMultiSignalGenerator<MorphStereoGenerator> SynthVoices;
#else
typedef PolyphonicMultiSignalProcessor<MorphStereoGenerator, VOICES> SynthVoices;
#endif

// typedef StereoPhaserProcessor FxProcessor;
// typedef StereoChorusProcessor FxProcessor;
typedef OverdriveProcessor FxProcessor;
// typedef StereoOverdriveProcessor FxProcessor;

class WaveBankPatch : public Patch {
private:
  SynthVoices* voices;
  MorphBank* bank1;
  MorphBank* bank2;
  TapTempoSineOscillator* lfo1;
  TapTempoAgnesiOscillator* lfo2;
  CvNoteProcessor* cvnote;
  FxProcessor* fx;
  AudioBuffer* buffer;
  static constexpr float cvrange = 5;

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
    registerParameter(PARAMETER_E, "Effect Amount");
    registerParameter(PARAMETER_F, "Sine LFO>");
    registerParameter(PARAMETER_G, "Witch LFO>");
    setParameterValue(PARAMETER_A, 0.8);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0.5);

    registerParameter(PARAMETER_AA, "FM Amount");
    setParameterValue(PARAMETER_AA, 0.1);
    // registerParameter(PARAMETER_AB, "Decay");
    // registerParameter(PARAMETER_AC, "Sustain");
    // setParameterValue(PARAMETER_AB, 0.0);
    // setParameterValue(PARAMETER_AC, 0.9);

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

#ifdef USE_MPE
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 0, 18+6*cvrange);
    // send MPE Configuration Message RPN
    // send MPE Configuration Message RPN
    sendMidi(MidiMessage::cc(0, 100, 5));
    sendMidi(MidiMessage::cc(0, 101, 0));
    sendMidi(MidiMessage::cc(0, 6, VOICES));
#else
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 12*cvrange, 24);
#endif

    // fx = FxProcessor::create(getSampleRate(), getBlockSize(), 0.240*getSampleRate());
    fx = FxProcessor::create();
    buffer = AudioBuffer::create(getNumberOfChannels(), getBlockSize());
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
    FxProcessor::destroy(fx);
    AudioBuffer::destroy(buffer);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      cvnote->gate(value, samples);
      break;
    case BUTTON_2:
      lfo1->trigger(value, samples);
      if(value)
	lfo1->reset();
      break;
    case BUTTON_3:
      lfo2->trigger(value, samples);
      if(value)
	lfo2->reset();
      break;
    case BUTTON_4:
#if VOICES > 1
      static bool sustain = false;
      if(value){
	sustain = !sustain; // toggle
	voices->setSustain(sustain);
	if(!sustain)
	  voices->allNotesOff();
      }
      setButton(BUTTON_4, sustain);
#endif
      break;
    }
  }
  
  void processMidi(MidiMessage msg){
    voices->process(msg);
  }

  void processAudio(AudioBuffer& audio) {
    cvnote->cv(getParameterValue(PARAMETER_A));
    cvnote->clock(getBlockSize());

    float x = getParameterValue(PARAMETER_B);  
    float y = getParameterValue(PARAMETER_C); 
    float env = getParameterValue(PARAMETER_D);
    fx->setEffect(getParameterValue(PARAMETER_E));
    voices->setParameter(MorphSynth::PARAMETER_ENV, env);
    voices->setParameter(MorphSynth::PARAMETER_X, x);
    voices->setParameter(MorphSynth::PARAMETER_Y, y);

    buffer->copyFrom(audio);
    float fm_amount = getParameterValue(PARAMETER_AA);
    buffer->multiply(fm_amount);
    // audio.clear();
#ifdef USE_MPE
    float fm = exp2f(cvrange*getParameterValue(PARAMETER_A) - cvrange*0.5) - 1;
    buffer->add(fm);
    voices->process(*buffer, audio);
#else
    voices->process(*buffer, audio);
#endif
    fx->process(audio, audio);

    // lfo
    lfo1->clock(getBlockSize());
    lfo2->clock(getBlockSize());
    float lfo = lfo1->generate()*0.5+0.5;
    fx->setModulation(lfo);
    setParameterValue(PARAMETER_F, lfo);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    setParameterValue(PARAMETER_G, lfo2->generate());
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
  }
};

#endif   // __WaveBankPatch
