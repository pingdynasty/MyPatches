#ifndef __StereoFilterDelayPatch_h__
#define __StereoFilterDelayPatch_h__

#include "OpenWareLibrary.h"

static const int TRIGGER_LIMIT = (1<<17);

#define USE_SVF
// #define FRAC_DELAY

#ifdef FRAC_DELAY
typedef FractionalDelayProcessor<HERMITE_INTERPOLATION> SmoothDelayProcessor;
#else
typedef CrossFadingDelayProcessor SmoothDelayProcessor;
#endif

typedef PingPongFeedbackProcessor FeedbackProcessor;

class ParallelSignalProcessor : public MultiSignalProcessor {
protected:
  SignalProcessor** processors;
  size_t count;
public:
  ParallelSignalProcessor(SignalProcessor** processors, size_t count)
    : processors(processors), count(count) {}
  void process(AudioBuffer& input, AudioBuffer& output){
    ASSERT(input.getChannels() >= count, "Insufficient input channels");
    ASSERT(output.getChannels() >= count, "Insufficient output channels");
    for(size_t i=0; i<count; ++i)
      processors[i]->process(input.getSamples(i), output.getSamples(i));
  }
  void setProcessor(size_t index, SignalProcessor* processor){
    if(index < count)
      processors[index] = processor;
  }
  SignalProcessor* getProcessor(size_t index){
    if(index < count)
      return processors[index];
    return NULL;
  }
  ParallelSignalProcessor* create(size_t count){
    return new ParallelSignalProcessor(new SignalProcessor*[count], count);
  }
};

class SeriesSignalProcessor : public SignalProcessor {
protected:
  SignalProcessor** processors;
  size_t count;
public:
  SeriesSignalProcessor(SignalProcessor** processors, size_t count)
    : processors(processors), count(count) {}
  void process(FloatArray input, FloatArray output){
    processors[0]->process(input, output);
    for(size_t i=1; i<count; ++i)
      processors[i]->process(output, output);
  }
  void setProcessor(size_t index, SignalProcessor* processor){
    if(index < count)
      processors[index] = processor;
  }
  SignalProcessor* getProcessor(size_t index){
    if(index < count)
      return processors[index];
    return NULL;
  }
  SeriesSignalProcessor* create(size_t count){
    return new SeriesSignalProcessor(new SignalProcessor*[count], count);
  }
};

class SeriesMultiSignalProcessor : public MultiSignalProcessor {
protected:
  MultiSignalProcessor** processors;
  size_t count;
public:
  SeriesMultiSignalProcessor(MultiSignalProcessor** processors, size_t count)
    : processors(processors), count(count) {}
  void process(AudioBuffer& input, AudioBuffer& output){
    processors[0]->process(input, output);
    for(size_t i=1; i<count; ++i)
      processors[i]->process(output, output);
  }
  void setProcessor(size_t index, MultiSignalProcessor* processor){
    if(index < count)
      processors[index] = processor;
  }
  MultiSignalProcessor* getProcessor(size_t index){
    if(index < count)
      return processors[index];
    return NULL;
  }
  SeriesMultiSignalProcessor* create(size_t count){
    return new SeriesMultiSignalProcessor(new MultiSignalProcessor*[count], count);
  }
};

class StereoFilterDelayPatch : public Patch {
private:
#ifdef USE_SVF
  typedef StereoStateVariableFilter StereoFilter;
#else
  typedef StereoBiquadFilter StereoFilter;
#endif
  FeedbackProcessor* feedback;
  AdjustableTapTempo* tempo;
  const size_t max_delay;
  SmoothFloat delay_samples;
  SmoothDelayProcessor* left_delay;
  SmoothDelayProcessor* right_delay;
  ParallelSignalProcessor* parallel;
  SeriesMultiSignalProcessor* series;
  StereoFilter* filter;
  AudioBuffer* outputbuffer;
  SmoothFloat inputgain;
  bool locked = false;
public:
  StereoFilterDelayPatch() : max_delay(getSampleRate()*0.5) {
    registerParameter(PARAMETER_A, "Delay");
    // setParameterValue(PARAMETER_A, 0.25);
    registerParameter(PARAMETER_B, "Feedback");
    // setParameterValue(PARAMETER_B, 0);
    registerParameter(PARAMETER_C, "Filter");
    // setParameterValue(PARAMETER_C, 0.8);
    registerParameter(PARAMETER_D, "Dry/Wet");
    // setParameterValue(PARAMETER_D, 0.50);

    tempo = AdjustableTapTempo::create(getSampleRate(), TRIGGER_LIMIT);
    tempo->setBeatsPerMinute(120);
    // tempo->resetAdjustment(getParameterValue(PARAMETER_B)*4096);

    // delay
#ifdef FRAC_DELAY
    left_delay = SmoothDelayProcessor::create(TRIGGER_LIMIT);
    right_delay = SmoothDelayProcessor::create(TRIGGER_LIMIT*2);
#else
    left_delay = SmoothDelayProcessor::create(TRIGGER_LIMIT, getBlockSize());
    right_delay = SmoothDelayProcessor::create(TRIGGER_LIMIT*2, getBlockSize());
#endif

    feedback = FeedbackProcessor::create(left_delay, right_delay, getBlockSize());
    // left_delay->setDelay(max_delay*0.5);
    // right_delay->setDelay(max_delay*0.5);
    delay_samples.lambda = 0.99;

#ifdef USE_SVF
    filter = StereoFilter::create(getSampleRate());
#else
    filter = StereoFilter::create(getSampleRate(), 2);
#endif
    outputbuffer = AudioBuffer::create(2, getBlockSize());
    setButton(BUTTON_2, locked, 0);
  }

  ~StereoFilterDelayPatch(){
    AudioBuffer::destroy(outputbuffer);
    StereoFilter::destroy(filter);
    SmoothDelayProcessor::destroy(left_delay);
    SmoothDelayProcessor::destroy(right_delay);
    FeedbackProcessor::destroy(feedback);
    AdjustableTapTempo::destroy(tempo);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      tempo->trigger(value, samples);
      break;
    case BUTTON_2:
      if(value)
	locked = !locked;
      setButton(BUTTON_2, locked, 0);
      break;
    }
  }
  static float noteToFrequency(float note){
    return 440 * exp2f((note - 69) / 12);
  }
  void processAudio(AudioBuffer &buffer){
    tempo->clock(getBlockSize());
    tempo->adjustSpeed(getParameterValue(PARAMETER_A));
    float fb = getParameterValue(PARAMETER_B)*1.1;
    float mix = getParameterValue(PARAMETER_D);
    float exp = getParameterValue(PARAMETER_E);
    float fc = noteToFrequency(getParameterValue(PARAMETER_C)*84+36);
    fc = std::clamp(fc * (1 - exp), 65.41f, 8372.02f); // C2 to C9
    delay_samples = tempo->getPeriodInSamples();

    // lock feedback loop: input gain 0, feedback 1
    inputgain = locked ? 0 : 1;
    if(locked)
      fb = 1;
    debugMessage("lock", (float)locked, inputgain, fb);
    
    // filter
    filter->setLowPass(fc, FilterStage::BUTTERWORTH_Q);
    filter->process(buffer, *outputbuffer);
    outputbuffer->multiply(inputgain);
    
    // delay
    left_delay->setDelay(delay_samples);
    right_delay->setDelay(delay_samples);
    feedback->setFeedback(fb);
    feedback->process(*outputbuffer, *outputbuffer);

    // mix
    outputbuffer->multiply(mix);
    buffer.multiply(1-mix);
    buffer.add(*outputbuffer);
    buffer.getSamples(LEFT_CHANNEL).softclip();
    buffer.getSamples(RIGHT_CHANNEL).softclip();
  }
};

#endif // __StereoFilterDelayPatch_h__
