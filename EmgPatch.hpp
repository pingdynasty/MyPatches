#ifndef __EmgPatch_hpp__
#define __EmgPatch_hpp__

#include "Patch.h"
#include "BiquadFilter.h"

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

class EmgSignalProcessor {
  const float sr;
  BiquadFilter* hp;
  BiquadFilter* notch;
  // EmgEnvelope env;
public:
  EmgSignalProcessor(float sr): sr(sr){
    hp = BiquadFilter::create(5);
    notch = BiquadFilter::create(5);
    // initial filter settings
    hp->setHighPass(10.0f/(sr/2), FilterStage::BUTTERWORTH_Q);
    notch->setNotch(50.0f/(sr/2), 8);
  }
  ~EmgSignalProcessor(){
    BiquadFilter::destroy(hp);
    BiquadFilter::destroy(notch);
  }
  void processRaw(FloatArray samples, float gain, float hpfc, float notchq){
    hp->setHighPass(hpfc/(sr/2), FilterStage::BUTTERWORTH_Q);
    notch->setNotch(50.0f/(sr/2), notchq);
    hp->process(samples);
    notch->process(samples);
    samples.multiply(gain);
    samples.clip();
  }
  // void processEnv(FloatArray samples, float alpha){
  //   env.process(alpha, left.getData(), right.getData(), left.getSize());    
  // }
};

class EmgPatch : public Patch {
private:
  EmgSignalProcessor** processor;
public:
  EmgPatch() {
    processor = new EmgSignalProcessor*[4];
    for(int ch=0; ch<getNumberOfChannels(); ++ch)
      processor[ch] = new EmgSignalProcessor(getSampleRate());
    registerParameter(PARAMETER_A, "Gain");
    registerParameter(PARAMETER_B, "HP Fc");
    registerParameter(PARAMETER_C, "Notch Q");
    registerParameter(PARAMETER_D, "Env Alpha");
    // initialise parameter settings
    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0);
  }
  ~EmgPatch() {
    for(int ch=0; ch<getNumberOfChannels(); ++ch){
      delete processor[ch];
    }
    delete[] processor;
  }

  void processAudio(AudioBuffer &buffer) {
    float gain = getParameterValue(PARAMETER_A)*1000+1;
    float fc = getParameterValue(PARAMETER_B)*10+4;
    float q = getParameterValue(PARAMETER_C)*20+0.707;
    float alpha = getParameterValue(PARAMETER_D)*0.7+0.299;
    for(int ch=0; ch<buffer.getChannels(); ++ch){
      FloatArray samples = buffer.getSamples(ch);
      processor[ch]->processRaw(samples, gain, fc, q);
      MidiMessage msg;
      msg = MidiMessage::pb(1+ch, samples.getMean()*8192); // Pitch Bend filtered EMG 
      sendMidi(msg);
      // processor[ch]->processEnv(samples);
      // msg = MidiMessage::cc(1, 1, samples.getMean()*128); // Control Change envelope
      // sendMidi(msg);
    }
    debugMessage("sr/bs/ch", (int)getSampleRate(), getBlockSize(), buffer.getChannels());
  }
};

#endif   // __EmgPatch_hpp__
