#ifndef __WaveBankPatch_hpp__
#define __WaveBankPatch_hpp__

#include "OpenWareLibrary.h"

#define USE_MPE
#define VOICES 4
// #define DDS_INTERPOLATE
#define SAMPLE_LEN 256
#define NOF_X_WF 8
#define NOF_Y_WF 8
#define NOF_Z_WF 7

#include "WitchFX.hpp"
#include "WaveBank.h"
#include "WaveBankSynth.h"

typedef MorphStereoGenerator SynthVoice;

#if defined USE_MPE
typedef MidiPolyphonicExpressionProcessor<SynthVoice, VOICES> Allocator;
#elif VOICES == 1
typedef MonophonicProcessor<SynthVoice> Allocator;
#else
typedef PolyphonicProcessor<SynthVoice, VOICES> Allocator;
#endif

typedef VoiceAllocatorMultiSignalProcessor<Allocator, SynthVoice, VOICES> SynthVoices;

#include "WitchPatch.hpp"

class WaveBankPatch : public WitchPatch {
private:
  MorphBank* bank1;
  MorphBank* bank2;
  AudioBuffer* buffer;

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

    setParameterValue(PARAMETER_FX_SELECT, fx->getParameterValueForEffect(WitchMultiEffect::DELAY));

    FloatArray wt1 = createWavebank("wavetable1.wav");
    bank1 = MorphBank::create(wt1);
    FloatArray::destroy(wt1);
    FloatArray wt2 = createWavebank("wavetable2.wav");
    bank2 = MorphBank::create(wt2);
    FloatArray::destroy(wt2);

    // voices
    voices = SynthVoices::create(2, getBlockSize());
    for(int i=0; i<VOICES; ++i)
      voices->setVoice(i, MorphStereoGenerator::create(bank1, bank2, getSampleRate(), getBlockSize()));

    buffer = AudioBuffer::create(getNumberOfChannels(), getBlockSize());
#ifdef USE_MPE
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 0, 18+6*cvrange);
#else
    cvnote = CvNoteProcessor::create(getSampleRate(), 6, voices, 12*cvrange, 24);
#endif
  }

  ~WaveBankPatch(){
    for(int i=0; i<VOICES; ++i)
      SynthVoice::destroy(voices->getVoice(i));
    SynthVoices::destroy(voices);
    CvNoteProcessor::destroy(cvnote);
    MorphBank::destroy(bank1);
    MorphBank::destroy(bank2);
    AudioBuffer::destroy(buffer);
  }

  void processAudio(AudioBuffer& audio) {
    doprocess(audio);

    voices->setParameter(MorphSynth::PARAMETER_ENV, getParameterValue(PARAMETER_D));
    voices->setParameter(MorphSynth::PARAMETER_X, getParameterValue(PARAMETER_B));
    voices->setParameter(MorphSynth::PARAMETER_Y, getParameterValue(PARAMETER_C));

    buffer->copyFrom(audio);
    audio.clear();
#ifdef USE_MPE
    float fm = exp2f(cvrange*getParameterValue(PARAMETER_A) - cvrange*0.5) - 1;
    buffer->add(fm);
#endif
    voices->process(*buffer, audio);
    dofx(audio);
    dolfo();
  }
};

#endif   // __WaveBankPatch
