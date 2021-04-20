
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
