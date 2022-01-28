#ifndef __QuantumHarmonicOscillatorPatch_hpp__
#define __QuantumHarmonicOscillatorPatch_hpp__

#include "MonochromeScreenPatch.h"
#include "OpenWareLibrary.h"
#include "HarmonicOscillator.hpp"

template<size_t WIDTH, size_t HEIGHT, size_t OFFSET>
class QuantumDisplay {
protected:
  uint8_t buffer[WIDTH]; // reduced sample buffer
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
    // for(size_t x=0; x<WIDTH; ++x)
    //   screen.setPixel(x, buffer[x] + OFFSET, WHITE);
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

class QuantumHarmonicOscillatorPatch : public MonochromeScreenPatch {
  typedef WavetableOscillator Osc;
  // typedef AntialiasedWavetableOscillator Osc;
private:
  static const int samples = 1024;
  static const int controls = 5;
  HarmonicOscillator<samples/2, controls> harms;
  Osc* osc;
  FloatArray wavetable;
  VoltsPerOctave hz;
  QuantumDisplay<128, 47, 0> display;
  float glauber = 0;
  Window window;
  BiquadFilter* filter;
  SmoothFloat rms;
public:
  QuantumHarmonicOscillatorPatch(){
    registerParameter(PARAMETER_A, "Tune");
    registerParameter(PARAMETER_B, "Rate");
    registerParameter(PARAMETER_C, "H2");
    registerParameter(PARAMETER_D, "H3");
    registerParameter(PARAMETER_E, "H4");
    registerParameter(PARAMETER_F, "RMS>");
    registerParameter(PARAMETER_G, "Energy>");
    registerParameter(PARAMETER_AA, "Glauber");
    setParameterValue(PARAMETER_A, 0.2);
    setParameterValue(PARAMETER_B, 0.2);
    setParameterValue(PARAMETER_AA, 0.2);
    wavetable = FloatArray::create(samples);
    osc = new Osc(getSampleRate(), wavetable);
    osc->setFrequency(440);
    harms.calculate(wavetable, samples/2);
    // window = Window::create(Window::HannWindow, samples);
    // window = Window::create(Window::WelchWindow, samples);
    window = Window::create(Window::SineWindow, samples/2);
    filter = BiquadFilter::create(getSampleRate(), 2);
    filter->setLowPass(2000, FilterStage::BUTTERWORTH_Q);
  }
  ~QuantumHarmonicOscillatorPatch(){
    delete osc;
  }
  void processAudio(AudioBuffer &buffer) {
    float tune = getParameterValue(PARAMETER_A)*3 - 3;
    float ts = getParameterValue(PARAMETER_B);
    harms.setTimeStep(ts*ts*ts);
    if(getParameterValue(PARAMETER_AA) != glauber){
      glauber = getParameterValue(PARAMETER_AA);
      harms.setGlauberState(glauber);
      harms.normalizeAmplitudes();
      setParameterValue(PARAMETER_C, harms.getControl(1));
      setParameterValue(PARAMETER_D, harms.getControl(2));
      setParameterValue(PARAMETER_E, harms.getControl(3));
      // setParameterValue(PARAMETER_F, harms.getControl(4));
    }else{
      harms.setControl(1, getParameterValue(PARAMETER_C));
      harms.setControl(2, getParameterValue(PARAMETER_D));
      harms.setControl(3, getParameterValue(PARAMETER_E));
      // harms.setControl(4, getParameterValue(PARAMETER_F));
      harms.normalizeAmplitudes();
    }
    harms.calculateNormalized(wavetable, samples/2);
    harms.increment();

    // FloatArray half = wavetable.subArray(0, samples/2);
    // window.process(half, half);
    for(size_t i=0; i<samples/2; ++i)
      wavetable[(samples-1)-i] = wavetable[i];
    // size_t mid = samples/2;
    // wavetable[mid-1] = Interpolator::cubic(wavetable[mid-2], wavetable[mid-1], wavetable[mid-0], 0.5);
    display.update(wavetable);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hz.setTune(tune);
    float freq = hz.getFrequency(left[0]);
    osc->setFrequency(freq);
    osc->generate(left);
    left.add(-0.5f);
    float ae = harms.getAverageEnergy(); // 0.5 to 4.03
    filter->setLowPass(freq*3, ae);
    filter->process(left, left);
    // left.softclip();
    left.tanh();
    rms = left.getRms();
    right.copyFrom(left);
    setParameterValue(PARAMETER_F, rms);
    setParameterValue(PARAMETER_G, ae/4);
  }
  void processScreen(MonochromeScreenBuffer& screen){
    display.draw(screen);
    // screen.print(10, 20, "e ");
    // screen.print(harms.getAverageEnergy());
  }
};

#endif   // __QuantumHarmonicOscillatorPatch_hpp__
