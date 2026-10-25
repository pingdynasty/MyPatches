#ifndef __TestSignalPatch_hpp__
#define __TestSignalPatch_hpp__

#include "Patch.h"
#include "SineOscillator.h"

/**
 * Generates a 1k Hz sine tone at -6dB on all channels.
 */
class TestSignalPatch : public Patch {
private:
  SineOscillator* osc;
public:
  TestSignalPatch() {
    osc = SineOscillator::create(getSampleRate());
    osc->setFrequency(440);
    debugMessage("SR/BS/CH", (int)getSampleRate(), getBlockSize(), getNumberOfChannels());    
  }
  ~TestSignalPatch() {
    SineOscillator::destroy(osc);
  }
  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    osc->generate(left);
    left.multiply(0.5); // -6dB
    for(size_t ch=1; ch<getNumberOfChannels(); ++ch)
      left.copyTo(buffer.getSamples(ch));
  }
};

#endif   // __TestSignalPatch_hpp__
