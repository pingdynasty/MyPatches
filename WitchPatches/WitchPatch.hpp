
class BusMessage {
protected:
  uint8_t data[4];
public:
 BusMessage(PatchParameterId pid, float value){
    data[0] = OWL_COMMAND_PARAMETER;
    data[1] = pid;
    int16_t ival = value*8192;
    data[2] = ival >> 8;
    data[3] = ival & 0xff;
  }
  BusMessage(uint8_t ch, OpenWareMidiControl cc, uint8_t value){
    data[0] = USB_COMMAND_CONTROL_CHANGE;
    data[1] = CONTROL_CHANGE | (ch & MIDI_CHANNEL_MASK);
    data[2] = cc;
    data[3] = value;
  }
  bool isControlChange(){
    return data[0] == USB_COMMAND_CONTROL_CHANGE && (data[1] & MIDI_STATUS_MASK) == CONTROL_CHANGE;
  }
  bool isParameter(){
    return data[0] == OWL_COMMAND_PARAMETER;
  }
  PatchParameterId getParameterId(){
    return (PatchParameterId)data[1];
  }
  float getParameterValue(){
    return (((int16_t)data[2] << 8) | data[3]) / 8192.0f;
  }
  MidiMessage getMidiMessage(){
    return MidiMessage(data[0], data[1], data[2], data[3]);
  }
};

typedef SimpleArray<BusMessage> BusMessageArray;

static const BusMessage cfg_default[] = {
   BusMessage(PARAMETER_WAVESHAPE, 0), // todo: should be 0.8 for WaveBank Stereo Mix
   BusMessage(PARAMETER_EXTL_AMOUNT, 0.2f),
   BusMessage(PARAMETER_EXTR_AMOUNT, 0.2f),
   BusMessage(PARAMETER_ATTACK, 0),
   BusMessage(PARAMETER_DECAY, 0),
   BusMessage(PARAMETER_SUSTAIN, 0.99f),
   BusMessage(PARAMETER_RELEASE, 0),
   BusMessage(0, PATCH_PARAMETER_BA, 97),
   BusMessage(0, PATCH_PARAMETER_BB, 97),
   BusMessage(0, PATCH_PARAMETER_BC, 97),
   BusMessage(0, PATCH_PARAMETER_BD, 97),
   BusMessage(0, PATCH_PARAMETER_LFO1_SHAPE, 32),
   BusMessage(0, PATCH_PARAMETER_LFO2_SHAPE, 0),
   BusMessage(0, PARAMETER_DYNAMIC_RANGE, 72),
   BusMessage(0, PARAMETER_FX_SELECT, FX_SELECT)
   // todo: dynamic range RPN
   // todo: pitch bend range RPN
   // todo: coarse and fine tuning RPN
};

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

class AbstractWitchPatch : public Patch {
public:
  void restore(){
    Resource* resource = Resource::load(PATCHNAME ".cfg");
    BusMessageArray cfg;
    if(resource){
      cfg = BusMessageArray((BusMessage*)resource->getData(), resource->getSize()/sizeof(BusMessage));
    }else{
      cfg = BusMessageArray((BusMessage*)cfg_default, sizeof(cfg_default)/sizeof(BusMessage));
    }
    for(int i=0; i<cfg.getSize(); ++i)
      processBusMessage(cfg[i]);
    if(resource)
      Resource::destroy(resource);
#ifdef USE_MPE
    // send MPE Configuration Message RPN
    sendMidi(MidiMessage::cc(0, 100, 5));
    sendMidi(MidiMessage::cc(0, 101, 0));
    sendMidi(MidiMessage::cc(0, 6, VOICES));
#endif
  }
  void processBusMessage(BusMessage msg){
    if(msg.isParameter()){
      setParameterValue(msg.getParameterId(), msg.getParameterValue());
      uint8_t pid = msg.getParameterId();
      if(pid >= PARAMETER_AA && pid <= PARAMETER_DH)
	pid = pid-PARAMETER_AA+PATCH_PARAMETER_AA;
      else if(pid == PARAMETER_H)
	pid = PATCH_PARAMETER_H;
      else
	return;
      sendMidi(MidiMessage::cc(0, pid, msg.getParameterValue()*127));
    }else if(msg.isControlChange()){
      processMidi(msg.getMidiMessage());
      sendMidi(msg.getMidiMessage());
    }
  }
  static float attenuvertion(float value){
    value = 4 * value/128.0f - 2;
    return value < 0 ? -value*value : value*value;
  }
  void processMidi(MidiMessage msg){
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
};

class WitchPatch : public AbstractWitchPatch {
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
    lfo1->setBeatsPerMinute(60);
    lfo2->setBeatsPerMinute(120);
    fx = WitchMultiEffect::create(getSampleRate(), getBlockSize());
    fx->setBeatsPerMinute(60);

    registerParameter(PARAMETER_FX_AMOUNT, "FX Amount");
    registerParameter(PARAMETER_EXTL_AMOUNT, "Ext L Amount");
    registerParameter(PARAMETER_EXTR_AMOUNT, "Ext R Amount");

    registerParameter(PARAMETER_ATTACK, "Attack");
    registerParameter(PARAMETER_DECAY, "Decay");
    registerParameter(PARAMETER_SUSTAIN, "Sustain");
    registerParameter(PARAMETER_RELEASE, "Release");
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
    default:
      break;
    }
  }

  void processMidi(MidiMessage msg){
    voices->process(msg);
    if(msg.isControlChange()){
      uint8_t value = msg.getControllerValue();
      switch(msg.getControllerNumber()){
      case PATCH_PARAMETER_LFO1_SHAPE:
	lfo1->select(value/128.0f);
	break;
      case PATCH_PARAMETER_LFO2_SHAPE:
	lfo2->select(value/128.0f);
	break;
      case PARAMETER_DYNAMIC_RANGE:
	voices->setDynamicRange(value);
	break;
      case PARAMETER_FX_SELECT:
	fx->select(value/128.0f);
	break;
      default:
	AbstractWitchPatch::processMidi(msg);
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
