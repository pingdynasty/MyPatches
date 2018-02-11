#ifndef __TimeFreezePatch_hpp__
#define __TimeFreezePatch_hpp__

#include "StompBox.h"
#include "Window.h"
#include "NoiseOscillator.h"

class TimeFreezePatch : public Patch {
private:
  // NoiseOscillator noise;
  FloatArray noise;
  FloatArray buffer;
  FloatArray dest;
  Window window;
  enum PatchMode {
    RECORD,
    FREEZE
  };
  PatchMode mode = RECORD;
public:
  TimeFreezePatch(){
    noise = FloatArray::create(getBlockSize());
    buffer = FloatArray::create(getBlockSize());
    dest = FloatArray::create(getBlockSize()*2);
    window = Window::create(Window::HanningWindow, getBlockSize());
    registerParameter(PARAMETER_D, "Dry/Wet");
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON && value)
      mode = mode == FREEZE ? RECORD : FREEZE;
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray samples = buffer.getSamples(LEFT_CHANNEL);
    switch(mode){
    case RECORD:
      record(samples);
      break;
    case FREEZE:
      freeze();
      break;
    }
    float wet = getParameterValue(PARAMETER_D);
    for(int n=0; n<samples.getSize(); ++n)
      samples[n] = samples[n]*(1-wet) + dest[n]*wet;
  }
  void record(FloatArray in){
    buffer.copyFrom(in);
    window.apply(buffer);
    in.copyTo(dest);
  }

  void freeze(){
    noise.noise();
    buffer.convolve(noise, dest);
  }
};

#endif // __TimeFreezePatch_hpp__
