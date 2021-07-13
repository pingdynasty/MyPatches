
// template<class BaseOscillator>
// class BlepTemplate {
// // ref: http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/
// protected:
//   static float blep(float incr, float t){
//     float dt = incr / (2*M_PI);
//     if(t < dt){
//       // 0 <= t < 1
//       t /= dt;
//       return t+t - t*t - 1.0;
//     }else if(t > 1.0 - dt){
//       // -1 < t < 0
//       t = (t - 1.0) / dt;
//       return t*t + t+t + 1.0;
//     }
//     // 0 otherwise
//     return 0.0;
//   }    
// public:
//   float generate(){
//     float sample = BaseOscillator::generate();
//     return sample - blep(incr, phase); // for saw only
//   }
// };


float constexpr sqrtfNewtonRaphson(float x, float curr, float prev){
  return curr == prev ? curr : sqrtfNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}
/**
 * Constexpr aproximation of the square root
 * Only valid for finite and non-negative values of 'x'
 */
float constexpr constsqrtf(float x){
  return sqrtfNewtonRaphson(x, x, 0);
}

class WitchPatch : public Patch {
protected:
  SynthVoices* voices;
  WitchLFO* lfo1;
  WitchLFO* lfo2;
  CvNoteProcessor* cvnote;
  WitchMultiEffect* fx;
  static constexpr float cvrange = 5;
public:
  WitchPatch(){
    // lfo
    lfo1 = WitchLFO::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo2 = WitchLFO::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo1->select(WitchLFO::SINE);
    lfo1->setBeatsPerMinute(60);
    lfo2->select(WitchLFO::AGNESI);
    lfo2->setBeatsPerMinute(120);
#ifdef USE_MPE
    // send MPE Configuration Message RPN
    sendMidi(MidiMessage::cc(0, 100, 5));
    sendMidi(MidiMessage::cc(0, 101, 0));
    sendMidi(MidiMessage::cc(0, 6, VOICES));
#endif
    fx = WitchMultiEffect::create(getSampleRate(), getBlockSize());
    fx->setBeatsPerMinute(60);

    registerParameter(PARAMETER_FX_AMOUNT, "FX Amount");
    // setParameterValue(PARAMETER_FX_AMOUNT, 0.0);
    registerParameter(PARAMETER_EXTL_AMOUNT, "Ext L Amount");
    setParameterValue(PARAMETER_EXTL_AMOUNT, 0.2);
    registerParameter(PARAMETER_EXTR_AMOUNT, "Ext R Amount");
    setParameterValue(PARAMETER_EXTR_AMOUNT, 0.2);

    registerParameter(PARAMETER_ATTACK, "Attack");
    registerParameter(PARAMETER_DECAY, "Decay");
    registerParameter(PARAMETER_SUSTAIN, "Sustain");
    registerParameter(PARAMETER_RELEASE, "Release");
    registerParameter(PARAMETER_FX_SELECT, "FX Select");

    // sendMidi(MidiMessage::cc(0, OpenWareMidiControl::PATCH_PARAMETER_AA, 0));
    // sendMidi(MidiMessage::cc(0, OpenWareMidiControl::PATCH_PARAMETER_AB, 0));
    // sendMidi(MidiMessage::cc(0, OpenWareMidiControl::PATCH_PARAMETER_AC, 127));
    // sendMidi(MidiMessage::cc(0, OpenWareMidiControl::PATCH_PARAMETER_AD, 0));
  }
  virtual ~WitchPatch(){
    WitchLFO::destroy(lfo1);
    WitchLFO::destroy(lfo2);
    WitchMultiEffect::destroy(fx);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      cvnote->gate(value, samples);
      break;
    case BUTTON_2:
      lfo1->trigger(value, samples);
      if(value)
	lfo1->reset();
      fx->trigger(value, samples);
      break;
    case BUTTON_3:
      lfo2->trigger(value, samples);
      if(value)
	lfo2->reset();
      break;
    case BUTTON_4:
#if VOICES > 1
      static bool sustain = false;
      if(value){
	sustain = !sustain; // toggle
	voices->setSustain(sustain);
	// voices->sustain(MidiMessage::cc(0, MIDI_CC_SUSTAIN, sustain ? 127 : 0));
	if(!sustain)
	  voices->allNotesOff();
      }
      setButton(BUTTON_4, sustain);
#endif
      break;
    }
  }

  static float attenuvertion(float value){
    value = 4 * value/128.0f - 2;
    return value < 0 ? -value*value : value*value;
  }    
  void processMidi(MidiMessage msg){
    voices->process(msg);
    if(msg.isControlChange()){
      uint8_t value = msg.getControllerValue();
      switch(msg.getControllerNumber()){
      case PATCH_PARAMETER_ATTENUATE_A:
	setParameterValue(PARAMETER_BA, attenuvertion(value));
	break;
      case PATCH_PARAMETER_ATTENUATE_B:
	setParameterValue(PARAMETER_BB, attenuvertion(value));
	break;
      case PATCH_PARAMETER_ATTENUATE_C:
	setParameterValue(PARAMETER_BC, attenuvertion(value));
	break;
      case PATCH_PARAMETER_ATTENUATE_D:
	setParameterValue(PARAMETER_BD, attenuvertion(value));
	break;
      case PATCH_PARAMETER_LFO1_SHAPE:
	lfo1->select(value/128.0f);
	break;
      case PATCH_PARAMETER_LFO2_SHAPE:
	lfo2->select(value/128.0f);
	break;
      case PARAMETER_DYNAMIC_RANGE:
	voices->setDynamicRange(value);
	break;
      case PATCH_BUTTON_ON:
	if(value > 3 && value < 8)
	  setButton((PatchButtonId)value, true);
	break;
      case PATCH_BUTTON_OFF:
	if(value > 3 && value < 8)
	  setButton((PatchButtonId)value, false);
	break;
      }
    }
  }    

  void doprocess(AudioBuffer &buffer){
    cvnote->clock(getBlockSize());
    cvnote->cv(getParameterValue(PARAMETER_A));
    float amount = getParameterValue(PARAMETER_EXTL_AMOUNT);
    amount *= amount;
    buffer.getSamples(LEFT_CHANNEL).multiply(amount);
    amount = getParameterValue(PARAMETER_EXTR_AMOUNT);
    amount *= amount;
    buffer.getSamples(RIGHT_CHANNEL).multiply(amount);
  }
  
  void dofx(AudioBuffer &buffer){
    fx->select(getParameterValue(PARAMETER_FX_SELECT));
    fx->setEffect(getParameterValue(PARAMETER_FX_AMOUNT));
    fx->process(buffer, buffer);
  }

  void dolfo(){
    // lfo
    lfo1->clock(getBlockSize());
    lfo2->clock(getBlockSize());
    float lfo = lfo1->generate()*0.5+0.5;
    fx->setModulation(lfo);
    setParameterValue(PARAMETER_F, lfo);
    bool gate = lfo1->getPhase() < M_PI;
    setButton(BUTTON_B, gate);
    setButton(BUTTON_E, gate);
    lfo = lfo2->generate()*0.5+0.5;
    setParameterValue(PARAMETER_G, lfo);
    gate = lfo2->getPhase() < M_PI;
    setButton(BUTTON_C, gate);
    setButton(BUTTON_F, gate);
  }
};
