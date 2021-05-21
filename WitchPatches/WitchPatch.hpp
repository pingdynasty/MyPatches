#if VOICES == 1
#define GAINFACTOR 1
#elif VOICES == 2
#define GAINFACTOR 0.70
#elif VOICES <= 4
#define GAINFACTOR 0.5
#elif VOICES <= 6
#define GAINFACTOR 0.4
#elif VOICES <= 8
#define GAINFACTOR 0.35
#else
#define GAINFACTOR 0.25
#endif

float constexpr sqrtfNewtonRaphson(float x, float curr, float prev){
  return curr == prev ? curr : sqrtfNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}
/**
 * Constexpr aproximation of the square root
 * Only valid for finite and non-negative values of 'x'
 */
float constexpr constsqrtf(float x){
  return sqrtfNewtonRaphson(x, x, 0);
}

class AbstractStatelessProcessor : public SignalProcessor, public MultiSignalProcessor {
public:
  using SignalProcessor::process;
  void process(AudioBuffer& input, AudioBuffer& output){
    for(size_t ch=0; ch<input.getChannels(); ++ch)
      SignalProcessor::process(input.getSamples(ch), output.getSamples(ch));
  }
};

class FuzzProcessor : public AbstractStatelessProcessor {
protected:
  float drive = 0; // -3 to 100
public:
  float process(float x){
    // //fuzz(x)      = x-0.15*x^2-0.15*x^3;
    // return x - 0.15*x*x - 0.15*x*x*x;
    // //fuzz(x)      = 1.5*x-0.5*x^3;
    // return 1.5*x - 0.5*x*x*x;
    // fuzz(x)      = (1+drive/101)*x/(1+drive/101*abs(x));
    return (1+drive/101)*x/(1+drive/101*abs(x));
    // https://www.desmos.com/calculator/7qom9leesg
  }
  using AbstractStatelessProcessor::process;
};

class FizzProcessor : public AbstractStatelessProcessor {
protected:
public:
  float process(float x){
    // fiz(x)       = x+(x^7);
    return x+(x*x*x*x*x*x*x);
  }
  using AbstractStatelessProcessor::process;
};

class SoftClipProcessor : public AbstractStatelessProcessor {
  // ref: https://www.dsprelated.com/freebooks/pasp/Soft_Clipping.html
  /**
   * Third-order static soft-clipping function used in Yamaha multi-effects
   * ref:  T. Araya and A. Suyama, “Sound effector capable of
   * imparting plural sound effects like distortion and other
   * effects,” US Patent 5,570,424, 29 Oct. 1996.
   */
public:
  float process(float x){
    return clamp((3*x/2)*(1-x*x/3), -1, 1); // cubic nonlinearity
    // return clamp(3*(x-x*x*x/3)/2, -1, 1); // cubic nonlinearity
  }
  using AbstractStatelessProcessor::process;
};

class ToneProcessor : public SignalProcessor {
protected:
  float B[3];
  float A[3];
  float tone; // tone control, must be between 0 and 1. Defaults to 0.5.
  float fs;   // sampling frequency
  float x[4]; // current and previous input samples
  float y[4]; // current and previous output samples
public:
  ToneProcessor(float sr): fs(sr) {
    memset(B, 0, sizeof(B));
    memset(A, 0, sizeof(A));
    memset(x, 0, sizeof(x));
    memset(y, 0, sizeof(y));
  }
  void setEffect(float value){
    setTone(value);
  }
  void setModulation(float value){
    // todo?
  }
  // void process(FloatArray input, FloatArray output){
  using SignalProcessor::process;
  float process(float input){
    static int p = 0;
    x[p] = input;
    y[p] = input*B[0] + x[(p+1)&3]*B[1]+x[(p+2)&3]*B[2] - (A[1]*y[(p+1)&3] + A[2]*y[(p+2)&3]);
    p = (p-1+4)&3;
    return y[p];
  } 
  void setTone(float newTone){ //float tone must be a float between 0 and 1 (no check is performed)
    tone = newTone;
    updateCoefficients();
  }
  void updateCoefficients(){
    /* Computes the filter coefficients for a bigMuff-style tone circuit, adpated from Matlab code by Guillaume Le Nost */
    // Values from schematics
    const float pot = 100e3; // tone pot
    const float cb = 3.3e-9; // C9  +/- 5%
    const float rb = 27e3 ; // R5 // better than 27e3 (nominal value)
    const float r1 = pot * (1-tone);
    const float rt = 27e3; // R8
    const float ct = 10e-9; // C8  better than 10e-9. Better than 0.9*10e-9 for Ton=0.5
    float r2 = pot * (tone) ;

    // Equation from circuit analysis (H(s))
    float b2 = ct*cb*r2*rt;
    float b1 = cb*(r1+r2+rt);
    float b0 = 1+ r1/rb;
    float a2 = ct*rt*cb*(r1+r2);
    float a1 = ct*rt*(1+(r1+r2)/rb) + cb*(r1+r2+rt);
    float a0 = 1+(r1+r2+rt)/rb;

    // Bilinear Transform
    float c = 2*fs; // c=2/T
    float cc = c*c; // cc=c^2;
    float value = 0.5-tone;
    float gain = 3*(value>=0 ? value: -value ); //gain=3*(abs(0.5-tone));
    gain = exp10f(gain/20); // from dB to linear
    float A0=a0 + a1*c + a2*cc;
    A[0]= 1;
    A[1] = (2*a0 - 2*a2*cc)/A0;
    A[2] = (a0 - a1*c + a2*cc)/A0;
  
    float gainOverA0=gain/A0;
    B[0] = (b0 + b1*c + b2*cc)*gainOverA0;
    B[1]= (2*b0 - 2*b2*cc)*gainOverA0;
    B[2] = (b0 - b1*c + b2*cc)*gainOverA0;
  }
};

class SeriesSignalProcessor : public SignalProcessor {
protected:
  SignalProcessor* a;
  SignalProcessor* b;
public:
  float process(float input){
    return b->process(a->process(input));
  }
  void process(FloatArray input, FloatArray output){
    a->process(input, output);
    b->process(output, output);
  }
};

template<class Processor>
class StereoSignalProcessor : public Processor {
protected:
  Processor right;
public:
  template <typename... Args>
  StereoSignalProcessor(Args&&... args) :
    Processor(std::forward<Args>(args)...), right(std::forward<Args>(args)...) {}
  void process(AudioBuffer& input, AudioBuffer& output){
    Processor::process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    right.process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }
};

// class StereoSignalProcessor : public MultiSignalProcessor {
// protected:
//   SignalProcessor* left;
//   SignalProcessor* right;
// public:
//   StereoSignalProcessor(SignalProcessor* left, SignalProcessor* right): left(left), right(right) {}
//   void process(AudioBuffer& input, AudioBuffer& output){
//     left->process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
//     right->process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
//   }
// };

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
    multiplier = value*1.5 + 0.5;
    // multiplier = 0.25 + value*2.75;
  }
  void setModulation(float value){
    // todo?
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
  void setEffect(float value){
    drive = value*8+1;
  }
  void setModulation(float value){
    offset = value*(drive-1)*0.04;
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
  size_t cv_ticks = 0;
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
    return cv*12*5 + 30; // todo
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
  float a1;
  FloatArray zm1;
  float feedback = 0;
  float depth = 0;
  float z = 0;
public:
  PhaserSignalProcessor(){
    zm1 = FloatArray::create(6);
  }
  ~PhaserSignalProcessor(){
    FloatArray::destroy(zm1);
  }
  void setDelay(float delay){ //sample delay time
    a1 = (1.f - delay) / (1.f + delay);
  }
  void setFeedback(float f){
    feedback = f;
  }
  void setDepth(float d){
    depth = d;
  }
  float process(float input){
    float x = input + z*feedback;
    for(size_t i=0; i<zm1.getSize(); ++i){
      float y = x * -a1 + zm1[i]; // each zm1 is an all pass stage
      zm1[i] = y * a1 + x;
      x = y; // the output of one stage becomes the input of the next one
    }
    z = x; // save for feedback
    input += x * depth;
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
  void reset(){
    oscillator->reset();
  }
  float generate(){
    oscillator->setFrequency(getFrequency());
    return oscillator->generate();
  }
  float getPhase(){
    return oscillator->getPhase();
  }
  void setPhase(float phase){
    oscillator->setPhase(phase);
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
