#ifndef __StereoMultiFilterPatch_h__
#define __StereoMultiFilterPatch_h__

#include "Patch.h"
#include "OpenWareLibrary.h"

#define USE_SVF

class StereoMultiFilterPatch : public Patch {
#ifdef USE_SVF
  StereoStateVariableFilter* filter[3];
  static constexpr float qrange = 9;
#else
  StereoBiquadFilter* filter[3];
  static constexpr float qrange = 2;
#endif
  AudioBuffer* process;
  AudioBuffer* output;
public:
  StereoMultiFilterPatch(){
    registerParameter(PARAMETER_A, "Cutoff");
    registerParameter(PARAMETER_B, "Resonance");
    registerParameter(PARAMETER_C, "Mode");
    registerParameter(PARAMETER_D, "Mix");
    registerParameter(PARAMETER_E, "Expression");
    process = AudioBuffer::create(2, getBlockSize());
    output = AudioBuffer::create(2, getBlockSize());
#ifdef USE_SVF
    filter[0] = StereoStateVariableFilter::create(getSampleRate());
    filter[1] = StereoStateVariableFilter::create(getSampleRate());
    filter[2] = StereoStateVariableFilter::create(getSampleRate());
#else
    filter[0] = StereoBiquadFilter::create(getSampleRate(), 4);
    filter[1] = StereoBiquadFilter::create(getSampleRate(), 4);
    filter[2] = StereoBiquadFilter::create(getSampleRate(), 4);
#endif
  }
  ~StereoMultiFilterPatch(){
    AudioBuffer::destroy(process);
    AudioBuffer::destroy(output);
#ifdef USE_SVF
    StereoStateVariableFilter::destroy(filter[0]);
    StereoStateVariableFilter::destroy(filter[1]);
    StereoStateVariableFilter::destroy(filter[2]);
#else
    StereoBiquadFilter::destroy(filter[0]);
    StereoBiquadFilter::destroy(filter[0]);
    StereoBiquadFilter::destroy(filter[0]);
#endif
  }
  static float noteToFrequency(float note){
    return 440 * exp2f((note - 69) / 12);
  }
  void processAudio(AudioBuffer &buffer){
    float exp = getParameterValue(PARAMETER_E);
    float fc = noteToFrequency(getParameterValue(PARAMETER_A)*84+36-48*exp); // C3 to C8
    float q = getParameterValue(PARAMETER_B) * qrange + 0.1;
    float mode = getParameterValue(PARAMETER_C)*2;
    float wet = getParameterValue(PARAMETER_D);
    float l0 = 1 - clamp(mode, 0.0f, 1.0f);
    float l1 = clamp(mode, 0.0f, 1.0f) - (clamp(mode, 1.0f, 2.0f) - 1);
    float l2 = clamp(mode, 1.0f, 2.0f) - 1;
    output->clear();
    filter[0]->setLowPass(fc, q);
    filter[0]->process(buffer, *process);
    process->multiply(l0);
    output->add(*process);
    filter[1]->setBandPass(fc, q*1.3);
    filter[1]->process(buffer, *process);
    process->multiply(l1);
    output->add(*process);
    filter[2]->setHighPass(fc, q);
    filter[2]->process(buffer, *process);
    process->multiply(l2);
    output->add(*process);
    output->multiply(wet);
    buffer.multiply(1-wet);
    buffer.add(*output);
    buffer.getSamples(LEFT_CHANNEL).softclip();
    buffer.getSamples(RIGHT_CHANNEL).softclip();
  }
};

#endif // __StereoMultiFilterPatch_h__
