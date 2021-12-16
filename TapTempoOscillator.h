#include "TapTempo.h"
#include "SignalGenerator.h"
#include "Oscillator.h"

template<class T>
class TapTempoOscillator : public AdjustableTapTempo, public SignalGenerator {
protected:
  T* oscillator;
public:
  TapTempoOscillator(float sr, size_t limit, T* osc): AdjustableTapTempo(sr, limit), oscillator(osc) {}
  void reset(){
    oscillator->reset();
  }
  // void setFrequency(float value){
  //   oscillator->setFrequency(value);
  // }
  float getPhase(){
    return oscillator->getPhase();
  }
  void setPhase(float phase){
    oscillator->setPhase(phase);
  }
  float generate(){
    oscillator->setFrequency(getFrequency());
    return oscillator->generate();
  }
  void generate(FloatArray output){
    oscillator->setFrequency(getFrequency());
    oscillator->generate(output);
  }
  T* getOscillator(){
    return oscillator;
  }
  static TapTempoOscillator* create(float sample_rate, size_t limit, float block_rate){
    return new TapTempoOscillator(sample_rate, limit, T::create(block_rate));
  }
  static void destroy(TapTempoOscillator* obj){
    T::destroy(obj->oscillator);
    delete obj;
  }
};

typedef TapTempoOscillator<SineOscillator> TapTempoSineOscillator;
typedef TapTempoOscillator<AgnesiOscillator> TapTempoAgnesiOscillator;
