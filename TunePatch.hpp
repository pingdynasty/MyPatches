#ifndef __TunePatch_hpp__
#define __TunePatch_hpp__

#include "StompBox.h"
#include "PitchDetector.h"

#include "AudioDisplay.hpp"

class TunePatch : public Patch {
private:
  AudioDisplay display;
  // SmoothFloat min, max, avg;
  ZeroCrossingPitchDetector* pitch;
public:
  TunePatch() {
    pitch = new ZeroCrossingPitchDetector(getSampleRate(), getBlockSize());
    registerParameter(PARAMETER_A, "Tune Left");
    registerParameter(PARAMETER_B, "Tune Right");
    registerParameter(PARAMETER_C, "Gain");
    registerParameter(PARAMETER_D, "Offset");

    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.5);
    setParameterValue(PARAMETER_C, 0.7);
    setParameterValue(PARAMETER_D, 0.5);

    // pitch->setHighPassCutoff(getParameterValue(PARAMETER_B)*15000+150);
    // pitch->setLowPassCutoff(getParameterValue(PARAMETER_C)*500+50);

    // display.setTriggerMode(AudioDisplay::TRIGGER_FREE);
  }

  ~TunePatch() {
    delete pitch;
  }

  void processAudio(AudioBuffer &buffer) {
    float tuneLeft = getParameterValue(PARAMETER_A)*2-1;
    float tuneRight = getParameterValue(PARAMETER_B)*2-1;
    float gain = getParameterValue(PARAMETER_C)*30+1;
    float offset = getParameterValue(PARAMETER_D)*20-10;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    // FloatArray data = buffer.getSamples(LEFT_CHANNEL);
    // min = data.getMinValue();
    // max = data.getMaxValue();
    // avg = data.getMean();
    // debugMessage("min/max/avg", min*1000, max*1000, avg*1000);

    left.multiply(gain);
    left.add(offset);
    
    display.update(left, 2, 0.07, 3.0, 0.0);

    pitch->process(left);
    // debugMessage("fc/rms", pitch->getFrequency(), left.getRms());

    left.setAll(tuneLeft);
    right.setAll(tuneRight);
  }

  void processScreen(ScreenBuffer& screen){
    display.draw(screen, WHITE);
    screen.print(1, 76, "fc ");
    screen.print(pitch->getFrequency());
   }
};

#endif   // __TunePatch_hpp__
