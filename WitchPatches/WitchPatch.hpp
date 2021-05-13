

class TapTempoOscillator : public TapTempo, public SignalGenerator {
protected:
  Oscillator* oscillator;
public:
  TapTempoOscillator(float sr, size_t limit, Oscillator* osc): TapTempo(sr, limit), oscillator(osc) {}
  
  float generate(){
    oscillator->setFrequency(getFrequency());
    return oscillator->generate();
  }
  float getPhase(){
    return oscillator->getPhase();
  }
  void generate(FloatArray output){
    oscillator->setFrequency(getFrequency());
    oscillator->generate(output);
  }

  static TapTempoOscillator* create(float sr, size_t limit, Oscillator* osc){
    return new TapTempoOscillator(sr, limit, osc);
  }
};

class TapTempoSineOscillator : public TapTempoOscillator {
public:
  TapTempoSineOscillator(float sr, size_t limit, Oscillator* osc): TapTempoOscillator(sr, limit, osc) {}
  static TapTempoSineOscillator* create(float sample_rate, size_t limit, float block_rate){
    return new TapTempoSineOscillator(sample_rate, limit, SineOscillator::create(block_rate));
  }
  static void destroy(TapTempoSineOscillator* obj){
    SineOscillator::destroy((SineOscillator*)obj->oscillator);
    delete obj;
  }
};

class TapTempoAgnesiOscillator : public TapTempoOscillator {
public:
  TapTempoAgnesiOscillator(float sr, size_t limit, Oscillator* osc): TapTempoOscillator(sr, limit, osc) {}
  static TapTempoAgnesiOscillator* create(float sample_rate, size_t limit, float block_rate){
    return new TapTempoAgnesiOscillator(sample_rate, limit, AgnesiOscillator::create(block_rate));
  }
  static void destroy(TapTempoAgnesiOscillator* obj){
    AgnesiOscillator::destroy((AgnesiOscillator*)obj->oscillator);
    delete obj;
  }
};

class WitchPatch : public Patch {
protected:
  SineOscillator* lfo;
  AdsrEnvelope* env;
public:
  WitchPatch(){
    lfo = SineOscillator::create(getBlockRate());
    env = AdsrEnvelope::create(getBlockRate());
  }
  virtual ~WitchPatch(){
    AdsrEnvelope::destroy(env);
    SineOscillator::destroy(lfo);
  }
  void dogate(bool state){
    env->gate(state);
  }
  void doenv(float shape){
    if(shape < 0.5){
      env->setAttack(shape*2);
      env->setRelease(0.01);
    }else{
      env->setAttack(0.01);
      env->setRelease(shape*2);
    }
    setParameterValue(PARAMETER_G, env->generate());
    setButton(BUTTON_F, env->getLevel() > 0.001);
  }
  void dolfo(float freq) {
    lfo->setFrequency(freq);
    setParameterValue(PARAMETER_F, lfo->generate()*0.5+0.5);
    setButton(BUTTON_E, lfo->getPhase() < M_PI);
  }
};
