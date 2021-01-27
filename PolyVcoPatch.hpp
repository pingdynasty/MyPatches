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
  static constexpr int POINTS_AVERAGE = 10;
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
    float mean = counts.subArray(2, counts.getSize()-2).getMean(); // exclude first two measurements
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
  float getValue(int index){
    if(index < table.getSize())
      return table[index];
    return 0.0f;
  }
  float getTuningCV(){
    return float(index) / table.getSize();
  }
  float getNoteCV(float freq){
  //   return getFrequencyCV(noteToFrequency(freq));
  // }
  // float getFrequencyCV(float freq){
    float low = 0.0;
    for(size_t i=0; i<table.getSize(); ++i){
      float high = table[i];
      if(high < freq){
	low = high;
      }else{
	float offset = (freq - low)/(high - low);
	// debugMessage("l/h/o", low, high, offset);
	// return float(i)/table.getSize() + offset;
	return float(i-1+offset)/table.getSize();
	// return float(i)/table.getSize();
	// return (i+offset)/table.getSize();
      }
    }
    return 1.0;
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
	PLAY_MODE,
	TUNE_MODE
  };
  PatchMode mode = INIT_MODE;
  SmoothFloat freq;
  static constexpr int TUNING_TABLE_SIZE = 1024;
public:
  PolyVcoPatch() : fc(getSampleRate()), tuner(TUNING_TABLE_SIZE, getSampleRate()) {
    registerParameter(PARAMETER_A, "Tune");
    setParameterValue(PARAMETER_A, 0);
    registerParameter(PARAMETER_H, "Tune>");
    // debugMessage("PolyVco SR/BS/CH", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());
  }

  void processMidi(MidiMessage msg){
    switch (msg.getStatus()) {
    case 0x80:
    case 0x90:
      basenote = msg.getNote();
      break;
    }
  }

  int counter = 0;
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      if(value){
	counter = 0;
	tuner.begin();
      }
      break;
    }
  }

  static constexpr float range1 = -2.00;
  static constexpr float offset1 = 0.99;
  static constexpr float range3 = 0.8;
  static constexpr float offset3 = -0.5;
  
  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(0);
    FloatArray right = buffer.getSamples(2); // channel 3
    if(isButtonPressed(BUTTON_B)){
      debugMessage("max/rms/min", left.getMaxValue(), left.getRms(), left.getMinValue());
      return;
    }
    fc.process(left);
    if(isButtonPressed(BUTTON_C)){
      float cv = getParameterValue(PARAMETER_A);
      debugMessage("cv/freq", cv, fc.getFrequency());
      left.setAll(cv*range1+offset1);
      right.setAll(cv*range3+offset3);
      return;
    }
    if(tuner.isTuned()){
      if(isButtonPressed(PUSHBUTTON)){
    	int index = getParameterValue(PARAMETER_A)*TUNING_TABLE_SIZE;
	float cv = float(index)/TUNING_TABLE_SIZE;
    	debugMessage("i/value/freq", (float)index, tuner.getValue(index), fc.getFrequency());
    	setParameterValue(PARAMETER_H, cv);
	left.setAll(cv*range1+offset1);
	right.setAll(cv*range3+offset3);
      }else{
    	float note = (int)(getParameterValue(PARAMETER_A)*63) + basenote;
    	float cv = tuner.getNoteCV(note);
    	debugMessage("note/freq/real", note, tuner.noteToFrequency(note), fc.getFrequency());
    	setParameterValue(PARAMETER_H, cv);
	left.setAll(cv*range1+offset1);
	right.setAll(cv*range3+offset3);
      }
    }else{
      tuner.process(left);
      float freq = fc.getFrequency();
      float cv = tuner.getTuningCV();
      debugMessage("f/cv/sd", freq, cv, fc.getSpread());
      setParameterValue(PARAMETER_H, cv);
      left.setAll(cv*range1+offset1);
      right.setAll(cv*range3+offset3);
    }
    left.clip(-1, 0.99); // SampleBuffer saturate not working
  }
};

#endif // __PolyVcoPatch_h__
