#ifndef __MorphingOscillatorPatch_hpp__
#define __MorphingOscillatorPatch_hpp__

#include "OpenWareLibrary.h"

class MorphingOscillatorPatch : public Patch {
  typedef AntialiasedSquareWaveOscillator Osc;
private:
  MorphingOscillator* morph;
  Osc* osc;
public:
  MorphingOscillatorPatch(){
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Amplitude");
    registerParameter(PARAMETER_C, "Shape");
    registerParameter(PARAMETER_D, "Morph");
    osc = Osc::create(getSampleRate());
    morph = MorphingOscillator::create(4, getBlockSize());
    morph->setOscillator(0, AntialiasedTriangleOscillator::create(getSampleRate()));
    morph->setOscillator(1, AntialiasedSquareWaveOscillator::create(getSampleRate()));
    morph->setOscillator(2, AntialiasedRampOscillator::create(getSampleRate()));
    morph->setOscillator(3, SineOscillator::create(getSampleRate()));
  }
  ~MorphingOscillatorPatch(){
    for(size_t i=0; i<morph->getNumberOfOscillators(); ++i){
      delete morph->getOscillator(i);
    }
    MorphingOscillator::destroy(morph);
    Osc::destroy(osc);
  }
  void processAudio(AudioBuffer &buffer){
    float note = getParameterValue(PARAMETER_A)*60+32;
    float freq = 440*exp2f((note-69)/12);
    float gain = getParameterValue(PARAMETER_B);
    float pwm = getParameterValue(PARAMETER_C);
    float select = getParameterValue(PARAMETER_D);
    morph->morph(select);
    morph->setFrequency(freq);
    osc->setFrequency(freq);
    osc->setPulseWidth(pwm);
    
    debugMessage("freq/morph", freq, select);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    morph->generate(left);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    osc->generate(right);
    left.multiply(gain);
    right.multiply(gain);
    // left.copyFrom(right);
    // right.copyFrom(left);
  }
};

#endif   // __MorphingOscillatorPatch_hpp__
