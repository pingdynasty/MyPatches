#include "common.h"
#include "WitchLFO.h"

#define TRIGGER_LIMIT (1<<22)

class StereoMixProcessor : public MultiSignalProcessor {
protected:
  float a = 2.424; // default pan law -3dB
  float amount = 0; // -1 to 1
public:
  /**
   * Set pan law (typically between -3 and -6 dB)
   */
  void setPanLaw(float db){
    float y = exp10f(db/20);
    a = - y/(y-1);
  }
  /** 
   * Set the stereo mix to a value between -1 and 1.
   * At 0 there is full separation, at 1 there is full mix.
   * Positive values add an amount of the opposite channel, 
   * while negative values subtract.
   */
  void setMixAmount(float value){
    amount = value < 0 ? -value*value : value*value;
  }
  void mix(FloatArray left, FloatArray right, FloatArray out){
    right.multiply(amount, out);
    out.add(left);
    out.multiply(a/(a+abs(amount)));
    // curve is a/(a+x*x) + a*x*x/(a+x*x)
    // https://www.desmos.com/calculator/62xrspfgff
    // with a=1, sum is unity for any amount, at full mix each side is 0.5, pan law -6dB
    // with a=2.424, sum is unity at 0 amount, at full mix each side is 0.707, pan law -3dB
    // with amplitude at edges 1.4125, 3dB
  }
    
  // note: in-place processing won't work
  void process(AudioBuffer& input, AudioBuffer& output){
    FloatArray lin = input.getSamples(LEFT_CHANNEL);
    FloatArray rin = input.getSamples(RIGHT_CHANNEL);
    FloatArray lout = output.getSamples(LEFT_CHANNEL);
    FloatArray rout = output.getSamples(RIGHT_CHANNEL);
    mix(lin, rin, lout);
    mix(rin, lin, rout);
  }
};


class WitchEnvelope : public AdsrEnvelope {
protected:
  static constexpr float tmin = 0.002;
  static constexpr float tmax = 2;
  Control<PARAMETER_ATTACK> ctrlattack;
  Control<PARAMETER_DECAY> ctrldecay;
  Control<PARAMETER_SUSTAIN> ctrlsustain;
  Control<PARAMETER_RELEASE> ctrlrelease;
public:
  WitchEnvelope(float sr): AdsrEnvelope(sr){
    ctrlsustain = 1;
  }

  /**
   * Adjust the attack and release
   * @param shape: value between 0 and 1
   */
  void adjust(float shape){
    // shape *= 3;
    // int di = (int)shape;
    // float df = shape - di;
    int di;
    float df;
    if(shape < 0.3){
      di = 0;
      df = shape/0.3;
    }else if(shape >= 0.7){
      di = 2;
      df = (shape - 0.7)/0.3;
    }else{
      di = 1;
      df = (shape - 0.3)/(0.7 - 0.3);
    }
    float attack, release;
    switch(di){
      // a/d
    case 0:
      // l/s
      df = 1 - df;
      attack = df*df*tmax + tmin;
      release = tmin;
      break;
    case 1:
      // s/s
      attack = tmin;
      release = df*df*tmax + tmin;
      break;
    case 2:
      // s/l
      attack = df*df*tmax + tmin;
      release = tmax + df*df; // allow extra-long decays
      break;
      // l/l
    }
    setAttack(attack + ctrlattack*tmax);
    setDecay(ctrldecay*tmax + tmin);
    setSustain(ctrlsustain*ctrlsustain);
    setRelease(release + ctrlrelease*tmax);
  }

  static WitchEnvelope* create(float sr){
    return new WitchEnvelope(sr);
  }
};

class WitchFX : public TapTempo, public MultiSignalProcessor {
protected:
  static constexpr int TAP_LIMIT = (1<<21);
public:
  WitchFX(float samplerate) : TapTempo(samplerate, TAP_LIMIT) {}
  virtual ~WitchFX(){}
  // virtual void trigger(bool on, int delay) = 0;
  // virtual void clock(size_t steps) = 0;
  virtual void setModulation(float value) = 0;
  virtual void setEffect(float value) = 0;
  virtual void process(AudioBuffer& input, AudioBuffer& output) = 0;
  // calls TapTempo::clock()
  // virtual static WitchFX* create(float samplerate, size_t blocksize) = 0;
  // virtual static void destroy(WitchFX* obj) = 0;
  virtual void reset(){}
};

class SmoothStereoDelayProcessor : public MultiSignalProcessor {
// typedef FractionalDelayProcessor<HERMITE_INTERPOLATION> SmoothDelayProcessor;
  typedef CrossFadingDelayProcessor SmoothDelayProcessor;
  typedef DryWetMultiSignalProcessor<PingPongFeedbackProcessor> MixProcessor;
// typedef DryWetMultiSignalProcessor<StereoFeedbackProcessor> MixProcessor;
protected:
  SmoothDelayProcessor* left_delay;
  SmoothDelayProcessor* right_delay;
  MixProcessor* delaymix;
  // static constexpr int TRIGGER_LIMIT = (1<<17);
public:
  SmoothStereoDelayProcessor(SmoothDelayProcessor* left_delay, SmoothDelayProcessor* right_delay, MixProcessor* mix) :
    left_delay(left_delay), right_delay(right_delay), delaymix(mix) {}
  ~SmoothStereoDelayProcessor(){
  }
  void setDelay(float delay_samples){
    left_delay->setDelay(delay_samples);
    right_delay->setDelay(delay_samples);
  }
  void setMix(float mix){
    delaymix->setMix(mix);
  }
  void setFeedback(float feedback){
    delaymix->setFeedback(feedback);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    delaymix->process(input, output);
  }
  void reset(){
    left_delay->clear();
    right_delay->clear();
  }
  static SmoothStereoDelayProcessor* create(size_t blocksize, size_t delaysize){
    SmoothDelayProcessor* left_delay = SmoothDelayProcessor::create(delaysize, blocksize);
    SmoothDelayProcessor* right_delay = SmoothDelayProcessor::create(delaysize*2, blocksize);
    MixProcessor* delaymix = MixProcessor::create(2, blocksize,
						  left_delay, right_delay,
						  FloatArray::create(blocksize),
						  FloatArray::create(blocksize));
    return new SmoothStereoDelayProcessor(left_delay, right_delay, delaymix);
  }
  static void destroy(SmoothStereoDelayProcessor* obj){
    SmoothDelayProcessor::destroy(obj->left_delay);
    SmoothDelayProcessor::destroy(obj->right_delay);
    MixProcessor::destroy(obj->delaymix);
    delete obj;
  }
};

class AbstractStatelessProcessor : public SignalProcessor, public MultiSignalProcessor {
public:
  using SignalProcessor::process;
  void process(AudioBuffer& input, AudioBuffer& output){
    for(int ch=0; ch<input.getChannels(); ++ch)
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
    return clamp((3*x/2)*(1-x*x/3), -1.0f, 1.0f); // cubic nonlinearity
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
    for(size_t i=0; i<WAVETABLE_SIZE; ++i)
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
    for(int ch=0; ch<input.getChannels(); ++ch)
      SignalProcessor::process(input.getSamples(ch), output.getSamples(ch));
  }
};

class OverdriveProcessor : public SignalProcessor, public MultiSignalProcessor {
protected:
  float offset = 0;
  float drive = 1;
public:
  void setOffset(float value){
    offset = value;
  }
  void setDrive(float value){
    drive = value;
  }
  static float nonlinear(float x){ // Overdrive curve
    return x * ( 27 + x*x ) / ( 27 + 9*x*x );
  }
  float process(float input){
    return clamp(nonlinear(input*drive+offset), -1.0f, 1.0f);
  }
  using SignalProcessor::process;
  void process(AudioBuffer& input, AudioBuffer& output){
    for(int ch=0; ch<input.getChannels(); ++ch){
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
  float previousOffset = 0;
public:
  StereoOverdriveProcessor(){}
  float process(float input, float offset){
    return clamp(nonlinear(input*drive+offset), -1.0f, 1.0f);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    float targetOffset = this->offset;
    float currentOffset = previousOffset;
    size_t len = input.getSize();
    float incr = (targetOffset - currentOffset)/len;
    float* lin = input.getSamples(LEFT_CHANNEL);
    float* rin = input.getSamples(RIGHT_CHANNEL);
    float* lout = output.getSamples(LEFT_CHANNEL);
    float* rout = output.getSamples(RIGHT_CHANNEL);
    while(len--){
      *lout++ = process(*lin++, currentOffset);
      *rout++ = process(*rin++, -currentOffset);
      currentOffset += incr;
    }
    previousOffset = currentOffset;
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
    return clamp(input, -1.0f, 1.0f);
  }
  using SignalProcessor::process;
};

class StereoPhaserProcessor : public MultiSignalProcessor {
protected:
  PhaserSignalProcessor phaser_left;
  PhaserSignalProcessor phaser_right;
public:
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

class WitchDelay : public WitchFX {
private:
  SmoothStereoDelayProcessor* delay;
public:
  WitchDelay(float sr, SmoothStereoDelayProcessor* delay) : WitchFX(sr), delay(delay) {}
  void setModulation(float value){
  }
  void setEffect(float value){
    delay->setFeedback(max(0.0f, value*0.75f-0.5f));
    delay->setMix(min(0.5f, value));
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    TapTempo::clock(input.getSize());
    delay->setDelay(TapTempo::getPeriodInSamples());
    delay->process(input, output);
    output.getSamples(LEFT_CHANNEL).softclip();
    output.getSamples(RIGHT_CHANNEL).softclip();
  }
  void reset(){
    delay->reset();
  }
  static WitchDelay* create(float samplerate, size_t blocksize){
    SmoothStereoDelayProcessor* delay = SmoothStereoDelayProcessor::create(blocksize, 48000*4);
    return new WitchDelay(samplerate, delay);
  }
  static void destroy(WitchDelay* obj){
    SmoothStereoDelayProcessor::destroy(obj->delay);
    delete obj;
  }
};

class WitchPhaser : public WitchFX, public StereoPhaserProcessor {
public:
  WitchPhaser(float sr) : WitchFX(sr) {}
  void setModulation(float value){
    setDelay(value);
  }
  void setEffect(float value){
    phaser_left.setDepth(min(1.0f, value*2));
    // phaser_left.setFeedback(max(0.0f, value*1.25-0.4));
    phaser_right.setDepth(min(1.0f, value*2));
    // phaser_right.setFeedback(max(0.0f, value*1.25-0.4));
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    StereoPhaserProcessor::process(input, output);
    output.getSamples(LEFT_CHANNEL).softclip();
    output.getSamples(RIGHT_CHANNEL).softclip();
  }
  static WitchPhaser* create(float samplerate, size_t blocksize){
    return new WitchPhaser(samplerate);
  }
  static void destroy(WitchPhaser* obj){
    delete obj;
  }
};

class WitchOverdrive : public WitchFX, public StereoOverdriveProcessor {
protected:
  float gain = 1;
public:
  WitchOverdrive(float samplerate): WitchFX(samplerate){}
  void setEffect(float value){
    setDrive(value*value*8+value*4+1);
    gain = max(1-value, 0.5f);
  }
  void setModulation(float value){
    setOffset(value*(drive-1)*0.1);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    StereoOverdriveProcessor::process(input, output);
    // OverdriveProcessor::process(input, output);
    output.multiply(gain);
  }
  static WitchOverdrive* create(float samplerate, size_t blocksize){
    return new WitchOverdrive(samplerate);
  }
  static void destroy(WitchOverdrive* obj){
    delete obj;
  }
};

class StereoChorusProcessor : public MultiSignalProcessor {
  // typedef InterpolatingCircularFloatBuffer<HERMITE_INTERPOLATION> CircularBufferType;
  typedef CrossFadingCircularBuffer<float> CircularBufferType;
protected:
  AudioBuffer* buffer;
  CircularBufferType* bufferL;
  CircularBufferType* bufferR;
  MultiBiquadFilter* filter;
  float depth;
  SmoothFloat delay;
  SmoothFloat spread;
  SmoothFloat amount;
  static constexpr size_t taps = 3;
  size_t delay_times[taps*2];
public:
  StereoChorusProcessor(AudioBuffer* buffer, CircularBufferType* bufferL,
			CircularBufferType* bufferR, MultiBiquadFilter* filter) :			
    buffer(buffer), bufferL(bufferL), bufferR(bufferR), filter(filter),
    depth(0), delay(0), spread(0), amount(0) {
    if(bufferL)
      delay = 0.5*bufferL->getSize();
    for(size_t i=0; i<taps*2; ++i)
      delay_times[i] = delay;
  }
  /**
   * @param value should be from 0 to 1
   */
  void setDelay(float value){
    delay = value*bufferL->getSize() - buffer->getSize();
    // subtract one buffer length because we write before we read
  }
  void setDepth(float value){
    depth = value;
  }
  void setSpread(float value){
    spread = value*bufferL->getSize()/(taps*2);
  }
  void setAmount(float value){
    amount = value;
  }
  // void process(FloatArray input, FloatArray output){
  // }
  void process(AudioBuffer& input, AudioBuffer& output){
    FloatArray bufL = buffer->getSamples(LEFT_CHANNEL);
    FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL);
    size_t len = input.getSize();
    filter->process(input, *buffer);
    bufferL->write(bufL.getData(), len);
    bufferR->write(bufR.getData(), len);
    float ph = spread*0.25;
    for(size_t i=0; i<taps; ++i){
      FloatArray bufL = buffer->getSamples(LEFT_CHANNEL+i*2);
      FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL+i*2);
      float index = delay + ph*depth + spread*i*1.7;
      bufferL->delay(bufL.getData(), bufL.getSize(), delay_times[i*2], index);
      delay_times[i*2] = index;
      index = delay + ph*depth - spread*i*2.3;
      bufferR->delay(bufR.getData(), bufR.getSize(), delay_times[i*2+1], index);
      delay_times[i*2+1] = index;
    }
    // filter->process(*buffer, *buffer);
    buffer->multiply(amount/taps);
    output.copyFrom(input);
    output.multiply(1-amount); // scale dry signal
    FloatArray outL = output.getSamples(LEFT_CHANNEL);
    FloatArray outR = output.getSamples(RIGHT_CHANNEL);
    for(size_t i=0; i<taps; ++i){
      FloatArray bufL = buffer->getSamples(LEFT_CHANNEL+i*2);
      FloatArray bufR = buffer->getSamples(RIGHT_CHANNEL+i*2);
      outL.add(bufL);
      outR.add(bufR);
    }
  }
  static StereoChorusProcessor* create(float sr, size_t bs, size_t delaysize){
    AudioBuffer* buffer = AudioBuffer::create(2*taps, bs);
    CircularBufferType* bufferL = CircularBufferType::create(delaysize, bs);
    CircularBufferType* bufferR = CircularBufferType::create(delaysize, bs);
    MultiBiquadFilter* filter = MultiBiquadFilter::create(48000, 2*taps, 4);
    filter->setLowPass(6000, FilterStage::BUTTERWORTH_Q);
    return new StereoChorusProcessor(buffer, bufferL, bufferR, filter);
  }
  static void destroy(StereoChorusProcessor* obj){
    AudioBuffer::destroy(obj->buffer);
    CircularBufferType::destroy(obj->bufferL);
    CircularBufferType::destroy(obj->bufferR);
    MultiBiquadFilter::destroy(obj->filter);
    delete obj;
  }
};

class WitchChorus : public WitchFX  {
protected:
  StereoChorusProcessor* processor;
public:
  WitchChorus(float samplerate, StereoChorusProcessor* processor):
    WitchFX(samplerate), processor(processor) {
    processor->setDelay(0.5);
    processor->setSpread(0.2);
  }
  void setEffect(float value){
    processor->setAmount(value*0.6);
    processor->setSpread(0.4*value);
    // processor->setDelay(value*0.8);
  }
  void setModulation(float value){
    processor->setDepth(value);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    processor->process(input, output);
    output.getSamples(LEFT_CHANNEL).softclip();
    output.getSamples(RIGHT_CHANNEL).softclip();
  }
  static WitchChorus* create(float samplerate, size_t blocksize){
    return new WitchChorus(samplerate, StereoChorusProcessor::create(samplerate, blocksize, 0.20*samplerate));
  }
  static void destroy(WitchChorus* obj){
    StereoChorusProcessor::destroy(obj->processor);
    delete obj;
  }
};

class WitchMultiEffect : public MultiSignalProcessor {
public:
  enum Effect { PHASER, DELAY, OVERDRIVE, CHORUS, NOF_EFFECTS };
private:
  WitchFX* fx[NOF_EFFECTS];
  size_t selected = 0;
public:
  WitchMultiEffect() {}
  static WitchMultiEffect* create(float samplerate, size_t blocksize){
    WitchMultiEffect* obj = new WitchMultiEffect();
    obj->fx[0] = WitchPhaser::create(samplerate, blocksize);
    obj->fx[1] = WitchDelay::create(samplerate, blocksize);
    obj->fx[2] = WitchOverdrive::create(samplerate, blocksize);
    obj->fx[3] = WitchChorus::create(samplerate, blocksize);
    return obj;
  }
  static void destroy(WitchMultiEffect* obj){
    WitchPhaser::destroy((WitchPhaser*)obj->fx[0]);
    WitchDelay::destroy((WitchDelay*)obj->fx[1]);
    WitchOverdrive::destroy((WitchOverdrive*)obj->fx[2]);
    WitchChorus::destroy((WitchChorus*)obj->fx[3]);
    delete obj;
  }
  void setBeatsPerMinute(float bpm){
    fx[selected]->setBeatsPerMinute(bpm);
  }
  void trigger(bool on, int delay){
    fx[selected]->trigger(on, delay);
  }
  float getParameterValueForEffect(Effect value){
    return float(value+0.5)/NOF_EFFECTS;
  }
  void select(float value){
    size_t next = clamp(int(value*NOF_EFFECTS), 0, NOF_EFFECTS-1);
    if(selected != next){
      // debugMessage("FX", (int)next);
      fx[next]->reset();
      fx[next]->setPeriodInSamples(fx[selected]->getPeriodInSamples());
      selected = next;
    }
  }
  void setFrequency(float freq){
    fx[selected]->setFrequency(freq);
  }
  void setModulation(float value){
    fx[selected]->setModulation(value);
  }
  void setEffect(float value){
    fx[selected]->setEffect(value);
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    fx[selected]->process(input, output);
  }    
};


class CvNoteProcessor {
protected:
  MidiProcessor* processor;
  size_t cv_delay;
  float note_range;
  float note_offset;
  uint8_t note_channel = 1;
  size_t cv_ticks = 0;
  bool cv_triggered = false;
  bool cv_ison = false;
  uint8_t cv_noteon = NO_NOTE;
  static constexpr uint8_t NO_NOTE = 0xff;
  static constexpr uint8_t CV_VELOCITY = 100;
public:
  CvNoteProcessor(MidiProcessor* processor, size_t delay, float range, float offset)
    : processor(processor), cv_delay(delay), note_range(range), note_offset(offset) {}
  uint8_t getNoteForCv(float cv){
    return round(cv*note_range + note_offset);
  }
  uint8_t getNote(){
    if(cv_noteon == NO_NOTE)
      return 0;
    return cv_noteon;
  }
  void gate(bool ison, size_t delay){
    if(ison != cv_ison && !cv_triggered){ // prevent re-triggers during delay
      cv_ison = ison;
      cv_triggered = true;
      cv_ticks = -delay;
      if(ison)
	cv_noteon = NO_NOTE;
    }
  }
  void cv(float value){
    if(cv_triggered && cv_ticks >= cv_delay){
      cv_triggered = false;
      if(cv_noteon == NO_NOTE){
	cv_noteon = getNoteForCv(value);
	processor->noteOn(MidiMessage::note(note_channel, cv_noteon, CV_VELOCITY));
      }else{
	processor->noteOff(MidiMessage::note(note_channel, cv_noteon, 0));
	cv_noteon = NO_NOTE;
      }
    }
  }
  void clock(size_t ticks){
    if(cv_ticks < cv_delay)
      cv_ticks += ticks;
  }
  static CvNoteProcessor* create(float sr, float delay_milliseconds, MidiProcessor* processor,
				 float range=12*5, float offset=30){
    return new CvNoteProcessor(processor, sr*delay_milliseconds/1000, range, offset);
  }
  static void destroy(CvNoteProcessor* obj){
    delete obj;
  }
};

