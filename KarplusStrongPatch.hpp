#ifndef __KarplusStrongPatch_hpp__
#define __KarplusStrongPatch_hpp__

#include "StompBox.h"
#include "Oscillator.h"

class KarplusStrongOscillator : public Oscillator {
private:
  FloatArray noise; // whitenoise
  FloatArray pluck; // output y(n)
  int phase;
  int numSamps;
public:
  float duration;
  float gain; // filter gain/string tension/decay factor
  bool noteOn;
public:
  KarplusStrongOscillator(FloatArray nbf, FloatArray pbf){
    noise = nbf;
    pluck = pbf;
    // initalize phase
    phase = 0;
    noteOn = false;
  }

  // set normalised frequency (0.0 to 1.0 for DC to Nyquist)
  void setFrequency(float f){
    setWavelength(1.0/f);
  }

  void setWavelength(float f){
    numSamps = min(noise.getSize()-1, max(2, f*noise.getSize()));
  }

  void setDuration(float d){
    duration = min(pluck.getSize()-1, max(2, d*pluck.getSize()));
  }
  
  float getNextSample(){
    float sample;
    if(noteOn){
      if(phase > (numSamps +  1)){
	// if we have filled up our delay line, y(n) = gain * (y(n-N) + y( n-(N+1) ))
	pluck[phase] = gain * ( pluck[phase-numSamps] + pluck[phase - (numSamps + 1)] );
      // pluck can presumably be replaced with a circular buffer of same length as noise
      }else{
	// computing the first N samples, y(n) = x(n)
	pluck[phase] = noise[phase];
      }
      sample = pluck[phase];
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

  static KarplusStrongOscillator* create(int noise, int pluck){
    KarplusStrongOscillator* kp = new KarplusStrongOscillator(FloatArray::create(noise), FloatArray::create(pluck));
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
public:
  KarplusStrongPatch(){
    registerParameter(PARAMETER_A, "Freq");
    registerParameter(PARAMETER_B, "Amp");
    registerParameter(PARAMETER_C, "Tension");
    registerParameter(PARAMETER_D, "Duration");
    osc = KarplusStrongOscillator::create(1500, 1<<16);
  }

  void processAudio(AudioBuffer &buffer){
    float freq = getParameterValue(PARAMETER_A);
    float gain = getParameterValue(PARAMETER_C)*(0.5-0.48)+0.48;
    float duration = getParameterValue(PARAMETER_D);
    osc->setFrequency(freq);
    osc->gain = gain;
    osc->setDuration(duration);
    if(isButtonPressed(PUSHBUTTON) && !osc->noteOn)
      osc->noteOn = true;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    osc->getSamples(left);
  }
};

#endif // __KarplusStrongPatch_hpp__
