#ifndef __MultiConvolutionPatch_hpp__
#define __MultiConvolutionPatch_hpp__

#include "Patch.h"
#include "OpenWareLibrary.h"

#define IR_RESOURCE "ir.wav"

// sox ir/EMT\ 244\ \(A.\ Bernhard\)/0\,4s\ Low\*2\ \ High_2.wav -c 1 -e float ../ir.wav
// 92% to 102% on Genius with bs=256, 111 segments, 592mS

// sox hang-sfz/samples/A3_light.wav  -c 1 -e float ../../hang-a3.wav trim 0 1


class ImpulseResponse {
  ComplexFloatArray* irbuf;
  size_t segments;
public:
  ImpulseResponse(ComplexFloatArray* irbuf, size_t segments)
    : irbuf(irbuf), segments(segments) {}
  static void destroy(ImpulseResponse* obj){
    for(size_t i=0; i<obj->segments; ++i)
      ComplexFloatArray::destroy(obj->irbuf[i]);
    delete[] obj->irbuf;
    delete obj;
  }
  static ImpulseResponse* create(FastFourierTransform* fft, FloatArray impulse, size_t blocksize, size_t segments, FloatArray input){
    size_t segsize = input.getSize();
    ComplexFloatArray* irbuf = new ComplexFloatArray[segments];
    // do forward fft into irbuf
    for(size_t i=0; i<segments; ++i){
      // allocate fft buffers
      irbuf[i] = ComplexFloatArray::create(segsize);
      // load IR and zero-pad to segment size
      input.clear();
      input.copyFrom(impulse.subArray(i*blocksize, blocksize));
      fft->fft(input, irbuf[i]);
    }
    return new ImpulseResponse(irbuf, segments);
  }
  void process(ComplexFloatArray in, ComplexFloatArray out, size_t segment){
    // complex multiply and accumulate
    irbuf[segment].complexByComplexMultiplication(in, out);
  }
};

class ConvolutionSignalProcessor : public SignalProcessor {
protected:
  ImpulseResponse* ir;
  ComplexFloatArray* cinbuf;
  ComplexFloatArray coutbuf;
  ComplexFloatArray cmulbuf;
  FloatArray input;
  FloatArray overlap;
  FastFourierTransform* fft;
  size_t segments;
  float mix = 1;
  int current = 0;
public:
  ConvolutionSignalProcessor(ComplexFloatArray* cinbuf,
			     ComplexFloatArray coutbuf,
			     ComplexFloatArray cmulbuf,
			     FloatArray input,
			     FloatArray overlap,
			     FastFourierTransform* fft,
			     size_t segments)
    : cinbuf(cinbuf), coutbuf(coutbuf), cmulbuf(cmulbuf),
      input(input), overlap(overlap), fft(fft), segments(segments){}

  ImpulseResponse* createImpulseResponse(FloatArray impulse){
    size_t blocksize = overlap.getSize();
    return ImpulseResponse::create(fft, impulse, blocksize, segments, input);
  }
  void setImpulseResponse(ImpulseResponse* ir){
    this->ir = ir;
  }
  void setMix(float value){
    mix = value;
  }
  void process(FloatArray in, FloatArray out){
    size_t blocksize = overlap.getSize();

    // zero-pad input samples
    input.clear();
    input.copyFrom(in);

    // forward fft
    fft->fft(input, cinbuf[current]);

    // complex multiply and accumulate
    ir->process(cinbuf[current], coutbuf, 0);
    // irbuf[0].complexByComplexMultiplication(cinbuf[current], coutbuf);
    for(size_t i=1; i<segments; ++i){
      int seg = (current+i) % segments;
      ir->process(cinbuf[seg], cmulbuf, i);
      // irbuf[i].complexByComplexMultiplication(cinbuf[seg], cmulbuf);
      coutbuf.add(cmulbuf);
    }
    // inverse fft
    fft->ifft(coutbuf, input);

    for(size_t i=0; i<blocksize; ++i)
      out[i] = mix*(input[i]+overlap[i]) + (1 - mix)*in[i];

    // save overlap
    overlap.copyFrom(input.subArray(blocksize, blocksize));
    current = (current > 0) ? (current - 1) : (segments - 1);
  }
  static ConvolutionSignalProcessor* create(size_t blocksize, size_t segments, size_t segsize){
    ComplexFloatArray* cinbuf = new ComplexFloatArray[segments];
    ComplexFloatArray coutbuf;
    ComplexFloatArray cmulbuf;
    FloatArray input = FloatArray::create(segsize);
    FloatArray overlap;
    FastFourierTransform* fft = FastFourierTransform::create(segsize);

    coutbuf = ComplexFloatArray::create(segsize);
    cmulbuf = ComplexFloatArray::create(segsize);
    overlap = FloatArray::create(blocksize);

    overlap.clear();

    for(size_t i=0; i<segments; ++i){
      // allocate fft buffers for complex multiply and accumulate
      cinbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i].clear();
    }
    return new ConvolutionSignalProcessor(cinbuf, coutbuf, cmulbuf, input, overlap, fft, segments);
  }
  static void destroy(ConvolutionSignalProcessor* obj){
    for(size_t i=0; i<obj->segments; ++i)
      ComplexFloatArray::destroy(obj->cinbuf[i]);
    delete[] obj->cinbuf;
    ComplexFloatArray::destroy(obj->cmulbuf);
    ComplexFloatArray::destroy(obj->coutbuf);
    FloatArray::destroy(obj->input);
    FloatArray::destroy(obj->overlap);
    FastFourierTransform::destroy(obj->fft);
    delete obj;
  }
};

class MultiConvolutionPatch : public Patch {
private:
  ConvolutionSignalProcessor* convolution;
  static constexpr size_t ir_count = 3;
  ImpulseResponse* ir[ir_count];
  DcBlockingFilter dc;
  EnvelopeFollower ef;
public:
  MultiConvolutionPatch() {
    registerParameter(PARAMETER_A, "Gain");
    setParameterValue(PARAMETER_A, 0.5);
    registerParameter(PARAMETER_B, "IR");
    setParameterValue(PARAMETER_B, 0.5);
    registerParameter(PARAMETER_C, "Mix");
    setParameterValue(PARAMETER_C, 1);

    size_t blocksize = getBlockSize();
    size_t segsize = 2*blocksize;

    size_t segments = 47*1.8; // performance roughly scales with segments: 47=47%, 56=56% (bs=1024)
    convolution = ConvolutionSignalProcessor::create(blocksize, segments, segsize);

    FloatArray impulse = FloatArray::create(segments*blocksize);    
    // ir[0] = loadImpulseResponse(impulse, "kalimba-c4s.wav");
    // // ir[0] = loadImpulseResponse(impulse, "marimba-f4.wav");
    // // ir[0] = loadImpulseResponse(impulse, "hang-a3.wav");
    // ir[1] = loadImpulseResponse(impulse, "hang-c4s.wav");
    // ir[2] = loadImpulseResponse(impulse, "hang-d3.wav");
    // // ir[3] = loadImpulseResponse(impulse, "ir.wav");
    // // ir[4] = loadImpulseResponse(impulse, "marimba-b3.wav");
    // // ir[5] = loadImpulseResponse(impulse, "marimba-d4.wav");
    // // ir[6] = loadImpulseResponse(impulse, "marimba-e5.wav");

    ir[0] = loadImpulseResponse(impulse, "marimba-b3.wav");
    ir[1] = loadImpulseResponse(impulse, "marimba-d4.wav");
    ir[2] = loadImpulseResponse(impulse, "marimba-g4.wav");
    // debugMessage("segments/mS", segments, 1000*impulse.getSize()/getSampleRate());
    debugMessage("ch/bs/sr", getNumberOfChannels(), getBlockSize(), (int)getSampleRate());
    
    FloatArray::destroy(impulse);
    convolution->setImpulseResponse(ir[0]);
    stimulus = FloatArray::create(getBlockSize());
  }
  ImpulseResponse* loadImpulseResponse(FloatArray impulse, const char* name){
    Resource* resource = getResource(name);
    WavFile wav(resource->getData(), resource->getSize());
    if(!wav.isValid())
      // error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
      error(CONFIGURATION_ERROR_STATUS, name);
    wav.read(0, impulse);
    Resource::destroy(resource);    
    float scale = impulse.getMaxValue() - impulse.getMinValue();
    impulse.multiply(1/scale); // normalise IR level to -6dB peak
    // debugMessage("rms/scale", impulse.getRms()*100, scale);
    // impulse.reverse();
    return convolution->createImpulseResponse(impulse);
  }
  ~MultiConvolutionPatch(){
    ConvolutionSignalProcessor::destroy(convolution);
    for(size_t i=0; i<ir_count; ++i)
      ImpulseResponse::destroy(ir[i]);
    FloatArray::destroy(stimulus);
  }
  FloatArray stimulus;
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      if(value)
	stimulus[0] = 10; // dirac
      break;
    case BUTTON_2:
      if(value)
	stimulus.noise();
      break;
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float gain = getParameterValue(PARAMETER_A);
    size_t select = getParameterValue(PARAMETER_B)*ir_count;
    float mix = getParameterValue(PARAMETER_C);
    convolution->setImpulseResponse(ir[select]);
    convolution->setMix(mix);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    dc.process(left, left);
    // left.clear();
    left.add(stimulus);
    if(!isButtonPressed(BUTTON_2))
      stimulus.clear();
    left.multiply(gain);
    float rms = left.getRms();
    // convolution->process(left, left);
    convolution->process(left, right);
    // ef.process(left, left);
    // right.multiply(left);
    // float outrms = right.getRms();
    // if(outrms > 0.0001)
    //   right.multiply(rms/outrms);

    // left.softclip();
    // left.tanh();
    // buffer.getSamples(RIGHT_CHANNEL).copyFrom(left);
  }
};

#endif   // __MultiConvolutionPatch_hpp__
