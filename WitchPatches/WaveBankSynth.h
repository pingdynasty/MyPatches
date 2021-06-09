
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
  MorphSynth(MorphOsc* osc, AdsrEnvelope* env): osc(osc), env(env), gain(1), x(0), y(0) {}  
  void setFrequency(float freq){
    osc->setFrequency(freq);    
  }
  void setGain(float gain){
    this->gain = gain*GAINFACTOR;
  }
  void trigger(){
    env->trigger();
  }
  void gate(bool state){
    env->gate(state);
  }
  MorphOsc* getOscillator(){
    return osc;
  }
  AdsrEnvelope* getEnvelope(){
    return env;
  }
  void setEnvelope(float df){
    constexpr float tmin = 0.001;
    int di = (int)df;
    df = df - di;
    float attack, release;
    switch(di){
      // a/d
    case 0: // l/s
      attack = 1- df + tmin;
      release = tmin;
      break;
    case 1: // s/s
      attack = tmin;
      release = df + tmin;
      break;
    case 2: // s/l
      attack = df*df*2 + tmin;
      release = 1.0 + df*df; // allow extra-long decays
      break;
      // l/l
    }
    env->setAttack(attack);
    env->setRelease(release);
  }
  void setModulation(float modulation) override {
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
      setEnvelope(value*3);
      break;
    }
  }
};

class MorphMonoGenerator : public MorphSynth, public SignalGenerator, public SignalProcessor {
public:
  MorphMonoGenerator(MorphOsc* osc, AdsrEnvelope* env): MorphSynth(osc, env) {}
  float generate(){
    return osc->generate()*env->generate()*gain;
  }
  using SignalGenerator::generate;
  float process(float input){
    return osc->generate(input)*env->generate()*gain;
  }
  using MidiProcessor::process;
  using SignalProcessor::process;
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

class MorphStereoGenerator : public MorphSynth, public MultiSignalGenerator, public MultiSignalProcessor  {
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
    FloatArray left = output.getSamples(LEFT_CHANNEL);
    FloatArray right = output.getSamples(RIGHT_CHANNEL);
    env->generate(buffer);
    buffer.multiply(gain);
    osc->generate(left);
    left.multiply(buffer);
    osc2->generate(right);
    right.multiply(buffer);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    osc2->setFrequency(osc->getFrequency());
    osc2->setMorphX(osc->getMorphX());
    osc2->setMorphY(osc->getMorphY());    
    FloatArray lin = input.getSamples(LEFT_CHANNEL);
    FloatArray rin = input.getSamples(RIGHT_CHANNEL);
    FloatArray lout = output.getSamples(LEFT_CHANNEL);
    FloatArray rout = output.getSamples(RIGHT_CHANNEL);
    env->generate(buffer);
    buffer.multiply(gain);
    osc->generate(lout, lin);
    lout.multiply(buffer);
    osc2->generate(rout, rin);
    rout.multiply(buffer);
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
