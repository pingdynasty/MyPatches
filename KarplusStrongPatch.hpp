#ifndef __KarplusStrongPatch_hpp__
#define __KarplusStrongPatch_hpp__

#include "StompBox.h"
#include "Oscillator.h"
#include "CircularBuffer.hpp"

class KarplusStrongOscillator : public Oscillator {
private:
  FloatArray noise; // whitenoise
  CircularBuffer* pluck; // output y(n)
  int phase;
  int numSamps;
public:
  int duration;
  float gain; // filter gain/string tension/decay factor
  bool noteOn;
public:
  KarplusStrongOscillator(FloatArray nbf, CircularBuffer* pbf){
    noise = nbf;
    pluck = pbf;
    // initalize phase
    phase = 0;
    noteOn = false;
  }

  void setFrequency(float f){
    setWavelength(1.0/f);
  }

  // set normalised wavelength
  void setWavelength(float f){
    numSamps = min(noise.getSize()-1, max(2, f*noise.getSize()));
  }
  
  float getNextSample(){
    float sample = 0.0f;
    if(noteOn){
      if(phase > (numSamps +  1)){
	// if we have filled up our delay line, y(n) = gain * (y(n-N) + y( n-(N+1) ))
	sample = gain * ( pluck->read(numSamps) + pluck->read(numSamps + 1));
      }else{
	// computing the first N samples, y(n) = x(n)
	sample = noise[phase];
      }
      pluck->write(sample);
      if(phase >= duration){
	// if we have reached the end of our duration
	phase = 0;
	noteOn = false;
      }else{
	phase++;
      }
    }
    return sample;
  }

  static void destroy(KarplusStrongOscillator* kp){
    FloatArray::destroy(kp->noise);
    CircularBuffer::destroy(kp->pluck);
    delete kp;
  }

  static KarplusStrongOscillator* create(int size){
    KarplusStrongOscillator* kp = new KarplusStrongOscillator(FloatArray::create(size),
							      CircularBuffer::create(size));
    // kp.noiseType = KP_NOISETYPE_GAUSSIAN;

    //generate white gaussian noise:
    // from http://www.musicdsp.org/showone.php?id=168

    /* Setup constants */
    const static int q = 15;
    const static float c1 = (1 << q) - 1;
    const static float c2 = ((int)(c1 / 3)) + 1;
    const static float c3 = 1.f / c1;
    float max = 0;

    /* random number in range 0 - 1 not including 1 */
    float random = 0.f;
    for(int i = 0; i < kp->noise.getSize(); i++){
      random = ((float)rand() / (float)RAND_MAX);
      kp->noise[i] = (2.f * ((random * c2) + (random * c2) + (random * c2)) - 3.f * (c2 - 1.f)) * c3;	  
      if(fabs(kp->noise[i]) > max)
	max = fabs(kp->noise[i]);
    }
    for (int i = 0; i < kp->noise.getSize(); i++){
      //normalize the gain of our noise
      kp->noise[i] = kp->noise[i] / max;
    }
    return kp;
  }
};

// number of samples for delay line, 1000 gives us a min. possible freq of 44100/1000 = 29.4hz (sufficient)
// #define KP_NUM_SAMPLES (1500)
// #define KP_NUM_BUFFER (1<<16)
// #define KP_NOISETYPE_GAUSSIAN 1
// #define KP_NOISETYPE_RANDOM 2

// typedef struct {
//   int numSamps; // N
//   float noise[KP_NUM_SAMPLES];  // white noise x(n)
//   float pluck[KP_NUM_BUFFER]; // output y(n)
//   float amp;
//   float duration;
//   int phase;
//   float g; // filter gain/string tension/decay factor
//   bool noteOn;
//   uint8_t noiseType;
// } KarplusData;

class KarplusStrongPatch : public Patch {
private:
  KarplusStrongOscillator* osc;
  int maxDuration;
public:
  KarplusStrongPatch(){
    registerParameter(PARAMETER_A, "Freq");
    registerParameter(PARAMETER_B, "Amp");
    registerParameter(PARAMETER_C, "Tension");
    registerParameter(PARAMETER_D, "Duration");
    osc = KarplusStrongOscillator::create(2048);
    maxDuration = getSampleRate()*3; // 3 seconds
  }

  void processAudio(AudioBuffer &buffer){
    float freq = getParameterValue(PARAMETER_A);
    // float gain = getParameterValue(PARAMETER_C)*(0.5-0.48)+0.48;
    float gain = getParameterValue(PARAMETER_C)*(0.5-0.48)+0.48;
    int duration = getParameterValue(PARAMETER_D)*maxDuration;
    osc->setWavelength(1.0-freq);
    osc->gain = gain;
    osc->duration = duration;
    if(isButtonPressed(PUSHBUTTON) && !osc->noteOn)
      osc->noteOn = true;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    osc->getSamples(left);
  }
};

#endif // __KarplusStrongPatch_hpp__
