
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

class WaveMultiplierProcessor : public SignalProcessor, public MultiSignalProcessor {
protected:
  static constexpr size_t WAVETABLE_SIZE = 1024;
  float wavetable[WAVETABLE_SIZE];
  float multiplier;
public:
  WaveMultiplierProcessor() {
    for(int i=0; i<WAVETABLE_SIZE; ++i)
      // wavetable[i] = 2.0f*i/WAVETABLE_SIZE - 1.0f; // tri
      wavetable[i] = sinf(2.0f*M_PI*i/WAVETABLE_SIZE); // sine
  }
  void setEffect(float value){
    multiplier = 0.25 + value*2.75;
  }
  void setMultiplier(float value){
    multiplier = value;
  }
  using SignalProcessor::process;
  float process(float input){
    size_t index = (input+1)*multiplier*WAVETABLE_SIZE;
    return wavetable[index & (WAVETABLE_SIZE-1)];
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    for(size_t ch=0; ch<input.getChannels(); ++ch)
      SignalProcessor::process(input.getSamples(ch), output.getSamples(ch));
  }
};

class OverdriveProcessor : public SignalProcessor, public MultiSignalProcessor {
protected:
  float offset = 0;
  float drive = 1;
public:
  void setOffset(float value){
    offset = value/10;
  }
  void setDrive(float value){
    drive = value*40+1;
  }
  void setEffect(float value){
    setDrive(value*0.8);
  }
  void setModulation(float value){
    offset = 0.5*value*(drive-1)/40;
  }
  float nonlinear(float x){ // Overdrive curve
    return x * ( 27 + x*x ) / ( 27 + 9*x*x );
  }
  float process(float input){
    return clamp(nonlinear(input*drive+offset), -1, 1);
  }
  using SignalProcessor::process;
  void process(AudioBuffer& input, AudioBuffer& output){
    for(size_t ch=0; ch<input.getChannels(); ++ch){
      SignalProcessor::process(input.getSamples(ch), output.getSamples(ch));
      offset *= -1;
    }
  }
};

class OffsetScaler : public SignalProcessor {
protected:
  float offset;
  float scalar;
public:
  float process(float input){
    return input*scalar + offset;
  }
  void process(FloatArray input, FloatArray output){
    input.add(offset, output);
    output.multiply(scalar);
  }
};

class CvNoteProcessor {
protected:
  MidiProcessor* processor;
  size_t cv_delay;
  size_t cv_ticks;
  bool cv_triggered = false;
  bool cv_ison = false;
  uint8_t cv_noteon = NO_NOTE;
  static constexpr uint8_t NO_NOTE = 0xff;
  static constexpr uint8_t CV_VELOCITY = 100;
public:
  // enum State {
  // 	      STATE_TRIGGERED,
  // 	      STATE_PLAYING,
  // };
  CvNoteProcessor(MidiProcessor* processor, size_t delay)
    : processor(processor), cv_delay(delay) {}
  virtual uint8_t getNoteForCv(float cv){
    return cv*12*5+42; // todo
  }
  void gate(bool ison, size_t delay){
    if(ison != cv_ison && !cv_triggered){ // prevent re-triggers during delay
      cv_ison = ison;
      cv_triggered = true;
      cv_ticks = delay;
      if(ison)
	cv_noteon = NO_NOTE;
    }
  }
  void cv(float value){
    if(cv_triggered && cv_ticks >= cv_delay){
      cv_triggered = false;
      if(cv_noteon == NO_NOTE){
	cv_noteon = getNoteForCv(value);
	processor->noteOn(MidiMessage::note(0, cv_noteon, CV_VELOCITY));
      }else{
	processor->noteOff(MidiMessage::note(0, cv_noteon, 0));
	cv_noteon = NO_NOTE;
      }
    }
  }
  void clock(size_t ticks){
    if(cv_ticks < cv_delay)
      cv_ticks += ticks;
  }
  static CvNoteProcessor* create(float sr, float delay_milliseconds, MidiProcessor* processor){
    return new CvNoteProcessor(processor, sr*delay_milliseconds/1000);
  }
  static void destroy(CvNoteProcessor* obj){
    delete obj;
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

class StereoPhaserProcessor : public MultiSignalProcessor {
protected:
  PhaserSignalProcessor phaser_left;
  PhaserSignalProcessor phaser_right;
public:
  void setModulation(float value){
    setDelay(value);
  }
  void setEffect(float value){
    phaser_left.setDepth(min(1, value*2));
    phaser_left.setFeedback(max(0, value*1.25-0.4));
    phaser_right.setDepth(min(1, value*2));
    phaser_right.setFeedback(max(0, value*1.25-0.4));
  }
  void setDelay(float value){
      // phaser.setDelay(value*0.04833+0.01833); // range: 440/(sr/2) to 1600/(sr/2)
    phaser_left.setDelay(value*0.04+0.02);
    phaser_right.setDelay(value*0.05+0.018);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    phaser_left.process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    phaser_right.process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }
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
