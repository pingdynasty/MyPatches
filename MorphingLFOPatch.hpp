#ifndef __MorphingLFOPatch_h__
#define __MorphingLFOPatch_h__

#include "Patch.h"
#include "OpenWareLibrary.h"
#include "MorphingLFO.h"

#define TRIGGER_LIMIT (1<<22)

class MorphingLFOPatch : public Patch {
  MorphingLFO* lfo;
  MorphingLFO* osc;
  Oscillator* ramp;
  static constexpr float LFO_MIN_HZ = 0.1;
  static constexpr float LFO_MAX_HZ = 40;
  static constexpr float LFO_DEFAULT_PBM = 60;
public:
  MorphingLFOPatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Shape");
    lfo = MorphingLFO::create(getSampleRate(), LFO_MIN_HZ, LFO_MAX_HZ, getBlockRate(), getBlockSize());
    lfo->setBeatsPerMinute(LFO_DEFAULT_PBM);
    setParameterValue(PARAMETER_B, (float)MorphingLFO::SINE/(MorphingLFO::NOF_SHAPES-1));
    osc = MorphingLFO::create(getSampleRate(), LFO_MIN_HZ, LFO_MAX_HZ, getSampleRate(), getBlockSize());
    osc->setBeatsPerMinute(LFO_DEFAULT_PBM);
    ramp = new RampOscillator(getSampleRate());
    lfo->select(MorphingLFO::SINE);
    osc->select(MorphingLFO::SINE);
    lfo->reset();
    osc->reset();
    ramp->reset();
  }
  ~MorphingLFOPatch(){
    MorphingLFO::destroy(lfo);
    MorphingLFO::destroy(osc);
    delete ramp;
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      lfo->trigger(value, samples);
      osc->trigger(value, samples);
      if(value){
	lfo->reset();
	osc->reset();
	ramp->reset();
      }
      break;
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float speed = getParameterValue(PARAMETER_A) - getParameterValue(PARAMETER_E);
    speed = clamp(speed, 0.0f, 1.0f);
    // float speed = getParameterValue(PARAMETER_A);
    float shape = getParameterValue(PARAMETER_B);
    lfo->morph(shape);
    lfo->clock(getBlockSize());
    lfo->adjustSpeed(speed);
    float output = lfo->generate() * 0.5 + 0.5;
    setParameterValue(PARAMETER_F, output);
    setParameterValue(PARAMETER_G, 1 - output);
    shape *= MorphingLFO::NOF_SHAPES - 1;
    debugMessage("shape/fmod/rem", shape, fmodf(shape, 1), remainderf(shape, 1));
    if(abs(remainderf(shape, 1)) < 0.06){
      setButton(GREEN_BUTTON, 4096 * output);
    }else{
      setButton(RED_BUTTON, 4096 * output);
    }
    // setButton(PUSHBUTTON, lfo->getPhase() < M_PI);
    osc->morph(getParameterValue(PARAMETER_B));
    osc->clock(getBlockSize());
    osc->adjustSpeed(speed);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    osc->generate(left);
    ramp->setFrequency(osc->getFrequency());
    ramp->generate(right);
  }
};

#endif // __MorphingLFOPatch_h__
