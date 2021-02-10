#ifndef __PolyVcoPatch_h__
#define __PolyVcoPatch_h__

#include <cfloat>
#include "basicmaths.h"

#undef min
#undef max
#undef sinf
#undef cosf
#undef sqrtf
#undef powf
#undef logf
#undef log2f
#undef log10f
#undef expf
#undef exp2f
#undef exp10f

#include "Patch.h"
#include "MonoVoiceAllocator.hpp"

static constexpr int TUNING_TABLE_SIZE = 128;
static constexpr int CV_SETTLE_BLOCKS = 1;

static constexpr int POINTS_AVERAGE = 3;
static constexpr int POINTS_DISCARD = 0;
static constexpr float POS_THRESHOLD = 0.1;
static constexpr float NEG_THRESHOLD = -0.1;
// static constexpr float POS_THRESHOLD = 1.0;
// static constexpr float NEG_THRESHOLD = 0.9;
// sync out on Magus audio in: -0.22 to 0.03
// sync out on Noctua audio in: 0 to 1.99
static constexpr float SPREAD_THRESHOLD = 0;

static constexpr float range1 = -2.00;
static constexpr float offset1 = 1.00;

static constexpr float CH1_MIN = 1; // inverted range
static constexpr float CH1_MAX = -1;
static constexpr float CH2_MIN = -0.1;
static constexpr float CH2_MAX = 0.9;

// static constexpr size_t MAX_PERIOD_SAMPLES = 6000; // 8 Hz
static constexpr size_t MAX_PERIOD_SAMPLES = 2400; // 20 Hz

static float noteToFrequency(float note){
  return 440.0f*exp2f((note-69.0f)/12.0f);
}

static float frequencyToNote(float freq){
  return 12.0f*log2f(freq/440.0f)+69.0f;
}

class FrequencyCounter {
private:
  const float SR;
  FloatArray counts;
  size_t index = 0;
  float lastCrossing = 0;
  float lastSample = 0;
  bool positive = false;
  bool done = false;
public:
  FrequencyCounter(float SR) : SR(SR) {
    counts = FloatArray::create(POINTS_AVERAGE+POINTS_DISCARD); //number of zcc to be averaged
    reset();
  }
  ~FrequencyCounter() {
    FloatArray::destroy(counts);
  }
  void process(FloatArray input){
    size_t len = input.getSize();
    for(size_t i=0; i<len; i++){
      float sample = input[i];
      if(positive && (sample < NEG_THRESHOLD)){
	positive = false;
      }else if(!positive && (sample > POS_THRESHOLD)){
	positive = true;
	float offset = (sample - POS_THRESHOLD)/(sample - lastSample); // linear interpolation
	counts[index++] = i - offset - lastCrossing;
        if(index == counts.getSize()){
	  index = 0;
	  done = true;
	}
	lastCrossing = i - offset;
      }
      lastSample = sample;
    }
    lastCrossing -= len;
    if(-lastCrossing > MAX_PERIOD_SAMPLES){
      reset(); // set all counts to zero
      done = true;
    }      
  }
  float getFrequency(){
    // exclude first N measurements
    FloatArray sub = counts.subArray(POINTS_DISCARD, counts.getSize()-POINTS_DISCARD);
    float mean = sub.getMean();
    if(mean > 0.0) // avoid divide by zero
      return SR/mean;
    return FLT_EPSILON;
  }
  float getSpread(){
    FloatArray sub = counts.subArray(POINTS_DISCARD, counts.getSize()-POINTS_DISCARD);
    return sub.getStandardDeviation();
  }
  bool complete(){
    if(SPREAD_THRESHOLD > 0)
      return done && getSpread() < SPREAD_THRESHOLD;
    else
      return done;
  }
  void reset(){
    counts.clear();
    index = 0;
    done = false;
  }
};

class Tuner {
private:
  FrequencyCounter fc;
  FloatArray table;
  size_t index = 0;
  uint32_t settle;
public:
  Tuner(size_t len, float SR) : fc(SR) {
    table = FloatArray::create(len);
  }
  ~Tuner(){
    FloatArray::destroy(table);
  }
  float getLowestFrequency(){
    return noteToFrequency(table.getMinValue());
  }
  float getHighestFrequency(){
    return noteToFrequency(table.getMaxValue());
  }
  void process(FloatArray input){
    if(settle){
      settle--;
    }else{
      if(index < table.getSize()){
	fc.process(input);
	if(fc.complete()){
	  table[index++] = frequencyToNote(fc.getFrequency());
	  // table[index++] = fc.getFrequency();
	  fc.reset();
	  beginSettle(); // incrementing index will produce a new CV which requires two blocks to settle
	}
      }
    }
  }
  void begin(){
    index = 0;
    fc.reset();
    beginSettle();
  }
  void beginSettle(){
    settle = CV_SETTLE_BLOCKS; // settle CV for some blocks
  }    
  bool isTuned(){
    return index == table.getSize();
  }
  float getValue(){
    return getValue(index);
  }
  float getValue(size_t index){
    if(index < table.getSize())
      return table[index];
    return 127;
  }
  float getNoteForCV(float cv){
    if(cv < 0)
      return 0;
    float idx = cv*table.getSize();
    float fraction = idx-(int)idx;
    float low = getValue(floor(idx));
    float high = getValue(ceil(idx));
    return low+(high-low)*fraction;
  }
  float getFrequencyForCV(float cv){
    return noteToFrequency(getNoteForCV(cv));
  }
  float getTuningCV(){
    return float(index) / table.getSize();
  }
  float getNoteCV(float note){
    float low = table[0];
    if(note < low)
      return 0;
    for(size_t i=1; i<table.getSize(); ++i){
      float high = table[i];
      if(high > note){
	float offset = (high - note)/(high - low);
	// debugMessage("lo/hi/note", low, high, note);
	// debugMessage("lo/hi/offset", low, high, offset);
	float cv = float(i-offset)/table.getSize();
	// debugMessage("cv/note/target", cv, getNoteForCV(cv), note);
	// debugMessage("cv/freq/target", cv, getFrequencyForCV(cv), noteToFrequency(note));
	return cv;
      }else{
	low = high;
      }
    }
    return 1;
  }
};

class PolyVcoPatch : public Patch {
  MonoVoiceAllocator allocator;
  FrequencyCounter fc;
  Tuner tuner;
  SmoothFloat cv;
  enum PatchMode {
	INIT_MODE,
	CTRL_MODE,
	PLAY_MODE,
	TUNE_MODE,
	LINEAR_MODE
  };
  PatchMode mode = INIT_MODE;
public:
  PolyVcoPatch() : fc(getSampleRate()), tuner(TUNING_TABLE_SIZE, getSampleRate()) {
    registerParameter(PARAMETER_A, "CV");
    setParameterValue(PARAMETER_A, 0);
    registerParameter(PARAMETER_B, "Note");
    setParameterValue(PARAMETER_B, 0);
    registerParameter(PARAMETER_H, "Tune>");
    // debugMessage("PolyVco SR/BS/CH", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());
  }
  
  float getLinearCVForNote(float note){
    float FREQ_LOW = 20;
    float FREQ_HIGH = 20000;
    if(tuner.isTuned()){
      FREQ_LOW = tuner.getLowestFrequency();
      FREQ_HIGH = tuner.getHighestFrequency();
    }
    float low = frequencyToNote(FREQ_LOW);
    float high = frequencyToNote(FREQ_HIGH);
    return (note-low)/(high-low);
  }

  void processMidi(MidiMessage msg){
    allocator.processMidi(msg);
    switch (msg.getStatus()) {
    case CONTROL_CHANGE:
      // todo: this should not be necessary
      switch(msg.getControllerNumber()){
    	case 27: // PATCH_BUTTON_ON
    	buttonChanged((PatchButtonId)msg.getControllerValue(), 127, 0);
    	break;
      case 28: // PATCH_BUTTON_OFF
    	buttonChanged((PatchButtonId)msg.getControllerValue(), 0, 0);
    	break;
      }
      break;
    }
  }

  virtual void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(value){
      switch(bid){
      // case PUSHBUTTON:
      case BUTTON_A:
	mode = CTRL_MODE;
	break;
      case BUTTON_B:
	mode = TUNE_MODE;
	break;
      case BUTTON_C:
	mode = PLAY_MODE;      
	break;
      case BUTTON_D:
	mode = LINEAR_MODE;
	break;
      case BUTTON_E:
	mode = INIT_MODE;
	break;
      default:
	break;
      }
    }
  }

  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(0);
    FloatArray right = buffer.getSamples(2); // channel 3 on Noctua
    float cv;
    fc.process(left);
    switch(mode){
    case INIT_MODE: {
      setButton(PUSHBUTTON, false);
      tuner.begin();
      mode = TUNE_MODE;
      break;
    }
    case CTRL_MODE: {
      setButton(PUSHBUTTON, true);
      cv = getParameterValue(PARAMETER_A);
      debugMessage("ctrl", cv, fc.getFrequency(), tuner.getFrequencyForCV(cv));
      break;
    }
    case TUNE_MODE: {
      if(tuner.isTuned()){
	setButton(PUSHBUTTON, false);
	cv = getParameterValue(PARAMETER_A);
	debugMessage("tuned", cv, fc.getFrequency(), tuner.getFrequencyForCV(cv));
      }else{
	setButton(PUSHBUTTON, true);
	tuner.process(left);
	cv = tuner.getTuningCV();
	debugMessage("tuning", cv, fc.getFrequency(), fc.getSpread());
	if(tuner.isTuned()){
	  // just finished
	  mode = PLAY_MODE;
	}
      }
      break;
    }
    case PLAY_MODE: {      
      float note = (int)(getParameterValue(PARAMETER_B)*63) + allocator.getNote();
      cv = tuner.getNoteCV(note);
      // debugMessage("play", note, fc.getFrequency(), noteToFrequency(note));
      float measured = frequencyToNote(fc.getFrequency());
      debugMessage("play", note, measured, (measured-note)*100);
      setButton(PUSHBUTTON, allocator.getGate());
      break;
    }
    case LINEAR_MODE: {
      float note = (int)(getParameterValue(PARAMETER_B)*63) + allocator.getNote();
      cv = getLinearCVForNote(note);
      // debugMessage("lerp", note, fc.getFrequency(), noteToFrequency(note));
      float measured = frequencyToNote(fc.getFrequency());
      debugMessage("lerp", note, measured, (measured-note)*100);
      setButton(PUSHBUTTON, allocator.getGate());
      break;
    }
    }
    // left.setAll(cv*range1+offset1);
    // left.setAll(cv*(CH1_MAX-CH1_MIN)+CH1_MIN);
    right.setAll(cv*(CH2_MAX-CH2_MIN)+CH2_MIN);
    setParameterValue(PARAMETER_H, cv);
  }
};

#endif // __PolyVcoPatch_h__
