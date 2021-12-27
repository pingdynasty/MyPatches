#include "TapTempoOscillator.h"
#include "MorphingOscillator.h"

class NoiseOscillator : public OscillatorTemplate<NoiseOscillator> {
protected:
  float sample = 0;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  NoiseOscillator(){}
  NoiseOscillator(float sr){
    setSampleRate(sr);
  }  
  float getSample(){
    return sample;
  }
  float generate() {
    phase += incr;
    if(phase >= end_phase){
      sample = randf()*2 - 1;
      phase -= 1;
    }
    return sample;
  }
  using OscillatorTemplate<NoiseOscillator>::generate;  
  void reset(){
    sample = randf()*2 - 1;
    phase = 0;
  }
  using SignalGenerator::generate;
  static NoiseOscillator* create(){
    return new NoiseOscillator();
  }
  static void destroy(NoiseOscillator* osc){
    delete osc;
  }
};

template<class Osc>
class PhaseShiftOscillator : public Osc {
protected:
  float phaseshift;
public:
  template <typename... Args>
  PhaseShiftOscillator(float phaseshift, Args&&... args) :
    Osc(std::forward<Args>(args)...), phaseshift(phaseshift) {}
  void setPhase(float phase){
    Osc::setPhase(phase +  phaseshift);
  }
  float getPhase(){
    return Osc::getPhase() - phaseshift;
  }
  void reset(){
    Osc::setPhase(phaseshift);
  }  
};


class MorphingLFO : public TapTempoOscillator<MorphingOscillator> {
public:
  // enum Shape { AGNESI, SINE, TRIANGLE, SQUARE, INVERTED_RAMP, RAMP, NOISE, NOF_SHAPES };
  // enum Shape { SINE, TRIANGLE, SQUARE, RAMP, NOISE, NOF_SHAPES };
  enum Shape { SINE, RAMP, SQUARE, INVERTED_RAMP, NOISE, NOF_SHAPES };
public:
  MorphingLFO(float sr, size_t min_limit, size_t max_limit, MorphingOscillator* osc): TapTempoOscillator(sr, min_limit, max_limit, osc) {}
  void select(Shape shape){
    select((float)shape / (NOF_SHAPES - 1));
  }
  void select(float value){
    oscillator->select(value);
  }
  void reset(){
    oscillator->reset();
  }
  static MorphingLFO* create(float sample_rate, float min_hz, float max_hz, float block_rate){
    float rate = block_rate;
    MorphingOscillator* morph = MorphingOscillator::create(NOF_SHAPES);
    // morph->setOscillator(AGNESI, AgnesiOscillator::create(rate, 0.5, 5));
    morph->setOscillator(SINE, new PhaseShiftOscillator<SineOscillator>(0, rate));
    // morph->setOscillator(SINE, new PhaseShiftOscillator<SineOscillator>(-M_PI/2, rate));
    // morph->setOscillator(TRIANGLE, new PhaseShiftOscillator<TriangleOscillator>(0, rate));
    morph->setOscillator(RAMP, new PhaseShiftOscillator<RampOscillator>(M_PI/2, rate));
    morph->setOscillator(SQUARE, new PhaseShiftOscillator<SquareWaveOscillator>(-M_PI/2, rate));
    morph->setOscillator(INVERTED_RAMP, new PhaseShiftOscillator<InvertedRampOscillator>(-M_PI/2, rate));
    morph->setOscillator(NOISE, new NoiseOscillator(rate));
    return new MorphingLFO(sample_rate, sample_rate/max_hz, sample_rate/min_hz, morph);
  }
  static void destroy(MorphingLFO* obj){
    MorphingOscillator* morph = (MorphingOscillator*)obj->oscillator;
    for(size_t i=0; i<morph->getNumberOfOscillators(); ++i)
      delete morph->getOscillator(i);
    MorphingOscillator::destroy(morph);
    delete obj;
  }
};
