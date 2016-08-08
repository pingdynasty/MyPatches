#ifndef __RandomLfoPatch_hpp__
#define __RandomLfoPatch_hpp__

#include "Patch.h"
#include "NoiseOscillator.h"
#include "VoltsPerOctave.h"

#define P1      0
#define m2      1
#define M2      2
#define m3      3
#define M3      4
#define P4      5
#define d5      6
#define A4      6
#define P5      7
#define m6      8
#define M6      9
#define m7      10
#define M7      11
#define P8      0
#define SEMI    1
#define WHOLE   2

class RandomLfoPatch : public Patch {
  const int scales[3][12] = {
    {P1, P1, M2, M2, M3, P4, P4, P5, P5, M6, M6, M7 }, // Major scale
    {P1, P1, M2, m3, m3, P4, P4, P5, m6, m6, M7, M7 }, // Harmonic Minor scale
    {P1, P1, M2, m3, m3, P4, P4, P5, P5, M6, M7, M7 }, // Melodic Minor scale
  };
public:
  const int maxrate;
  const int minrate;
  Oscillator* noise;
  float last, target;
  int step;
  FloatParameter range;
  IntParameter rate;
  VoltsPerOctave hz;
  IntParameter scale;
  IntParameter offset;
  RandomLfoPatch() : maxrate(getSampleRate()*2),
		     minrate(getSampleRate()/2000) {
    noise = WhiteNoiseOscillator::create();
    range = getFloatParameter("Range", 0.01, 1.0, 1.0);
    rate = getIntParameter("Rate", 0, maxrate, 0);
    scale = getIntParameter("Scale", 0, 5, 0);
    offset = getIntParameter("Offset", 0, 12, 0);
    target = last = 0.0f;
    step = 0;
  }
  float getIncrement(){
    if(rate < minrate)
      return 0.0f;
    return (last-target)/(maxrate-rate);
  }
  float quantize(float volts){
    switch(scale){
    case 0:
      return volts; // no quantization
    case 1:
      return round(volts*12)/12.0f;
    default:
      int octave = (int)volts;
      int semitone = (int)round(volts*12) % 12;
      return scales[scale-2][semitone] + octave + offset;
    }
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON && value){
      // skip straight to target
      last = target;
      step = 0;
    }
  }  
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    for(int i = 0; i<buffer.getSize(); i++){
      if(++step >= (maxrate-rate)){
	last = target;
	target = noise->getNextSample()*range;
	step = 0;
      }
      left[i] = last;
      last += getIncrement();
      right[i] = hz.voltsToSample(quantize(hz.sampleToVolts(right[i])));
    }
  }
};

#endif   // __RandomLfoPatch_hpp__
