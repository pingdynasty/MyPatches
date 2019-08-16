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

class EmgPatch : public Patch {
private:
  StereoBiquadFilter* hp;
  StereoBiquadFilter* notch;
  float sr;
  EmgEnvelope env;
public:
  EmgPatch() {
    hp = StereoBiquadFilter::create(5);
    notch = StereoBiquadFilter::create(5);
    // initial filter settings
    hp->setHighPass(10.0f/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
    notch->setNotch(50.0f/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
    registerParameter(PARAMETER_A, "Gain");
    registerParameter(PARAMETER_B, "HP Fc");
    registerParameter(PARAMETER_C, "Notch Q");
    registerParameter(PARAMETER_D, "Env Alpha");
    // initialise parameter settings
    setParameterValue(PARAMETER_A, 0);
    setParameterValue(PARAMETER_B, 0);
    setParameterValue(PARAMETER_C, 0);
    setParameterValue(PARAMETER_D, 0);
  }
  ~EmgPatch() {
    StereoBiquadFilter::destroy(hp);
    StereoBiquadFilter::destroy(notch);
  }

  void processAudio(AudioBuffer &buffer) {
    float gain = getParameterValue(PARAMETER_A)*1000+1;
    float fc = getParameterValue(PARAMETER_B)*10+4;
    float q = getParameterValue(PARAMETER_C)*20+0.707;
    float alpha = getParameterValue(PARAMETER_D)*0.7+0.299;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hp->setHighPass(fc/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
    notch->setNotch(50.0f/(getSampleRate()/2), q);
    hp->process(buffer);
    notch->process(buffer);
    left.multiply(gain);
    right.multiply(gain);
    env.process(alpha, left.getData(), right.getData(), left.getSize());
    debugMessage("gain/range/rms", gain, abs(left.getMaxValue()-left.getMinValue()), left.getRms());
    left.clip();
    right.clip();
    MidiMessage msg;
    // msg = MidiMessage::cc(1, PATCH_PARAMETER_F, left[0]*128); // Control Change message
    msg = MidiMessage::pb(1, right[0]*8192); // Pitch Bend message
    sendMidi(msg);
  }
};

#endif   // __EmgPatch_hpp__
