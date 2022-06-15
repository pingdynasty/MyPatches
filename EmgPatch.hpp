#ifndef __EmgPatch_hpp__
#define __EmgPatch_hpp__

#include "Patch.h"
#include "OpenWareLibrary.h"

#define USE_DCBLOCKER

template<class Processor>
class BufferingSignalProcessor : public Processor {
protected:
  CircularFloatBuffer* in;
  CircularFloatBuffer* out;
  FloatArray buffer; // contiguous data
public:
  template <typename... Args>
  BufferingSignalProcessor(CircularFloatBuffer* in,
			   CircularFloatBuffer* out,
			   FloatArray buffer,
			   Args&&... args)
    : Processor(std::forward<Args>(args)...), in(in), out(out), buffer(buffer) {}
  CircularFloatBuffer* getInputBuffer(){
    return in;
  }
  CircularFloatBuffer* getOutputBuffer(){
    return out;
  }
  void process(FloatArray input, FloatArray output){
    in->write(input.getData(), input.getSize());
    while(in->getReadCapacity() >= buffer.getSize()){
      in->read(buffer.getData(), buffer.getSize());
      Processor::process(buffer, buffer);
      out->write(buffer.getData(), buffer.getSize());
    }
    return out->read(output.getData(), output.getSize());
  }
  template <typename... Args>
  static BufferingSignalProcessor* create(size_t buflen, size_t blocksize, Args&&... args){
    return new BufferingSignalProcessor(CircularFloatBuffer::create(buflen),
					CircularFloatBuffer::create(buflen),
					FloatArray::create(blocksize),
					std::forward<Args>(args)...);
  }
  static void destroy(BufferingSignalProcessor* obj){
    CircularFloatBuffer::destroy(obj->in);
    CircularFloatBuffer::destroy(obj->out);
    FloatArray::destroy(obj->buffer);
    delete obj;
  }
};
static size_t stepcounter = 0;

class EmgEnvelope : public SignalProcessor {
  float ysquared = 0.0f;
  float lambda = 0.995f;
public:
  void update(float x){
    /* RMS detection with smoothing coefficient (Moving RMS) as per:
     * R. J. Cassidy, “Level Detection Tunings and Techniques for the Dynamic Range Compression of Audio Signals,” presented at the 117th Convention of the Audio Engineering Society (2004 Oct.), convention paper 6235.
     * Referenced in https://www.eecs.qmul.ac.uk/~josh/documents/2012/GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf
     */
    ysquared = lambda*ysquared + (1.0f-lambda)*x*x;
  }
  void setLambda(float lambda){
    this->lambda = lambda;
  }
  /* process a single sample and return the result */
  float process(float in){
    update(in);
    return getAverage();
  }
  void process(FloatArray input, FloatArray output){
    for(size_t i=0; i<input.getSize(); i++){
      update(input[i]);
      output[i] = getAverage();
    }
  }
  void process(FloatArray input){
    for(size_t i=0; i<input.getSize(); i++)
      update(input[i]);      
  }
  float getAverage(){
    return sqrtf(ysquared);
  }
};

class EmgSignalProcessor : public SignalProcessor {
  Patch* patch;
  uint8_t ch;
  CircularFloatBuffer* buffer;
  DcBlockingFilter* dcblocker;
  BiquadFilter* hpf;
  BiquadFilter* notch;
  BiquadFilter* lpf;
  EmgEnvelope env;
  // ExponentialMovingAverage env;
  float gain = 1;
  size_t counter = 0;
  static constexpr size_t channels = 7;
  static constexpr size_t stepdiv = 4;
public:
  EmgSignalProcessor(Patch* patch): patch(patch) {
    float sr = patch->getSampleRate();
    dcblocker = DcBlockingFilter::create();
    hpf = BiquadFilter::create(sr, 2);
    notch = BiquadFilter::create(sr, 2);
    lpf = BiquadFilter::create(sr, 2);
    lpf->setLowPass(sr / patch->getBlockSize(), FilterStage::BUTTERWORTH_Q);
  }
  ~EmgSignalProcessor(){
    DcBlockingFilter::destroy(dcblocker);
    BiquadFilter::destroy(hpf);
    BiquadFilter::destroy(notch);
    BiquadFilter::destroy(lpf);
  }
  void setChannel(size_t channel){
    ch = channel;
  }
  void setParameters(float gain, float hpfc, float notchq, float alpha){
    this->gain = gain;
    hpf->setHighPass(hpfc, FilterStage::BUTTERWORTH_Q);
    notch->setNotch(50.0f, notchq);
    env.setLambda(alpha);
  }
  void process(FloatArray input, FloatArray output){
    if(ch < 4){ // EMG/EEG signal
#if defined(USE_DCBLOCKER)
      dcblocker->process(input, output);
#else
      hpf->process(input, output);
#endif
      notch->process(output, output);
      lpf->process(output, output);
      output.multiply(gain);
      output.clip();
      sendPitchBend(output.getMean());
      env.process(output); // envelope calculated on filtered signal
      sendEnvelope(env.getAverage());
    }else{ // Accelerometer signal
      sendPitchBend(input.getMean());
      env.process(input); // envelope calculated on raw signal
      sendEnvelope(env.getAverage());
    }
  }
  void sendEnvelope(float average){
    MidiMessage msg = MidiMessage::cc(1+ch, 1, average*128); // Control Change 1 envelope data
    if((++counter % (channels*stepdiv)) == ch*stepdiv)
      patch->sendMidi(msg);
  }
  void sendPitchBend(float mean){
    MidiMessage msg = MidiMessage::pb(1+ch, mean*8192); // Pitch Bend filtered EMG data
    if((counter % (channels*stepdiv)) == ch*stepdiv)
      patch->sendMidi(msg);
  }
  static EmgSignalProcessor* create(size_t buflen, size_t blocksize, Patch* patch){
    return new EmgSignalProcessor(patch);
  }
  static void destroy(EmgSignalProcessor* obj){
    delete obj;
  }
};

// typedef BufferingSignalProcessor<EmgSignalProcessor> Processor;
typedef EmgSignalProcessor Processor;

// #define EMG_BLOCKSIZE 16 // output rate: 8k/16 = 500 Hz
// #define EMG_BLOCKSIZE 32 // output rate: 8k/32 = 250 Hz
// #define EMG_BLOCKSIZE 1024 // output rate: 8k/32 = 250 Hz
#define EMG_BLOCKSIZE 128

class EmgPatch : public Patch {
private:
  Processor** processor = NULL;
  FloatArray outbuffer;
public:
  EmgPatch() {
    debugMessage("EMG v10 SR/BS/CH", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());
    size_t channels = getNumberOfChannels();
    // size_t buflen = getBlockSize() * channels + EMG_BLOCKSIZE;
    size_t buflen = EMG_BLOCKSIZE;
    processor = new Processor*[channels];
    for(size_t ch=0; ch<channels; ++ch){
      processor[ch] = Processor::create(buflen, EMG_BLOCKSIZE, this);
      processor[ch]->setChannel(ch);

      // processor[ch]->getInputBuffer()->moveReadHead(getBlockSize()*ch);
    }
    registerParameter(PARAMETER_A, "Gain");
    registerParameter(PARAMETER_B, "HP Fc");
    registerParameter(PARAMETER_C, "Notch Q");
#ifdef SEND_ENV_DATA
    registerParameter(PARAMETER_D, "Env Alpha");
#endif
    // initialise parameter settings
    setParameterValue(PARAMETER_A, 0.25);
    setParameterValue(PARAMETER_B, 0.25);
    setParameterValue(PARAMETER_C, 0.25);
    setParameterValue(PARAMETER_D, 0);
    outbuffer = FloatArray::create(getBlockSize());
  }
  ~EmgPatch() {
    for(size_t ch=0; ch<getNumberOfChannels(); ++ch)
      Processor::destroy(processor[ch]);
    delete[] processor;
  }

  void processAudio(AudioBuffer &buffer) {
    float gain = getParameterValue(PARAMETER_A)*1000+1;
    float hpfc = getParameterValue(PARAMETER_B)*10+10;
    float q = getParameterValue(PARAMETER_C)*20+0.707;
    float alpha = getParameterValue(PARAMETER_D)*0.8+0.2;
    for(size_t ch=0; ch<buffer.getChannels(); ++ch){
      FloatArray samples = buffer.getSamples(ch);
      processor[ch]->setParameters(gain, hpfc, q, alpha);      
      processor[ch]->process(samples, outbuffer);
      // samples.copyFrom(outbuffer);
    }
  }

};

#endif   // __EmgPatch_hpp__
