#ifndef __StereoChorusPatch_h__
#define __StereoChorusPatch_h__

#include "OpenWareLibrary.h"
#include "MorphingLFO.h"

#define TRIGGER_LIMIT (1<<22)

class StereoChorusProcessor : public MultiSignalProcessor {
  // typedef InterpolatingCircularFloatBuffer<HERMITE_INTERPOLATION> CircularBufferType;
  typedef CrossFadingCircularBuffer<float> CircularBufferType;
protected:
  AudioBuffer* buffer;
  CircularBufferType* bufferL;
  CircularBufferType* bufferR;
  MultiBiquadFilter* filter;
  float depth;
  SmoothFloat delay;
  SmoothFloat spread;
  SmoothFloat amount;
  static constexpr size_t taps = 3;
  size_t delay_times[taps*2];
public:
  StereoChorusProcessor(AudioBuffer* buffer, CircularBufferType* bufferL,
			CircularBufferType* bufferR, MultiBiquadFilter* filter) :			
    buffer(buffer), bufferL(bufferL), bufferR(bufferR), filter(filter),
    depth(0), delay(0), spread(0), amount(0) {
    if(bufferL)
      delay = 0.5*bufferL->getSize();
    for(size_t i=0; i<taps*2; ++i)
      delay_times[i] = delay;
  }
  /**
   * @param value should be from 0 to 1
   */
  void setDelay(float value){
    delay = value*bufferL->getSize() - buffer->getSize();
    // subtract one buffer length because we write before we read
  }
  void setDepth(float value){
    depth = value;
  }
  void setSpread(float value){
    spread = value*bufferL->getSize()/(taps*2);
  }
  void setAmount(float value){
    amount = value;
  }
  // void process(FloatArray input, FloatArray output){
  // }
  void process(AudioBuffer& input, AudioBuffer& output){
    FloatArray bufL = buffer->getSamples(LEFT_CHANNEL);
    FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL);
    size_t len = input.getSize();
    filter->process(input, *buffer);
    bufferL->write(bufL.getData(), len);
    bufferR->write(bufR.getData(), len);
    float ph = spread*0.25;
    for(size_t i=0; i<taps; ++i){
      FloatArray bufL = buffer->getSamples(LEFT_CHANNEL+i*2);
      FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL+i*2);
      float index = delay + ph*depth + spread*i*1.7;
      bufferL->delay(bufL.getData(), bufL.getSize(), delay_times[i*2], index);
      delay_times[i*2] = index;
      index = delay + ph*depth - spread*i*2.3;
      bufferR->delay(bufR.getData(), bufR.getSize(), delay_times[i*2+1], index);
      delay_times[i*2+1] = index;
    }
    buffer->multiply(amount/taps);
    output.copyFrom(input);
    output.multiply(1-amount); // scale dry signal
    FloatArray outL = output.getSamples(LEFT_CHANNEL);
    FloatArray outR = output.getSamples(RIGHT_CHANNEL);
    for(size_t i=0; i<taps; ++i){
      FloatArray bufL = buffer->getSamples(LEFT_CHANNEL+i*2);
      FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL+i*2);
      outL.add(bufL);
      outR.add(bufR);
    }
  }
  static StereoChorusProcessor* create(float sr, size_t bs, size_t delaysize){
    AudioBuffer* buffer = AudioBuffer::create(2*taps, bs);
    CircularBufferType* bufferL = CircularBufferType::create(delaysize, bs);
    CircularBufferType* bufferR = CircularBufferType::create(delaysize, bs);
    MultiBiquadFilter* filter = MultiBiquadFilter::create(48000, 2*taps, 4);
    filter->setLowPass(6000, FilterStage::BUTTERWORTH_Q);
    return new StereoChorusProcessor(buffer, bufferL, bufferR, filter);
  }
  static void destroy(StereoChorusProcessor* obj){
    AudioBuffer::destroy(obj->buffer);
    CircularBufferType::destroy(obj->bufferL);
    CircularBufferType::destroy(obj->bufferR);
    MultiBiquadFilter::destroy(obj->filter);
    delete obj;
  }
};

class StereoChorusPatch : public Patch {
private:
  typedef DryWetMultiSignalProcessor<StereoChorusProcessor> StereoChorusMixProcessor;
  StereoChorusMixProcessor* processor;
  MorphingLFO* lfo;
public:
  StereoChorusPatch(){
    registerParameter(PARAMETER_A, "StereoChorus");
    StereoChorusProcessor* chorus = StereoChorusProcessor::create(getSampleRate(), getBlockSize(), 0.20*getSampleRate());
    processor = StereoChorusMixProcessor::create(2, getBlockSize(), *chorus); // calls StereoChorusProcessor copy ctor
    delete chorus;
    processor->setSpread(0.2);
    lfo = MorphingLFO::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo->setBeatsPerMinute(60);
    setParameterValue(PARAMETER_AA, (float)MorphingLFO::SINE/(MorphingLFO::NOF_SHAPES - 1));
  }
  ~StereoChorusPatch(){
    StereoChorusMixProcessor::destroy(processor);
    MorphingLFO::destroy(lfo);
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
    lfo->select(getParameterValue(PARAMETER_AA));
    lfo->adjustSpeed(getParameterValue(PARAMETER_A)*3-1);
    lfo->clock(getBlockSize());
    setButton(PUSHBUTTON, lfo->getPhase() < M_PI);
    processor->setDelay(lfo->generate()*0.5+0.5);
    processor->setAmount(getParameterValue(PARAMETER_B));
    processor->setDepth(getParameterValue(PARAMETER_C));
    processor->setMix(getParameterValue(PARAMETER_D));
    processor->process(buffer, buffer);
    buffer.getSamples(LEFT_CHANNEL).softclip();
    buffer.getSamples(RIGHT_CHANNEL).softclip();
  }
};

#endif // __StereoChorusPatch_h__
