#ifndef __TrackerPatch_hpp__
#define __TrackerPatch_hpp__

#include "StompBox.h"
#include "PitchDetector.h"
#include "SmoothValue.h"
#include "PolyBlepOscillator.h"

/**
 * Mono patch that tracks the frequency of the audio input and produces a matching tone on the output.
 * With variable waveform shape (tri to PWM square), pitch offset, and portamento/glide.
 */
class TrackerVoice {
private:
  PolyBlepOscillator osc;
  ZeroCrossingPitchDetector zcc;
  SmoothFloat freq;
  SmoothFloat gain;
  FloatArray output;
public:
  TrackerVoice(float samplerate, int blocksize) :
    osc(samplerate), 
    zcc(samplerate, blocksize),
    freq(0.9, 440.0),
    gain(0.9) {
    zcc.setLowPassCutoff(40);
    zcc.setHighPassCutoff(8000);
    output = FloatArray::create(blocksize);
  }      
  ~TrackerVoice(){
    FloatArray::destroy(output);
  }
  void setPortamento(float portamento){
    freq.lambda = portamento;
  }
  void setShape(float shape){
    if(shape > 1.0){
      osc.setShape(1.0);
      osc.setPulseWidth(0.48*shape);
    }else{
      osc.setShape(shape);
      osc.setPulseWidth(0.48);
    }
  }
  void process(float tune, float mix, FloatArray samples) {
    zcc.process(samples);
    freq = zcc.getFrequency()*tune;
    float rms = samples.getRms();
    if(rms > 0.1)
      gain = rms*1.66;
    else
      gain = 0.0;
    samples.multiply(1.0-mix);
    osc.setFrequency(freq);
    osc.getSamples(output);
    output.multiply(mix*gain);
    samples.add(output);
  }
};

class TrackerPatch : public Patch {
private:
  TrackerVoice left, right;
public:
  TrackerPatch() :
    left(getSampleRate(), getBlockSize()),
    right(getSampleRate(), getBlockSize()) {
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Glide");
    registerParameter(PARAMETER_C, "Shape");
    registerParameter(PARAMETER_D, "Dry/Wet");
    registerParameter(PARAMETER_E, "Pitch");
  }      
  void processAudio(AudioBuffer &buffer) {
    float portamento = getParameterValue(PARAMETER_B)*0.1+0.9;
    float tune = getParameterValue(PARAMETER_A)*1.5+0.5; // up/down 1 octave
    float shape = getParameterValue(PARAMETER_C)*2;
    float mix = getParameterValue(PARAMETER_D);
    tune += getParameterValue(PARAMETER_E); // pitch up by up to one octave
    left.setPortamento(portamento);
    right.setPortamento(portamento);
    left.setShape(shape);
    right.setShape(shape);
    FloatArray lbuf = buffer.getSamples(LEFT_CHANNEL);
    FloatArray rbuf = buffer.getSamples(RIGHT_CHANNEL);
    left.process(tune, mix, lbuf);
    right.process(tune, mix, rbuf);
  }
};

#endif   // __TrackerPatch_hpp__
