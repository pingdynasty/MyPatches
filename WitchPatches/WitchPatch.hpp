
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

class CvNoteProcessor {
protected:
  MidiProcessor* processor;
  size_t cv_delay;
  float note_range;
  float note_offset;
  uint8_t note_channel = 1;
  size_t cv_ticks = 0;
  bool cv_triggered = false;
  bool cv_ison = false;
  uint8_t cv_noteon = NO_NOTE;
  static constexpr uint8_t NO_NOTE = 0xff;
  static constexpr uint8_t CV_VELOCITY = 100;
public:
  CvNoteProcessor(MidiProcessor* processor, size_t delay, float range, float offset)
    : processor(processor), cv_delay(delay), note_range(range), note_offset(offset) {}
  uint8_t getNoteForCv(float cv){
    return round(cv*note_range + note_offset);
  }
  uint8_t getNote(){
    if(cv_noteon == NO_NOTE)
      return 0;
    return cv_noteon;
  }
  void gate(bool ison, size_t delay){
    if(ison != cv_ison && !cv_triggered){ // prevent re-triggers during delay
      cv_ison = ison;
      cv_triggered = true;
      cv_ticks = -delay;
      if(ison)
	cv_noteon = NO_NOTE;
    }
  }
  void cv(float value){
    if(cv_triggered && cv_ticks >= cv_delay){
      cv_triggered = false;
      if(cv_noteon == NO_NOTE){
	cv_noteon = getNoteForCv(value);
	processor->noteOn(MidiMessage::note(note_channel, cv_noteon, CV_VELOCITY));
      }else{
	processor->noteOff(MidiMessage::note(note_channel, cv_noteon, 0));
	cv_noteon = NO_NOTE;
      }
    }
  }
  void clock(size_t ticks){
    if(cv_ticks < cv_delay)
      cv_ticks += ticks;
  }
  static CvNoteProcessor* create(float sr, float delay_milliseconds, MidiProcessor* processor,
				 float range=12*5, float offset=30){
    return new CvNoteProcessor(processor, sr*delay_milliseconds/1000, range, offset);
  }
  static void destroy(CvNoteProcessor* obj){
    delete obj;
  }
};

class TapTempoOscillator : public TapTempo, public SignalGenerator {
protected:
  Oscillator* oscillator;
public:
  TapTempoOscillator(float sr, size_t limit, Oscillator* osc): TapTempo(sr, limit), oscillator(osc) {}
  void reset(){
    oscillator->reset();
  }
  float generate(){
    oscillator->setFrequency(getFrequency());
    return oscillator->generate();
  }
  float getPhase(){
    return oscillator->getPhase();
  }
  void setPhase(float phase){
    oscillator->setPhase(phase);
  }
  void generate(FloatArray output){
    oscillator->setFrequency(getFrequency());
    oscillator->generate(output);
  }
  static TapTempoOscillator* create(float sr, size_t limit, Oscillator* osc){
    return new TapTempoOscillator(sr, limit, osc);
  }
};

class TapTempoSineOscillator : public TapTempoOscillator {
public:
  TapTempoSineOscillator(float sr, size_t limit, Oscillator* osc): TapTempoOscillator(sr, limit, osc) {}
  static TapTempoSineOscillator* create(float sample_rate, size_t limit, float block_rate){
    return new TapTempoSineOscillator(sample_rate, limit, SineOscillator::create(block_rate));
  }
  static void destroy(TapTempoSineOscillator* obj){
    SineOscillator::destroy((SineOscillator*)obj->oscillator);
    delete obj;
  }
};

class TapTempoAgnesiOscillator : public TapTempoOscillator {
public:
  TapTempoAgnesiOscillator(float sr, size_t limit, Oscillator* osc): TapTempoOscillator(sr, limit, osc) {}
  static TapTempoAgnesiOscillator* create(float sample_rate, size_t limit, float block_rate){
    return new TapTempoAgnesiOscillator(sample_rate, limit, AgnesiOscillator::create(block_rate));
  }
  static void destroy(TapTempoAgnesiOscillator* obj){
    AgnesiOscillator::destroy((AgnesiOscillator*)obj->oscillator);
    delete obj;
  }
};

class WitchPatch : public Patch {
protected:
  SynthVoices* voices;
  TapTempoSineOscillator* lfo1;
  TapTempoAgnesiOscillator* lfo2;
  CvNoteProcessor* cvnote;
  WitchMultiEffect* fx;
  static constexpr float cvrange = 5;
public:
  WitchPatch(){
    // lfo
    lfo1 = TapTempoSineOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo2 = TapTempoAgnesiOscillator::create(getSampleRate(), TRIGGER_LIMIT, getBlockRate());
    lfo1->setBeatsPerMinute(60);
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
    setParameterValue(PARAMETER_FX_AMOUNT, 0.0);
    registerParameter(PARAMETER_FM_AMOUNT, "FM Amount");
    setParameterValue(PARAMETER_FM_AMOUNT, 0.5);

    registerParameter(PARAMETER_ATTACK, "Attack");
    registerParameter(PARAMETER_DECAY, "Decay");
    registerParameter(PARAMETER_SUSTAIN, "Sustain");
    registerParameter(PARAMETER_RELEASE, "Release");
    registerParameter(PARAMETER_FX_SELECT, "FX Select");
  }
  virtual ~WitchPatch(){
    TapTempoSineOscillator::destroy(lfo1);
    TapTempoAgnesiOscillator::destroy(lfo2);
    WitchMultiEffect::destroy(fx);
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    switch(bid){
    case BUTTON_1:
      cvnote->gate(value, samples);
      break;
    case BUTTON_2:
      lfo1->trigger(value, samples);
      fx->trigger(value, samples);
      if(value)
	lfo1->reset();
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
	if(!sustain)
	  voices->allNotesOff();
      }
      setButton(BUTTON_4, sustain);
#endif
      break;
    }
  }

  void processMidi(MidiMessage msg){
    voices->process(msg);
  }

  void doprocess(AudioBuffer &buffer){
    cvnote->clock(getBlockSize());
    cvnote->cv(getParameterValue(PARAMETER_A));
    float fm_amount = getParameterValue(PARAMETER_FM_AMOUNT)*0.2;
    buffer.multiply(fm_amount);
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
    setParameterValue(PARAMETER_F, lfo*0.86+0.02);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    setParameterValue(PARAMETER_G, lfo2->generate()*0.86+0.02);
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
  }
};
