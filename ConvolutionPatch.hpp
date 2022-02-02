#ifndef __ConvolutionPatch_hpp__
#define __ConvolutionPatch_hpp__

#include "Patch.h"
#include "OpenWareLibrary.h"
// ./wav2float MyPatches/ir/EMT\ 244\ \(A.\ Bernhard\)/0\,4s\ Low\*2\ \ High_2.wav > MyPatches/impulse-response.h

#define IR_RESOURCE "ir.wav"
#define REPURPOSE

class BufferedSignalProcessor : public SignalProcessor {
protected:
  CircularFloatBuffer* input_buffer;
  CircularFloatBuffer* output_buffer;
  SignalProcessor* processor;
  size_t blocksize;
public:
  BufferedSignalProcessor(CircularFloatBuffer* in, CircularFloatBuffer* out,
			  SignalProcessor* processor, size_t blocksize)
    : input_buffer(in), output_buffer(out), processor(processor), blocksize(blocksize) {}
  virtual void process(FloatArray input, FloatArray output){
    size_t len = input.getSize();
    input_buffer->write(input.getData(), len);
    while(input_buffer->getReadCapacity() >= blocksize){
      // buffer size must be a multiple of blocksize
      ASSERT(input_buffer->getContiguousReadCapacity() >= blocksize, "Misaligned input buffer");
      ASSERT(output_buffer->getContiguousReadCapacity() >= blocksize, "Misaligned output buffer");
      FloatArray in(input_buffer->getReadHead(), blocksize);
      FloatArray out(output_buffer->getWriteHead(), blocksize);
      processor->process(in, out);
      input_buffer->moveReadHead(blocksize);
      output_buffer->moveWriteHead(blocksize);
    }
    output_buffer->read(output.getData(), len);
  }
  static BufferedSignalProcessor* create(size_t buffer_size, size_t out_blocksize, SignalProcessor* processor){
    return new BufferedSignalProcessor(CircularFloatBuffer::create(buffer_size), CircularFloatBuffer::create(buffer_size), processor, out_blocksize);				       
  }
};

class ConvolutionSignalProcessor : public SignalProcessor {
protected:
  ComplexFloatArray* irbuf;
  ComplexFloatArray* cinbuf;
  ComplexFloatArray coutbuf;
  ComplexFloatArray cmulbuf;
  FloatArray input;
  FloatArray overlap;
  FastFourierTransform* fft;
  size_t segments = 11;
  float mix = 1;
  int current = 0;
public:
  ConvolutionSignalProcessor(ComplexFloatArray* irbuf,
			     ComplexFloatArray* cinbuf,
			     ComplexFloatArray coutbuf,
			     ComplexFloatArray cmulbuf,
			     FloatArray input,
			     FloatArray overlap,
			     FastFourierTransform* fft,
			     size_t segments)
    : irbuf(irbuf), cinbuf(cinbuf), coutbuf(coutbuf), cmulbuf(cmulbuf),
      input(input), overlap(overlap), fft(fft), segments(segments){}

  static ConvolutionSignalProcessor* create(FloatArray impulse, size_t blocksize, size_t segments, size_t segsize){
    ComplexFloatArray* irbuf = new ComplexFloatArray[segments];
    ComplexFloatArray* cinbuf = new ComplexFloatArray[segments];
    ComplexFloatArray coutbuf;
    ComplexFloatArray cmulbuf;
    FloatArray input = FloatArray::create(segsize);
    FloatArray overlap;
    FastFourierTransform* fft = FastFourierTransform::create(segsize);

#ifndef REPURPOSE
    coutbuf = ComplexFloatArray::create(segsize);
    cmulbuf = ComplexFloatArray::create(segsize);
    overlap = FloatArray::create(blocksize);
#endif

    overlap.clear();

    // FloatArray impulse(resource.getData()+2, resource.getSize()-2);
    // FloatArray impulse((float*)resource->getData()+2, resource->getSize()/sizeof(float)+2);
    // debugMessage("ir size/min/max", resource->getSize(), impulse.getMaxValue(), impulse.getMinValue());
    
    // normalise IR level
    float scale = 1.0f/max(impulse.getMaxValue(), -impulse.getMinValue());
    // impulse.multiply(scale);
    // debugMessage("normalised", scale);

    // do forward fft into irbuf
    for(size_t i=0; i<segments; ++i){
      // allocate fft buffers
      irbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i] = ComplexFloatArray::create(segsize);
      cinbuf[i].clear();
      // load IR and zero-pad to segment size
      input.clear();
      input.copyFrom(impulse.subArray(i*blocksize, blocksize));
      fft->fft(input, irbuf[i]);
      // fft->fft(impulse.subArray(i*blocksize, blocksize), irbuf[i]);
    }

#ifdef REPURPOSE
    // repurpose impulse response memory
    cmulbuf = ComplexFloatArray((ComplexFloat*)(float*)impulse, segsize);
    coutbuf = ComplexFloatArray((ComplexFloat*)(float*)impulse.subArray(blocksize*4, blocksize*4), segsize);
    // input = FloatArray((float*)impulse.subArray(blocksize*8, blocksize*2), segsize);
    overlap = FloatArray((float*)impulse.subArray(blocksize*8, blocksize), blocksize);
#else
    FloatArray::destroy(impulse);
#endif
    return new ConvolutionSignalProcessor(irbuf, cinbuf, coutbuf, cmulbuf, input, overlap, fft, segments);
  }
  void setMix(float value){
    mix = value;
  }
  void process(FloatArray in, FloatArray out){
    size_t blocksize = overlap.getSize();

    input.clear();
    input.copyFrom(in);

    // forward fft
    fft->fft(input, cinbuf[current]);

    // complex multiply and accumulate
    irbuf[0].complexByComplexMultiplication(cinbuf[current], coutbuf);
    for(size_t i=1; i<segments; ++i){
      int seg = (current+i) % segments;
      irbuf[i].complexByComplexMultiplication(cinbuf[seg], cmulbuf);
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
  static void destroy(ConvolutionSignalProcessor* obj){
    FloatArray::destroy(obj->input);
    for(size_t i=0; i<obj->segments; ++i){
      ComplexFloatArray::destroy(obj->irbuf[i]);
      ComplexFloatArray::destroy(obj->cinbuf[i]);
    }
    delete[] obj->irbuf;
    delete[] obj->cinbuf;
#ifdef REPURPOSE
    FloatArray::destroy(FloatArray((float*)obj->cmulbuf.getData(), obj->segments*obj->overlap.getSize()));
#else
    ComplexFloatArray::destroy(obj->cmulbuf);
    ComplexFloatArray::destroy(obj->coutbuf);
    FloatArray::destroy(obj->overlap);
#endif
    FastFourierTransform::destroy(obj->fft);
  }
};

class ConvolutionPatch : public Patch {
private:
  ConvolutionSignalProcessor* convolution;
public:
  ConvolutionPatch() {
    registerParameter(PARAMETER_D, "Wet/Dry");

    size_t blocksize = getBlockSize();
    size_t segsize = 2*blocksize;

    Resource* resource = getResource(IR_RESOURCE);
    ASSERT(resource, "Missing resource " IR_RESOURCE);

    WavFile wav(resource->getData(), resource->getSize());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");

    static const int segments = 23; // 30mS, 93% on Witch

    FloatArray impulse = FloatArray::create(segments*blocksize);
    wav.read(0, impulse);
    // FloatArray impulse = wav.createFloatArray(0);
    Resource::destroy(resource);

    debugMessage("samples/ir/mS", (int)wav->getNumberOfSamples(), impulse.getSize(), 1000*impulse.getSize()/getSampleRate());
    
    // normalise IR level
    // float scale = 1.0f/max(impulse.getMaxValue(), -impulse.getMinValue());
    // impulse.multiply(scale);
    // debugMessage("normalised", scale);

    convolution = ConvolutionSignalProcessor::create(impulse, blocksize, segments, segsize);
  }
  ~ConvolutionPatch(){
    ConvolutionSignalProcessor::destroy(convolution);
  }
  void processAudio(AudioBuffer &buffer) {
    float mix = getParameterValue(PARAMETER_D);
    convolution->setMix(mix);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    convolution->process(left, left);
    buffer.getSamples(RIGHT_CHANNEL).copyFrom(left);
  }
};

#endif   // __ConvolutionPatch_hpp__
