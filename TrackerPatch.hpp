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
    freq.lambda = SmoothFloat::normal(portamento, output.getSize());
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
    float rms = samples.getRms();
    if(rms > 0.1){
      gain = rms*1.66;
      zcc.process(samples);
      freq = zcc.getFrequency()*tune;
      samples.multiply(1.0-mix);
      osc.setFrequency(freq);
      osc.getSamples(output);
      output.multiply(mix*gain);
      samples.add(output);
    }else{
      samples.multiply(1.0-mix);
    }
  }
};

class TrackerPatch : public Patch {
private:
  TrackerVoice left, right;
  FloatParameter tune, glide, shape, mix, pitch;
public:
  TrackerPatch() :
    left(getSampleRate(), getBlockSize()),
    right(getSampleRate(), getBlockSize()) {
    tune = getFloatParameter("Tune", 0.5, 2.0, 1.0); // up/down 1 octave
    glide = getFloatParameter("Glide", 0.8, 1.0, 0.9, 0.0, 0.0, EXP);
    shape = getFloatParameter("Shape", 0, 2);
    mix = getFloatParameter("Dry/Wet", 0, 1);
    pitch = getFloatParameter("Pitch", 0, 1); // pitch up one more octave
  }      
  void processAudio(AudioBuffer &buffer) {
    left.setPortamento(glide);
    right.setPortamento(glide);
    left.setShape(shape);
    right.setShape(shape);
    debugMessage("tune/glide/shape", tune, glide, shape);
    FloatArray lbuf = buffer.getSamples(LEFT_CHANNEL);
    FloatArray rbuf = buffer.getSamples(RIGHT_CHANNEL);
    left.process(tune+pitch, mix, lbuf);
    right.process(tune+pitch, mix, rbuf);
  }
};

#endif   // __TrackerPatch_hpp__
