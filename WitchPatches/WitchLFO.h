#include "TapTempoOscillator.h"

class WitchLFO : public TapTempoOscillator<MorphingOscillator> {
public:
  enum Shape { AGNESI, SINE, TRIANGLE, RAMP, SQUARE, NOF_SHAPES };
public:
  WitchLFO(float sr, size_t limit, MorphingOscillator* osc): TapTempoOscillator(sr, limit, osc) {}
  void select(Shape shape){
    select(shape / NOF_SHAPES);
  }
  void select(float value){
    oscillator->morph(value);
  }
  void reset(){
    oscillator->reset();
  }
  static WitchLFO* create(float sample_rate, size_t limit, float block_rate, size_t block_size){
    float rate = block_rate;
    MorphingOscillator* morph = MorphingOscillator::create(NOF_SHAPES, block_size);
    morph->setOscillator(0, AgnesiOscillator::create(rate));
    morph->setOscillator(1, SineOscillator::create(rate));
    morph->setOscillator(2, TriangleOscillator::create(rate));
    morph->setOscillator(3, RampOscillator::create(rate));
    morph->setOscillator(4, InvertedSquareWaveOscillator::create(rate));
    return new WitchLFO(sample_rate, limit, morph);
  }
  static void destroy(WitchLFO* obj){
    MorphingOscillator* morph = (MorphingOscillator*)obj->oscillator;
    for(size_t i=0; i<morph->getNumberOfOscillators(); ++i)
      delete morph->getOscillator(i);
    MorphingOscillator::destroy(morph);
    delete obj;
  }
};
