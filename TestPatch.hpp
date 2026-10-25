#ifndef __TestPatch_hpp__
#define __TestPatch_hpp__

#include "Patch.h"
// #include "ServiceCall.h"
// #include "SimpleValue.h"
// #include "InterpolatingSignalGenerator.h"
// #include "InterpolatingCircularBuffer.h"

#include "OpenWareLibrary.h"

class TestPatch : public Patch {
private:
  static constexpr InterpolationMethod im1 = LINEAR_INTERPOLATION;
  // static constexpr InterpolationMethod im1 = COSINE_INTERPOLATION;
  // static constexpr InterpolationMethod im1 = CUBIC_4P_INTERPOLATION;
  // static constexpr InterpolationMethod im2 = CUBIC_4P_INTERPOLATION;
  static constexpr InterpolationMethod im2 = CUBIC_4P_SMOOTH_INTERPOLATION;
  // static constexpr InterpolationMethod im2 = HERMITE_INTERPOLATION;
  // SmoothLinearFloat b = SmoothLinearFloat(0.98, LinearFloat(10.0f, 100.0f, 20.0f)); // initialise lambda and range
  InterpolatingSignalGenerator<im1>* sig1;
  InterpolatingSignalGenerator<im2>* sig2;
  SmoothFloat a = 0.5;
  SmoothFloat b = 0.5;
  // SmoothLinearFloat a = 0.5f; // initialise value
  InterpolatingCircularFloatBuffer<im2>* points;
public:
  TestPatch(){
    registerParameter(PARAMETER_A, "a");
    registerParameter(PARAMETER_B, "b");
    setParameterValue(PARAMETER_C, 0.25);
    // a.lambda = 0.8;
    // a.setRange(-10, 10);
    LinearFloat lin;
    lin.setRange(-10, 10, 10, 100);
    debugMessage("lin/in", lin, lin.getControl());
    lin = -10;
    debugMessage("lin/in", lin, lin.getControl());
    lin = 0;
    debugMessage("lin/in", lin, lin.getControl());
    lin = 10;
    debugMessage("lin/in", lin, lin.getControl());
    sig1 = InterpolatingSignalGenerator<im1>::create(getBlockSize());
    sig2 = InterpolatingSignalGenerator<im2>::create(getBlockSize());
    setParameterValue(PARAMETER_A, a);
    setParameterValue(PARAMETER_B, b);
    sig1->set(a);
    sig2->set(b);
    // b.lambda = 0.2;
    points = InterpolatingCircularFloatBuffer<im2>::create(4);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case PUSHBUTTON:
    case BUTTON_A:
      // setButton(BUTTON_B, !value);
      sendMidi(MidiMessage::note(0, 12, 24));
      debugMessage("button A", (int)bid, (int)value);
      break;
    case BUTTON_B:
      // setButton(BUTTON_A, !value);
      sendMidi(MidiMessage::note(0, 28, 48));
      debugMessage("button B", (int)bid, (int)value);
      break;
  //   // case BUTTON_A:
  //   //   setButton(BUTTON_B, value);
  //   //   setButton(BUTTON_C, !value);
  //   //   // setButton(BUTTON_E, value);
  //   //   // debugMessage("cc1/F", cc1value);
  //   //   break;
  //   // case BUTTON_B:
  //   //   setButton(BUTTON_A, value);
  //   //   // debugMessage("cc75/AA", cc75value);
  //   //   break;
    }
    // debugMessage("button", (int)bid, (int)value);
    // setButton(bid, value);
  }

  // int cc1value = 0;
  // int cc75value = 0;

  void processMidi(MidiMessage msg){
    debugMessage("midi", msg.getStatus(), (int)msg.data[2], (int)msg.data[3]);
  }

  void processAudio(AudioBuffer &buffer) {
    debugMessage("a/b", a, b);
    // a = getParameterValue(PARAMETER_A);
    b = 0.5;
    debugMessage("a/b", a, b);
    b = 0;
    debugMessage("a/b", a, b);
    // setParameterValue(PARAMETER_A, a);
    // setParameterValue(PARAMETER_B, b);
    // static float previous = 0;
    // float current = getParameterValue(PARAMETER_A);
    // // buffer.getSamples(0).ramp(previous, current);
    // buffer.getSamples(0).setAll(current);
    // buffer.getSamples(1).copyFrom(buffer.getSamples(0));
    // previous = current;
    // setParameterValue(PARAMETER_F, getParameterValue(PARAMETER_A));
    // setParameterValue(PARAMETER_G, getParameterValue(PARAMETER_B));

    // buffer.getSamples(0).setAll(getParameterValue(PARAMETER_A)*2 - 1);
    // buffer.getSamples(1).setAll(getParameterValue(PARAMETER_B)*2 - 1);
    // buffer.getSamples(2).setAll(getParameterValue(PARAMETER_C)*2 - 1);
    // buffer.getSamples(3).setAll(getParameterValue(PARAMETER_D)*2 - 1);

    // buffer.getSamples(4).setAll(getParameterValue(PARAMETER_E)*2 - 1);
    // buffer.getSamples(5).setAll(getParameterValue(PARAMETER_F)*2 - 1);
    // buffer.getSamples(6).setAll(getParameterValue(PARAMETER_G)*2 - 1);
    // buffer.getSamples(7).setAll(getParameterValue(PARAMETER_H)*2 - 1);
    // cc1value = getParameterValue(PARAMETER_F)*127;
    // cc75value = getParameterValue(PARAMETER_AA)*127;

    // static bool hardsync = false;
    // setButton(BUTTON_E, hardsync);
    // if(isButtonPressed(BUTTON_B))
    //   hardsync = !hardsync;

    // float freq = (uint16_t)(getParameterValue(PARAMETER_AA)*4096);
    // if(isButtonPressed(BUTTON_A))
    //   debugMessage("pressed", (int)(getParameterValue(PARAMETER_A)*4095));
    // else
    //   debugMessage("freq", freq);

      // debugMessage("A", (int)(getParameterValue(PARAMETER_E)*4095));
    //   debugMessage("B", (int)(getParameterValue(PARAMETER_B)*4095));

    // if(isButtonPressed(BUTTON_A))
    // debugMessage("cde", getParameterValue(PARAMETER_C), getParameterValue(PARAMETER_D), getParameterValue(PARAMETER_E));
    // // else if(isButtonPressed(BUTTON_B))
    // else
    //   debugMessage("abc", getParameterValue(PARAMETER_A), getParameterValue(PARAMETER_B), getParameterValue(PARAMETER_C));

    // setParameterValue(PARAMETER_AA, getParameterValue(PARAMETER_A));
    // setParameterValue(PARAMETER_AB, getParameterValue(PARAMETER_B));
    // setParameterValue(PARAMETER_AC, getParameterValue(PARAMETER_C));
    // setParameterValue(PARAMETER_AD, getParameterValue(PARAMETER_D));

    // setParameterValue(PARAMETER_BA, getParameterValue(PARAMETER_A));
    // setParameterValue(PARAMETER_BB, getParameterValue(PARAMETER_B));
    // setParameterValue(PARAMETER_BC, getParameterValue(PARAMETER_C));
    // setParameterValue(PARAMETER_BD, getParameterValue(PARAMETER_D));

    // setParameterValue(PARAMETER_F, isButtonPressed(BUTTON_A) ? 0 : 0.99);
    // setParameterValue(PARAMETER_G, isButtonPressed(BUTTON_B) ? 0 : 0.99);

    // buffer.getSamples(0).setAll(0.5);
    // buffer.getSamples(1).setAll(-0.5);
    a = getParameterValue(PARAMETER_A);
    b = getParameterValue(PARAMETER_B);
    sig1->update(a);
    sig2->update(b);
    // sig1->update(getParameterValue(PARAMETER_A));
    // sig2->update(getParameterValue(PARAMETER_A));
    sig1->generate(buffer.getSamples(0));
    sig2->generate(buffer.getSamples(1));
    // points->write(b);
    // FloatArray samples = buffer.getSamples(1);
    // float inc = 1.0f/samples.getSize();
    // float mu = points->getWriteIndex() - 4;
    // for(int i=0; i<samples.getSize(); ++i){
    //   samples[i] = points->readAt(mu);
    //   mu += inc;
    // }
  }
};

#endif   // __TestPatch_hpp__
