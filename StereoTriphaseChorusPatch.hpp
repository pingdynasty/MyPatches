#ifndef __StereoTriphaseChorusPatch_h__
#define __StereoTriphaseChorusPatch_h__

#include "OpenWareLibrary.h"
#include "TapTempoOscillator.h"
#define TRIGGER_LIMIT (1<<22)
constexpr size_t taps = 3;

template<size_t TAPS=3>
class ChorusProcessor : public SignalProcessor {
protected:
  typedef CrossFadingCircularBuffer<float> CircularBufferType;
  // typedef InterpolatingCircularFloatBuffer<HERMITE_INTERPOLATION> CircularBufferType;
  CircularBufferType* ringbuffer[TAPS];
  FloatArray inbuf;
  FloatArray outbuf;
  BiquadFilter* lpf;
  BiquadFilter* hpf;
  float delay = 0;
  float depth = 0;
  float amount = 0;
  float phase = 0;
  float feedback = 0;
  static constexpr float RADIANS_OFFSET = 2*M_PI/3; // 120 degrees in radians
public:
  ChorusProcessor(float sr, size_t bs, size_t ds){
    lpf = BiquadFilter::create(sr, 1);
    lpf->setLowPass(6000, FilterStage::BUTTERWORTH_Q);
    hpf = BiquadFilter::create(sr, 1);
    hpf->setHighPass(200, FilterStage::BUTTERWORTH_Q);
    for(size_t i=0; i<TAPS; ++i){
      // ringbuffer[i] = CircularBufferType::create(ds);
      ringbuffer[i] = CircularBufferType::create(ds, bs);
    }
    inbuf = FloatArray::create(bs);
    outbuf = FloatArray::create(bs);
    setDelay(0.5);
  }
  ~ChorusProcessor(){
    FloatArray::destroy(inbuf);
    FloatArray::destroy(outbuf);
    for(size_t i=0; i<TAPS; ++i)
      CircularBufferType::destroy(ringbuffer[i]);
    BiquadFilter::destroy(lpf);
    BiquadFilter::destroy(hpf);
  }
  /**
   * @param value should be from 0 to 1
   */
  void setDelay(float value){
    delay = ringbuffer[0]->getSize()*0.25f + value*(ringbuffer[0]->getSize())*0.25f;
  }
  void setDepth(float value){
    depth = value*delay*0.5f;
  }
  void setAmount(float value){
    amount = value;
  }
  void setFeedback(float value){
    feedback = value;
  }
  /*
   * Phase should be in radians: (0, 2*M_PI)
   */
  void setPhase(float value){
    phase = value;
  }
  void setLowPass(float fc){
    lpf->setLowPass(fc*4000 + 4000, FilterStage::BUTTERWORTH_Q);
  }
  void setHighPass(float fc){
    lpf->setHighPass(fc*400 + 40, FilterStage::BUTTERWORTH_Q);
  }
  // float process(float input){
  //   for(size_t i=0; i<TAPS; ++i){
  //     ringbuffer[i].write(input);
  //   }
  // }
  void process(FloatArray input, FloatArray output){
    ASSERT(input.getData() != output.getData(), "Inplace processing not supported");
    inbuf.multiply(feedback);
    inbuf.add(input);
    lpf->process(inbuf, inbuf);
    hpf->process(inbuf, inbuf);
    output.clear();
    for(size_t i=0; i<TAPS; ++i){
      float dt = delay + depth * sinf(phase + RADIANS_OFFSET * i);
      ringbuffer[i]->delay(inbuf, outbuf, inbuf.getSize(), dt);
      output.add(outbuf);
    }
    output.multiply(amount/(TAPS*0.707));
    outbuf.copyFrom(input);
    outbuf.multiply(1-amount);
    output.add(outbuf);
    inbuf.copyFrom(output); // save for feedback
  }
};

template<size_t TAPS=3>
class StereoChorusProcessor : public MultiSignalProcessor {
protected:
  ChorusProcessor<TAPS>* left;
  ChorusProcessor<TAPS>* right;
public:
  StereoChorusProcessor(float sr, size_t bs, size_t ds){
    left = new ChorusProcessor<TAPS>(sr, bs, ds*1.1);
    right = new ChorusProcessor<TAPS>(sr, bs, ds*0.9);
  }
  ~StereoChorusProcessor(){
    delete left;
    delete right;
  }
  void setDelay(float value){
    left->setDelay(value);
    right->setDelay(value);
  }
  void setDepth(float value){
    left->setDepth(value);
    right->setDepth(value);
  }
  void setFeedback(float value){
    left->setFeedback(value);
    right->setFeedback(value);
  }
  void setAmount(float value){
    left->setAmount(value);
    right->setAmount(value);
  }
  void setPhase(float value){
    left->setPhase(value);
    right->setPhase(-value);
  }
  void setLowPass(float fc){
    left->setLowPass(fc);
    right->setLowPass(fc);
  }
  void setHighPass(float fc){
    left->setHighPass(fc);
    right->setHighPass(fc);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    left->process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    right->process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }
  static StereoChorusProcessor<TAPS>* create(float sr, size_t bs, size_t delaysize){
    return new StereoChorusProcessor<TAPS>(sr, bs, delaysize);
  }
  static void destroy(StereoChorusProcessor<TAPS>* processor){
    delete processor;
  }
};

class StereoTriphaseChorusPatch : public Patch {
private:
  typedef StereoChorusProcessor<taps> StereoChorusMixProcessor;
  // 
  StereoChorusMixProcessor* processor;
  AudioBuffer* outputbuffer;
  TapTempoOscillator<SineOscillator>* lfo;
public:
  StereoTriphaseChorusPatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Depth");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Mix");
    registerParameter(PARAMETER_E, "Speedup");
    processor = StereoChorusMixProcessor::create(getSampleRate(), getBlockSize(), 0.200*getSampleRate());
    lfo = TapTempoOscillator<SineOscillator>::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo->setBeatsPerMinute(8);
    outputbuffer = AudioBuffer::create(2, getBlockSize());
  }
  ~StereoTriphaseChorusPatch(){
    StereoChorusMixProcessor::destroy(processor);
    TapTempoOscillator<SineOscillator>::destroy(lfo);
    AudioBuffer::destroy(outputbuffer);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      lfo->trigger(value, samples);
      if(value)
	lfo->reset();
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float speed = clamp(getParameterValue(PARAMETER_A)*4 - 1 - getParameterValue(PARAMETER_E)*4, -0.9f, 3.0f);
    lfo->clock(getBlockSize());
    lfo->adjustSpeed(speed);
    float modulation = lfo->generate()*0.5+0.5;
    float depth = getParameterValue(PARAMETER_B);
    float feedback = getParameterValue(PARAMETER_C)*0.6;
    depth = clamp(depth + feedback*0.1f, 0.0f, 1.0f); // compensate for zero depth/high feedback
    setButton(GREEN_BUTTON, modulation*4096);
    setParameterValue(PARAMETER_F, modulation);
    setParameterValue(PARAMETER_G, 1 - modulation);
    // processor->setDelay(0.5 - getParameterValue(PARAMETER_E)*0.5);
    processor->setDepth(depth);
    processor->setFeedback(feedback);
    processor->setAmount(getParameterValue(PARAMETER_D));
    // processor->setHighPass(getParameterValue(PARAMETER_AC));
    // processor->setLowPass(getParameterValue(PARAMETER_AD));
    processor->setPhase(lfo->getPhase());
    processor->process(buffer, *outputbuffer);
    outputbuffer->getSamples(LEFT_CHANNEL).softclip();
    outputbuffer->getSamples(RIGHT_CHANNEL).softclip();
    buffer.copyFrom(*outputbuffer);
  }
};

#endif // __StereoTriphaseChorusPatch_h__
