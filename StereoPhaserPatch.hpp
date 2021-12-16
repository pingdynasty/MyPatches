#ifndef __StereoPhaserPatch_hpp__
#define __StereoPhaserPatch_hpp__

#include "OpenWareLibrary.h"
#include "MorphingLFO.h"

#define TRIGGER_LIMIT (1<<22)

class PhaserSignalProcessor : public SignalProcessor {
protected:
  float a1;
  FloatArray zm1;
  float feedback = 0;
  float depth = 0;
  float z = 0;
public:
  PhaserSignalProcessor(){
    zm1 = FloatArray::create(6);
  }
  ~PhaserSignalProcessor(){
    FloatArray::destroy(zm1);
  }
  void setDelay(float delay){ //sample delay time
    a1 = (1.f - delay) / (1.f + delay);
  }
  void setFeedback(float f){
    feedback = f;
  }
  void setDepth(float d){
    depth = d;
  }
  float process(float input){
    float x = input + z*feedback;
    for(size_t i=0; i<zm1.getSize(); ++i){
      float y = x * -a1 + zm1[i]; // each zm1 is an all pass stage
      zm1[i] = y * a1 + x;
      x = y; // the output of one stage becomes the input of the next one
    }
    z = x; // save for feedback
    input += x * depth;
    return input;
  }
  using SignalProcessor::process;
};

class StereoPhaserProcessor : public MultiSignalProcessor {
protected:
  PhaserSignalProcessor phaser_left;
  PhaserSignalProcessor phaser_right;
public:
  StereoPhaserProcessor(){
  }
  ~StereoPhaserProcessor(){
  }
  void setDelay(float value){
      // phaser.setDelay(value*0.04833+0.01833); // range: 440/(sr/2) to 1600/(sr/2)
    phaser_left.setDelay(value*0.04+0.02);
    phaser_right.setDelay(value*0.05+0.018);
  }
  void setDepth(float value){
    phaser_left.setDepth(value*2);
    phaser_right.setDepth(value*2);
  }
  void setFeedback(float value){
    phaser_left.setFeedback(value);
    phaser_right.setFeedback(value);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    phaser_left.process(output.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    phaser_right.process(output.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }
  static StereoPhaserProcessor* create(){
    return new StereoPhaserProcessor();
  }
  static void destroy(StereoPhaserProcessor* obj){
    delete obj;
  }
};

class FilteredStereoPhaserProcessor : public StereoPhaserProcessor {
protected:
  StereoBiquadFilter* filter;
public:
  FilteredStereoPhaserProcessor(float sr){
    filter = StereoBiquadFilter::create(sr, 1);
    filter->setHighPass(120, FilterStage::BUTTERWORTH_Q);
  }
  ~FilteredStereoPhaserProcessor(){
    StereoBiquadFilter::destroy(filter);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    filter->process(input, output);
    StereoPhaserProcessor::process(output, output);
  }
};

class StereoPhaserPatch : public Patch {
private:
  typedef DryWetMultiSignalProcessor<FilteredStereoPhaserProcessor> StereoPhaserMixProcessor;
  StereoPhaserMixProcessor* processor;
  MorphingLFO* lfo;
  SmoothFloat delay;
public:
  StereoPhaserPatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Shape");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Mix");
    registerParameter(PARAMETER_E, "Speedup");
    processor = StereoPhaserMixProcessor::create(2, getBlockSize(), getSampleRate());
    lfo = MorphingLFO::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo->setBeatsPerMinute(60);
    delay.lambda = 0.8;
  }
  ~StereoPhaserPatch(){
    StereoPhaserMixProcessor::destroy(processor);
    MorphingLFO::destroy(lfo);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      lfo->trigger(value, samples);
      if(value)
	lfo->reset();
      break;
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float speed = clamp(getParameterValue(PARAMETER_A)*4 - 1 - getParameterValue(PARAMETER_E)*4, -0.9f, 3.0f);
    lfo->select(getParameterValue(PARAMETER_B));
    lfo->clock(getBlockSize());
    lfo->adjustSpeed(speed);
    delay = lfo->generate()*0.5+0.5;
    processor->setDelay(delay);
    setButton(GREEN_BUTTON, 4096 * delay);
    setParameterValue(PARAMETER_F, delay);
    setParameterValue(PARAMETER_G, 1 - delay);
    // setButton(PUSHBUTTON, lfo->getPhase() < M_PI);
    processor->setDepth(getParameterValue(PARAMETER_C));
    processor->setFeedback(getParameterValue(PARAMETER_C)*0.8);
    processor->setMix(getParameterValue(PARAMETER_D));
    processor->process(buffer, buffer);
    buffer.getSamples(LEFT_CHANNEL).softclip();
    buffer.getSamples(RIGHT_CHANNEL).softclip();
  }
};

#endif   // __StereoPhaserPatch_hpp__
