
typedef WaveBank<NOF_X_WF, NOF_Y_WF, NOF_Z_WF, SAMPLE_LEN> MorphBank;
typedef WaveBankOscillator<NOF_X_WF, NOF_Y_WF, NOF_Z_WF, SAMPLE_LEN> MorphOsc;

class MorphSynth : public AbstractSynth {
protected:
  MorphOsc* osc;
  AdsrEnvelope* env;
  float gain;
  SmoothFloat x;
  SmoothFloat y;
  float xmod = 0;
  float ymod = 0;
public:
  enum Parameters {
		   PARAMETER_X,
		   PARAMETER_Y,
		   PARAMETER_ENV
  };
  MorphSynth(MorphOsc* osc, AdsrEnvelope* env): osc(osc), env(env) {}  
  void setFrequency(float freq){
    osc->setFrequency(freq);    
  }
  void setGain(float gain){
    this->gain = gain*GAINFACTOR;
    // debugMessage("gain/x/y", gain, x, y);
  }
  void trigger(){
    env->trigger();
  }
  void gate(bool state){
    env->gate(state);
    // debugMessage("gate1", osc->getFrequency(), osc->getPhase());
  }
  MorphOsc* getOscillator(){
    return osc;
  }
  AdsrEnvelope* getEnvelope(){
    return env;
  }
  void setEnvelope(float df){
    int di = (int)df;
    float attack, release;
    switch(di){
      // a/d
    case 0: // l/s
      attack = 1.0-df;
      release = 0.0;
      break;
    case 1: // s/s
      attack = 0.0;
      release = df-1;
      break;
    case 2: // s/l
      attack = df-2;
      release = 1.0;
      break;
    case 3: // l/l
      attack = 1.0;
      release = 1.0;
      break;
    }
    env->setAttack(attack);
    env->setRelease(release);
  }
  void setModulation(float modulation) override {
    // debugMessage("mod", mod, modulation);
    xmod = modulation*0.5;
  }
  void setPressure(float pressure) override {
    ymod = pressure*0.5;
  }
  virtual void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case PARAMETER_X:
      x = value+xmod;
      osc->setMorphX(x);
      break;
    case PARAMETER_Y:
      y = value+ymod;
      osc->setMorphY(y);
      break;
    case PARAMETER_ENV:
      setEnvelope(value*4);
      break;
    }
  }
};

class MorphMonoGenerator : public MorphSynth, public SignalGenerator {
public:
  MorphMonoGenerator(MorphOsc* osc, AdsrEnvelope* env): MorphSynth(osc, env) {}
  float generate(){
    return osc->generate()*env->generate()*gain;
  }
  using SignalGenerator::generate;
  static MorphMonoGenerator* create(MorphBank* bank, float sr){
    MorphOsc* osc = MorphOsc::create(bank, sr);
    AdsrEnvelope* env = AdsrEnvelope::create(sr);
    return new MorphMonoGenerator(osc, env);
  }
  static void destroy(MorphMonoGenerator* obj){
    MorphOsc::destroy(obj->osc);
    AdsrEnvelope::destroy(obj->env);
    delete obj;
  }
};

class MorphStereoGenerator : public MorphSynth, public MultiSignalGenerator {
private:
  MorphOsc* osc2;
  FloatArray buffer;
public:
  MorphStereoGenerator(MorphOsc* left, MorphOsc* right, AdsrEnvelope* env, FloatArray buffer)
    : MorphSynth(left, env), osc2(right), buffer(buffer) {}
  void generate(AudioBuffer& output){
    osc2->setFrequency(osc->getFrequency());
    osc2->setMorphX(osc->getMorphX());
    osc2->setMorphY(osc->getMorphY());
    // debugMessage("osc1", osc->getFrequency(), osc->getPhase());
    FloatArray left = output.getSamples(LEFT_CHANNEL);
    FloatArray right = output.getSamples(RIGHT_CHANNEL);
    env->generate(buffer);
    buffer.multiply(gain);
    osc->generate(left);
    left.multiply(buffer);
    osc2->generate(right);
    right.multiply(buffer);
  }
  static MorphStereoGenerator* create(MorphBank* bank1, MorphBank* bank2, float sr, size_t bs){
    FloatArray buffer = FloatArray::create(bs);
    MorphOsc* osc1 = MorphOsc::create(bank1, sr);
    MorphOsc* osc2 = MorphOsc::create(bank2, sr);
    AdsrEnvelope* env = AdsrEnvelope::create(sr);
    return new MorphStereoGenerator(osc1, osc2, env, buffer);
  }
  static void destroy(MorphStereoGenerator* obj){
    MorphOsc::destroy(obj->osc);
    MorphOsc::destroy(obj->osc2);
    FloatArray::destroy(obj->buffer);
    AdsrEnvelope::destroy(obj->env);
    delete obj;
  }
};
