#ifndef __EmgPatch_hpp__
#define __EmgPatch_hpp__

#include "Patch.h"
#include "BiquadFilter.h"

// #define SEND_XYZ_DATA
// #define SEND_ENV_DATA

#ifdef SEND_ENV_DATA
class EmgEnvelope {
  float ysquared = 0.0f;
public:
  float envelope(float alpha, float x){
    /* RMS detection with smoothing coefficient (Moving RMS) as per:
     * R. J. Cassidy, “Level Detection Tunings and Techniques for the Dynamic Range Compression of Audio Signals,” presented at the 117th Convention of the Audio Engineering Society (2004 Oct.), convention paper 6235.
     * Referenced in https://www.eecs.qmul.ac.uk/~josh/documents/2012/GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf
     */
    ysquared = alpha*ysquared + (1.0f-alpha)*x*x;
    return sqrtf(ysquared);
  }
  void process(float alpha, float* input, float* output, size_t len){
    for(size_t i=0; i<len; ++i)
      output[i] = envelope(alpha, input[i]);
  }
  // void process(float alpha, FloatArray input, FloatArray output){
};
#endif

class EmgSignalProcessor {
  const float sr;
  BiquadFilter* hpf;
  BiquadFilter* notch;
  BiquadFilter* lpf;
#ifdef SEND_ENV_DATA
  EmgEnvelope env;
#endif
public:
  EmgSignalProcessor(float sr, float notch_fc, float lp_fc): sr(sr){
    hpf = BiquadFilter::create(sr, 5);
    notch = BiquadFilter::create(sr, 5);
    lpf = BiquadFilter::create(sr, 5);
    // initial filter settings
    hpf->setHighPass(10.0f, FilterStage::BUTTERWORTH_Q);
    notch->setNotch(notch_fc, 8);
    lpf->setLowPass(lp_fc, FilterStage::BUTTERWORTH_Q);
  }
  ~EmgSignalProcessor(){
    BiquadFilter::destroy(hpf);
    BiquadFilter::destroy(notch);
    BiquadFilter::destroy(lpf);
  }
  void processRaw(FloatArray samples, float gain, float hpfc, float notchq){
    hpf->setHighPass(hpfc, FilterStage::BUTTERWORTH_Q);
    notch->setNotch(50.0f, notchq);
    hpf->process(samples);
    notch->process(samples);
    lpf->process(samples);
    samples.multiply(gain);
    samples.clip();
  }
  // void processEnv(FloatArray samples, float alpha){
  //   env.process(alpha, left.getData(), right.getData(), left.getSize());    
  // }
};

class EmgPatch : public Patch {
private:
  EmgSignalProcessor** processor = NULL;
  int channels = 0;
public:
  EmgPatch() {
    debugMessage("EMG v9 SR/BS/CH", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());
    channels = min(4, getNumberOfChannels());
    processor = new EmgSignalProcessor*[channels];
    for(int ch=0; ch<channels; ++ch)
      processor[ch] = new EmgSignalProcessor(getSampleRate(), 50, getSampleRate()/getBlockSize());
    registerParameter(PARAMETER_A, "Gain");
    registerParameter(PARAMETER_B, "HP Fc");
    registerParameter(PARAMETER_C, "Notch Q");
#ifdef SEND_ENV_DATA
    registerParameter(PARAMETER_D, "Env Alpha");
#endif
    // initialise parameter settings
    setParameterValue(PARAMETER_A, 0.25);
    setParameterValue(PARAMETER_B, 0.25);
    setParameterValue(PARAMETER_C, 0.25);
    setParameterValue(PARAMETER_D, 0);
  }
  ~EmgPatch() {
    for(int ch=0; ch<channels; ++ch)
      delete processor[ch];
    delete[] processor;
  }

  void processAudio(AudioBuffer &buffer) {
    float gain = getParameterValue(PARAMETER_A)*1000+1;
    float fc = getParameterValue(PARAMETER_B)*10+10;
    float q = getParameterValue(PARAMETER_C)*20+0.707;
    float alpha = getParameterValue(PARAMETER_D)*0.7+0.299;
#ifdef SEND_XYZ_DATA
    static bool flip = false;
    if(flip && channels<buffer.getChannels()){
      for(int ch=channels; ch<buffer.getChannels(); ++ch){
    	// accelerometer data (beyond channel 4)
    	FloatArray samples = buffer.getSamples(ch);
    	MidiMessage msg;
    	msg = MidiMessage::pb(1+ch, samples.getMean()*8192); // Pitch Bend accelerometer data
    	sendMidi(msg);      
      }
    }else{
#endif
      for(int ch=0; ch<channels; ++ch){
	FloatArray samples = buffer.getSamples(ch);
	processor[ch]->processRaw(samples, gain, fc, q);
	MidiMessage msg;
	msg = MidiMessage::pb(1+ch, samples.getMean()*8192); // Pitch Bend filtered EMG data
	sendMidi(msg);
#ifdef SEND_ENV_DATA
	processor[ch]->processEnv(samples, alpha);
	msg = MidiMessage::cc(1+ch, 1, samples.getMean()*128); // Control Change 1 envelope data
	sendMidi(msg);
#endif
      }
#ifdef SEND_XYZ_DATA
    }
    flip = !flip;
#endif
  }
};

#endif   // __EmgPatch_hpp__
