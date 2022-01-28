#ifndef __QuantumHarmonicOscillatorPatch_hpp__
#define __QuantumHarmonicOscillatorPatch_hpp__

#include "MonochromeScreenPatch.h"
#include "OpenWareLibrary.h"
#include "HarmonicOscillator.hpp"

template<size_t WIDTH, size_t HEIGHT, size_t OFFSET>
class QuantumDisplay {
protected:
  uint8_t buffer[WIDTH];
public:
  void update(FloatArray samples){
    size_t len = samples.getSize();
    size_t stepsize = len/WIDTH;
    float* src = samples.getData();
    for(size_t i=0; i<WIDTH; ++i){
      buffer[i] = HEIGHT - *src * HEIGHT + OFFSET;
      src += stepsize;
    }
  }
  void draw(MonochromeScreenBuffer& screen){
    int y = buffer[0];
    for(size_t x=0; x<WIDTH; ++x)
      y = drawVerticalLine(screen, x, y, buffer[x], WHITE);
  }
  uint16_t drawVerticalLine(MonochromeScreenBuffer& screen, uint16_t x, uint16_t y, uint16_t to, uint16_t c){
    if(y > to)
      drawVerticalLine(screen, x, to, y, c);
    else
      do{
	screen.setPixel(x, y++, c);
      }while(y < to);
    return to;
  }
};

template<size_t samples, size_t controls, InterpolationMethod im = COSINE_INTERPOLATION>
class QuantumHarmonicGenerator : public HarmonicOscillator<samples, controls>, SignalGenerator {
  typedef HarmonicOscillator<samples, controls> QHO;
protected:
  InterpolatingCircularFloatBuffer<im>* wavetable;
  float rate = 1;
  float pos = 0;
public:
  QuantumHarmonicGenerator(float sr){
    wavetable = InterpolatingCircularFloatBuffer<im>::create(samples*2);
  }
  ~QuantumHarmonicGenerator(){
    InterpolatingCircularFloatBuffer<im>::destroy(wavetable);
  }
  void setFrequency(float freq){    
    rate = freq/110.0f;
  }
  void generate(FloatArray output){
    QHO::calculateNormalized(wavetable->getData(), samples);
    QHO::increment();
    float* wt = wavetable->getData();
    for(size_t i=0; i<samples; ++i)
      wt[(samples*2-1)-i] = wt[i];
    size_t len = output.getSize();
    wavetable->read(output.getData(), len, pos, rate);
    pos = fmodf(pos+rate*len, samples*2);
  }
  FloatArray getWavetable(){
    return FloatArray(wavetable->getData(), samples*2);
  }
};


template<size_t samples, size_t controls, InterpolationMethod im = COSINE_INTERPOLATION>
class HarmonicWavetableOscillator : public HarmonicOscillator<samples, controls>, SignalGenerator {
  typedef HarmonicOscillator<samples, controls> QHO;
  typedef WavetableOscillator Osc;
  // typedef AntialiasedWavetableOscillator Osc;
protected:
  Osc* osc;
  FloatArray wavetable;
  float rate = 1;
  float pos = 0;
public:
  HarmonicWavetableOscillator(float sr){
    wavetable = FloatArray::create(samples*2);
    osc = new Osc(sr, wavetable);
  }
  ~HarmonicWavetableOscillator(){
    FloatArray::destroy(wavetable);
  }
  void setFrequency(float freq){
    osc->setFrequency(freq);
  }
  void generate(FloatArray output){
    QHO::calculateNormalized(wavetable, samples);
    QHO::increment();
    for(size_t i=0; i<samples; ++i)
      wavetable[(samples*2-1)-i] = wavetable[i];
    osc->generate(output);
    output.add(-0.5f);
  }
  FloatArray getWavetable(){
    return wavetable;
  }
};

class QuantumHarmonicOscillatorPatch : public MonochromeScreenPatch {
private:
  static const int samples = 512;
  static const int controls = 5;
  // typedef HarmonicWavetableOscillator<samples, controls> QuantumOsc;
  typedef QuantumHarmonicGenerator<samples, controls, HERMITE_INTERPOLATION> QuantumOsc;
  CircularFloatBuffer* wavetable;
  QuantumOsc* quantum;
  VoltsPerOctave hz;
  QuantumDisplay<128, 47, 0> display;
  float glauber = 0;
  StateVariableFilter* filter;
  SmoothFloat freq;
  SmoothFloat fc;
public:
  QuantumHarmonicOscillatorPatch(){
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Rate");
    registerParameter(PARAMETER_C, "Coherence");
    registerParameter(PARAMETER_D, "Filter");
    registerParameter(PARAMETER_E, "Resonance");
    registerParameter(PARAMETER_F, "LFO>");
    registerParameter(PARAMETER_G, "Energy>");
    registerParameter(PARAMETER_AA, "H1");
    registerParameter(PARAMETER_AB, "H2");
    registerParameter(PARAMETER_AC, "H3");
    registerParameter(PARAMETER_AD, "H4");
    registerParameter(PARAMETER_AE, "H5");
    setParameterValue(PARAMETER_A, 0.2);
    setParameterValue(PARAMETER_B, 0.2);
    setParameterValue(PARAMETER_C, 0.2);
    quantum = new QuantumOsc(getSampleRate());
    filter = StateVariableFilter::create(getSampleRate());
    filter->setLowPass(6000, FilterStage::BUTTERWORTH_Q);
    FloatArray wt = quantum->getWavetable();
    wavetable = new CircularFloatBuffer(wt.getData(), wt.getSize());
  }
  ~QuantumHarmonicOscillatorPatch(){
    delete quantum;
    delete wavetable;
    StateVariableFilter::destroy(filter);
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*3 - 3;
    float ts = getParameterValue(PARAMETER_B);
    quantum->setTimeStep(ts*ts*ts);
    if(getParameterValue(PARAMETER_C) != glauber){
      glauber = getParameterValue(PARAMETER_C);
      quantum->setGlauberState(glauber);
      quantum->normalizeAmplitudes();
      setParameterValue(PARAMETER_AA, quantum->getControl(0));
      setParameterValue(PARAMETER_AB, quantum->getControl(1));
      setParameterValue(PARAMETER_AC, quantum->getControl(2));
      setParameterValue(PARAMETER_AD, quantum->getControl(3));
      setParameterValue(PARAMETER_AE, quantum->getControl(4));
    }else{
      quantum->setControl(0, getParameterValue(PARAMETER_AA));
      quantum->setControl(1, getParameterValue(PARAMETER_AB));
      quantum->setControl(2, getParameterValue(PARAMETER_AC));
      quantum->setControl(3, getParameterValue(PARAMETER_AD));
      quantum->setControl(4, getParameterValue(PARAMETER_AE));
      quantum->normalizeAmplitudes();
    }    
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune);
    freq = hz.getFrequency(left[0]);
    quantum->setFrequency(freq);
    quantum->generate(left);
    float ae = quantum->getAverageEnergy(); // 0.5 to 4.03
    float fcamount = getParameterValue(PARAMETER_D)*7+1;
    fc = clamp(freq*fcamount, 80.0f, getSampleRate()*0.4f);
    float qamount = getParameterValue(PARAMETER_E)*2;
    float q = ae*qamount + FilterStage::BUTTERWORTH_Q;
    filter->setLowPass(fc, q);
    filter->process(left, left);
    left.tanh();
    right.copyFrom(left);
    setParameterValue(PARAMETER_F, wavetable->read());
    setParameterValue(PARAMETER_G, ae/4);
    display.update(quantum->getWavetable());
  }
  void processScreen(MonochromeScreenBuffer& screen){
    display.draw(screen);
  }
};

#endif   // __QuantumHarmonicOscillatorPatch_hpp__
