#ifndef __PolyVcoPatch_h__
#define __PolyVcoPatch_h__

#include <cfloat>
#include "Patch.h"
#include "PitchDetector.h"
#include "SmoothValue.h"
#include "VoltsPerOctave.h"

class FrequencyCounter {
private:
  const float SR;
  FloatArray counts;
  size_t index = 0;
  size_t lastCrossing = 0;
  bool positive = false;
  bool done = false;
  static constexpr int POINTS_AVERAGE = 4;
  static constexpr float ZERO_THRESHOLD = 0.025;
  static constexpr float ZERO_OFFSET = -1.00;
// sync out on Magus audio in: -0.22 to 0.03
// sync out on Noctua audio in: 0 to 1.99
  static constexpr float SPREAD_THRESHOLD = 6.0;
public:
  FrequencyCounter(float SR) : SR(SR) {
    counts = FloatArray::create(POINTS_AVERAGE); //number of zcc to be averaged
    reset();
  }
  ~FrequencyCounter() {
    FloatArray::destroy(counts);
  }
  void process(FloatArray input){
    size_t len = input.getSize();
    for(size_t i=0; i<len; i++){
      float sample = input[i] + ZERO_OFFSET;
      if(positive && (sample < -ZERO_THRESHOLD)){
	counts[index++] = i - lastCrossing;
        if(index == counts.getSize()){
	  index = 0;
	  done = true;
	}
	lastCrossing = i;
	positive = false;
      }else if(!positive && (sample > ZERO_THRESHOLD)){
	positive = true;	
      }
    }
    lastCrossing -= len;
  }
  float getFrequency(){
    float mean = counts.subArray(1, counts.getSize()-1).getMean(); // exclude first two measurements
    if(mean > 0.0) // avoid divide by zero
      return SR/mean;
    return FLT_EPSILON;
  }
  float getSpread(){
    return counts.getStandardDeviation();
  }
  bool complete(){
    // return done && getSpread() < SPREAD_THRESHOLD;
    return done;
  }
  void reset(){
    index = 0;
    done = false;
  }
};

class Tuner {
private:
  FrequencyCounter fc;
  FloatArray table;
  size_t index = 0;
public:
  Tuner(size_t len, float SR) : fc(SR) {
    table = FloatArray::create(len);
  }
  ~Tuner(){
    FloatArray::destroy(table);
  }
  void process(FloatArray input){
    if(index < table.getSize()){
      fc.process(input);
      if(fc.complete()){
	table[index++] = frequencyToNote(fc.getFrequency());
	// table[index++] = fc.getFrequency();
	fc.reset();
      }
    }
  }
  void begin(){
    index = 0;
    fc.reset();
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
    return 0.0f;
  }
  float getFrequencyForCV(float cv){
    return noteToFrequency(getValue(cv*table.getSize()));
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
      if(high < note){
	low = high;
      }else{
	float offset = (note - low)/(high - low);
	// debugMessage("l/h/o", low, high, offset);
	// return float(i)/table.getSize() + offset;
	return float(i-1+offset)/table.getSize();
	// return float(i)/table.getSize();
	// return (i+offset)/table.getSize();
      }
    }
    return 1;
  }
  static float noteToFrequency(float m){
    return 440*exp2f((m-69)/12);
  }
  static float frequencyToNote(float f){
    return 12*log2f(f/440.0)+69;
  }
};

class PolyVcoPatch : public Patch {
  FrequencyCounter fc;
  Tuner tuner;
  SmoothFloat cv;
  int basenote = 40;
  enum PatchMode {
	INIT_MODE,
	CTRL_MODE,
	PLAY_MODE,
	TUNE_MODE
  };
  PatchMode mode = INIT_MODE;
  SmoothFloat freq;
  static constexpr int TUNING_TABLE_SIZE = 1024;
public:
  PolyVcoPatch() : fc(getSampleRate()), tuner(TUNING_TABLE_SIZE, getSampleRate()) {
    registerParameter(PARAMETER_A, "CV");
    setParameterValue(PARAMETER_A, 0);
    registerParameter(PARAMETER_B, "Note");
    setParameterValue(PARAMETER_B, 0);
    registerParameter(PARAMETER_H, "Tune>");
    // debugMessage("PolyVco SR/BS/CH", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());
  }

  void processMidi(MidiMessage msg){
    switch (msg.getStatus()) {
    case NOTE_ON:
    case NOTE_OFF:
      basenote = msg.getNote();
      break;
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
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      mode = CTRL_MODE;
      break;
    case BUTTON_B:
      mode = INIT_MODE;
      break;
    case BUTTON_C:
      mode = PLAY_MODE;      
      break;
    case BUTTON_D:
      if(value){
	tuner.begin();
	mode = INIT_MODE;
      }
      break;
    }
  }

  static constexpr float range1 = -2.00;
  static constexpr float offset1 = 1.00;
  static constexpr float range3 = 1.0;
  static constexpr float offset3 = 0.0;
  
  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(0);
    FloatArray right = buffer.getSamples(2); // channel 3
    float cv = 0;
    fc.process(left);
    switch(mode){
    case INIT_MODE: {
      mode = TUNE_MODE;
      break;
    }
    case CTRL_MODE: {
      cv = getParameterValue(PARAMETER_A);
      debugMessage("ctrl", cv, fc.getFrequency(), tuner.getFrequencyForCV(cv));
      break;
    }
    case TUNE_MODE: {
      if(tuner.isTuned()){
	cv = getParameterValue(PARAMETER_A);
	debugMessage("tuned", cv, fc.getFrequency(), tuner.getFrequencyForCV(cv));
      }else{
	tuner.process(left);
	cv = tuner.getTuningCV();
	debugMessage("tuning", cv, fc.getFrequency(), fc.getSpread());
	if(tuner.isTuned()) // just finished
	  mode = PLAY_MODE;
      }
      break;
    }
    case PLAY_MODE: {
      float note = (int)(getParameterValue(PARAMETER_B)*63) + basenote;
      cv = tuner.getNoteCV(note);
      debugMessage("play", note, fc.getFrequency(), tuner.noteToFrequency(note));
      break;
    }
    }
    setParameterValue(PARAMETER_H, cv);
    left.setAll(cv*range1+offset1);
    right.setAll(cv*range3+offset3);
  }
};

#endif // __PolyVcoPatch_h__
