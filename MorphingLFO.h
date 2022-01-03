#include "TapTempoOscillator.h"
#include "MorphingOscillator.h"


class MorphingLFO : public TapTempoOscillator<MorphingOscillator> {
public:
  // enum Shape { AGNESI, SINE, TRIANGLE, SQUARE, INVERTED_RAMP, RAMP, NOISE, NOF_SHAPES };
  // enum Shape { SINE, TRIANGLE, SQUARE, RAMP, NOISE, NOF_SHAPES };
  enum Shape { SINE, RAMP, SQUARE, INVERTED_RAMP, NOISE, NOF_SHAPES };
public:
  MorphingLFO(float sr, size_t min_limit, size_t max_limit, MorphingOscillator* osc): TapTempoOscillator(sr, min_limit, max_limit, osc) {}
  void select(Shape shape){
    morph((float)shape / (NOF_SHAPES - 1));
  }
  void morph(float value){
    oscillator->morph(value);
  }
  void reset(){
    oscillator->reset();
  }
  static MorphingLFO* create(float sample_rate, float min_hz, float max_hz, float block_rate, size_t blocksize){
    float rate = block_rate;
    MorphingOscillator* morph = MorphingOscillator::create(NOF_SHAPES, blocksize);
    // morph->setOscillator(AGNESI, AgnesiOscillator::create(rate, 0.5, 5));
    morph->setOscillator(SINE, PhaseShiftOscillator<SineOscillator>::create(0, rate));
    // morph->setOscillator(SINE, new PhaseShiftOscillator<SineOscillator>(0, rate));
    // morph->setOscillator(SINE, new PhaseShiftOscillator<SineOscillator>(-M_PI/2, rate));
    // morph->setOscillator(TRIANGLE, new PhaseShiftOscillator<TriangleOscillator>(0, rate));
    morph->setOscillator(RAMP, PhaseShiftOscillator<RampOscillator>::create(M_PI/2, rate));
    morph->setOscillator(SQUARE, PhaseShiftOscillator<SquareWaveOscillator>::create(-M_PI/2, rate));
    morph->setOscillator(INVERTED_RAMP, PhaseShiftOscillator<InvertedRampOscillator>::create(-M_PI/2, rate));
    morph->setOscillator(NOISE, NoiseOscillator::create(rate));
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
