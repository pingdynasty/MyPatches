
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

class StereoChorusProcessor : public MultiSignalProcessor {
protected:
  AudioBuffer* buffer;
  CircularFloatBuffer* bufferL;
  CircularFloatBuffer* bufferR;
  MultiBiquadFilter* filter;
  SineOscillator* lfo;
  float depth;
  SmoothFloat delay;
  SmoothFloat spread;
  SmoothFloat amount;
  static constexpr size_t taps = 4;
public:
  StereoChorusProcessor(AudioBuffer* buffer, CircularFloatBuffer* bufferL,
			CircularFloatBuffer* bufferR, SineOscillator* lfo):
    buffer(buffer), bufferL(bufferL), bufferR(bufferR), lfo(lfo),
    depth(0), delay(0), spread(0), amount(0) {
    if(bufferL)
      delay = 0.75*bufferL->getSize();
    filter = MultiBiquadFilter::create(48000, 2*taps, 4);
    filter->setLowPass(6000, FilterStage::BUTTERWORTH_Q);
  }
  void setDelay(float value){
    delay = value;    
  }
  void setDepth(float value){
    depth = value;
  }
  void setSpread(float value){
    spread = value;
  }
  void setAmount(float value){
    amount = value;
  }
  void setEffect(float value){
    amount = value*0.6;
    spread = 0.125*value*delay/taps;
    delay = value*bufferL->getSize() - spread*taps;
  }
  void setModulation(float value){
    depth = value;
  }
  // void process(FloatArray input, FloatArray output){
  // }
  void process(AudioBuffer& input, AudioBuffer& output){
    FloatArray inL = input.getSamples(LEFT_CHANNEL);
    FloatArray inR = input.getSamples(RIGHT_CHANNEL);
    output.multiply(1-amount); // scale dry signal
    FloatArray outL = output.getSamples(LEFT_CHANNEL);
    FloatArray outR = output.getSamples(RIGHT_CHANNEL);
    // outL.multiply(1-amount);
    // outR.multiply(1-amount);
    size_t len = input.getSize();
    bufferL->write(inL.getData(), len);
    bufferR->write(inR.getData(), len);
    // float ph = lfo->generate();
    float ph = spread*0.25;
    for(size_t i=0; i<taps; ++i){
      FloatArray bufL = buffer->getSamples(LEFT_CHANNEL+i);
      FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL+i);
      float index = delay + ph*depth + spread*i;
      bufferL->interpolatedDelay(bufL.getData(), bufL.getSize(), index);
      index = delay + ph*depth - spread*(i+1);
      bufferR->interpolatedDelay(bufR.getData(), bufR.getSize(), index);
    }
    filter->process(*buffer, *buffer);
    buffer->multiply(amount);
    for(size_t i=0; i<taps; ++i){
      FloatArray bufL = buffer->getSamples(LEFT_CHANNEL+i);
      FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL+i);
      outL.add(bufL);
      outR.add(bufR);      
    }
  }
  static StereoChorusProcessor* create(float sr, size_t bs, size_t delaysize){
    AudioBuffer* buffer = AudioBuffer::create(2*taps, bs);
    CircularFloatBuffer* bufferL = CircularFloatBuffer::create(delaysize);
    CircularFloatBuffer* bufferR = CircularFloatBuffer::create(delaysize);
    SineOscillator* lfo = SineOscillator::create(sr);
    return new StereoChorusProcessor(buffer, bufferL, bufferR, lfo);
  }
  static void destroy(StereoChorusProcessor* obj){
    AudioBuffer::destroy(obj->buffer);
    CircularFloatBuffer::destroy(obj->bufferL);
    CircularFloatBuffer::destroy(obj->bufferR);
    SineOscillator::destroy(obj->lfo);
    delete obj;
  }
};

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
  static float nonlinear(float x){ // Overdrive curve
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
  static OverdriveProcessor* create(){
    return new OverdriveProcessor();
  }
  static void destroy(OverdriveProcessor* obj){
    delete obj;
  }
};

class StereoOverdriveProcessor : public OverdriveProcessor {
protected:
  float previousOffset;
public:
  StereoOverdriveProcessor(): previousOffset(0) {}
  void process(AudioBuffer& input, AudioBuffer& output){
    float targetOffset = offset;
    size_t len = input.getSize();
    float incr = (targetOffset-previousOffset)/len;
    float* lin = input.getSamples(LEFT_CHANNEL);
    float* rin = input.getSamples(RIGHT_CHANNEL);
    float* lout = output.getSamples(LEFT_CHANNEL);
    float* rout = output.getSamples(RIGHT_CHANNEL);
    while(len--){
      *lout = OverdriveProcessor::process(*lin++);
      offset *= -1;
      *rout = OverdriveProcessor::process(*rin++);
      offset *= -1;
      offset += incr;
    }
    previousOffset = targetOffset;
  }
  static StereoOverdriveProcessor* create(){
    return new StereoOverdriveProcessor();
  }
  static void destroy(StereoOverdriveProcessor* obj){
    delete obj;
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
  void setModulation(float value){
    setDelay(value);
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
    return clamp(input, -1, 1);
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
    // phaser_left.setFeedback(max(0, value*1.25-0.4));
    phaser_right.setDepth(min(1, value*2));
    // phaser_right.setFeedback(max(0, value*1.25-0.4));
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
  static StereoPhaserProcessor* create(){
    return new StereoPhaserProcessor();
  }
  static void destroy(StereoPhaserProcessor* obj){
    delete obj;
  }
};
