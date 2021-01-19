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
  static constexpr int POINTS_AVERAGE = 10;
  static constexpr float ZERO_THRESHOLD = 0.025;
  static constexpr float ZERO_OFFSET = 0.10;
// sync out on Magus audio in: -0.22 to 0.03
  bool done = false;
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
    float mean = counts.getMean();
    if(mean > 0.0) // avoid divide by zero
      return SR/mean;
    return FLT_EPSILON;
  }
  float getSpread(){
    if(!done)
      return FLT_MAX;
    return counts.getStandardDeviation();
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
  static constexpr float SETTLED_THRESHOLD = 5.0;
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
      if(fc.getSpread() < SETTLED_THRESHOLD){
	table[index++] = frequencyToNote(fc.getFrequency());
	// table[index++] = fc.getFrequency();
	fc.reset();
      }
    }
  }
  void begin(){
    index = 0;
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

  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    if(counter++ < 1000){
      setParameterValue(PARAMETER_H, 0); // make sure it settles before tuning
      return;
    }
    if(tuner.isTuned()){
      if(isButtonPressed(PUSHBUTTON)){
	int index = getParameterValue(PARAMETER_A)*TUNING_TABLE_SIZE;
	debugMessage("i/value", (float)index, tuner.getValue(index));
	setParameterValue(PARAMETER_H, (float)index/TUNING_TABLE_SIZE);
      }else{
	float note = (int)(getParameterValue(PARAMETER_A)*63) + basenote;
	float cv = tuner.getNoteCV(note);
	debugMessage("note/freq/cv", note, tuner.noteToFrequency(note), cv);
	setParameterValue(PARAMETER_H, cv);
      }
    }else{
      tuner.process(left);
      fc.process(left);
      float freq = fc.getFrequency();
      debugMessage("f/sd", freq, fc.getSpread());
      setParameterValue(PARAMETER_H, tuner.getTuningCV());
    }
    // debugMessage("freq/delta/target", freq, delta, target);
    // debugMessage("target/delta/cv", target, delta, cv);
  }
};

#endif // __PolyVcoPatch_h__
