class SeriesSignalProcessor : public SignalProcessor {
protected:
  SignalProcessor* a;
  SignalProcessor* b;
public:
  void process(FloatArray input, FloatArray output){
    a->process(input, output);
    b->process(output, output);
  }
};

class StereoSignalProcessor : public MultiSignalProcessor {
protected:
  SignalProcessor* left;
  SignalProcessor* right;
public:
  StereoSignalProcessor(SignalProcessor* left, SignalProcessor* right): left(left), right(right) {}
  void process(AudioBuffer& input, AudioBuffer& output){
    left->process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    right->process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }
};

class PhaserSignalProcessor : public SignalProcessor {
protected:
  class AllpassDelay {
  private:
    float _a1;
    float _zm1;        
  public:
    AllpassDelay() : _a1( 0.f ), _zm1( 0.f ){}        
    void delay(float delay){ //sample delay time
      _a1 = (1.f - delay) / (1.f + delay);
    }
    float update(float x){
      float y = x * -_a1 + _zm1;
      _zm1 = y * _a1 + x;
      return y;
    }
  };
  AllpassDelay alps[6];
  float feedback = 0;
  float depth = 0;
  float zm1 = 0;
public:
  void setFeedback(float f){
    feedback = f;
  }
  void setDepth(float d){
    depth = d;
  }
  void setDelay(float d){
    //update filter coeffs
    for(size_t i=0; i<6; i++)
      alps[i].delay(d);
  }
  float process(float input){
    float y = alps[0].update(alps[1].update(alps[2].update(alps[3].update(alps[4].update(alps[5].update(input + zm1 * feedback ))))));											      
    zm1 = y;
    input += y * depth;
    return input;
  }
  using SignalProcessor::process;
};

// todo: reset oscillator phase on trigger
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
