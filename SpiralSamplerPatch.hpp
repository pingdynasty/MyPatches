#ifndef __SpiralSamplerPatch_hpp__
#define __SpiralSamplerPatch_hpp__

#include "OpenWareLibrary.h"
#include "TapTempo.hpp"

static const int TRIGGER_LIMIT = (1<<17);

#define SAMPLE_NAME "sample1.wav"
#define POLYPHONY 8
#define BUTTON_VELOCITY 100

// class InterpolatedFloatArray : public FloatArray {
// cosine, cubic, Catmull-Rom, Hermite interpolation algorithms
// ref: http://paulbourke.net/miscellaneous/interpolation/
  static float LinearInterpolate(float y1, float y2, float mu){
    return y1 + (y2 - y1) * mu;
  }
  static float CosineInterpolate(float y1, float y2, float mu){
    float mu2 = (1-cosf(mu*M_PI))/2;
    return y1*(1-mu2)+y2*mu2;
  }
  /**
   * Get the interpolated value of a curve described by y0 to y4,
   * somewhere between the points y1 and y2.
   */
  float CubicInterpolate(float y0, float y1, float y2, float y3, float mu) {
    float mu2 = mu*mu;
    float a0 = y3 - y2 - y0 + y1;
    float a1 = y0 - y1 - a0;
    float a2 = y2 - y0;
    float a3 = y1;
    return a0*mu*mu2+a1*mu2+a2*mu+a3;
  }
  static float SmoothCubicInterpolate(float y0, float y1, float y2, float y3, float mu) {
    // use Catmull-Rom splines:
    // take the slope between the previous point and the next as the derivative at the current point
   float mu2 = mu*mu;
   float a0 = -0.5*y0 + 1.5*y1 - 1.5*y2 + 0.5*y3;
   float a1 = y0 - 2.5*y1 + 2*y2 - 0.5*y3;
   float a2 = -0.5*y0 + 0.5*y2;
   float a3 = y1;
   return a0*mu*mu2+a1*mu2+a2*mu+a3;
  }
  /*
    Tension: 1 is high, 0 normal, -1 is low
    Bias: 0 is even,
         positive is towards first segment,
         negative towards the other
  */
  static float HermiteInterpolate(float y0, float y1, float y2, float y3, float mu, float tension, float bias){
    float mu2 = mu * mu;
    float mu3 = mu2 * mu;
    float m0  = (y1-y0)*(1+bias)*(1-tension)/2;
    m0 += (y2-y1)*(1-bias)*(1-tension)/2;
    float m1  = (y2-y1)*(1+bias)*(1-tension)/2;
    m1 += (y3-y2)*(1-bias)*(1-tension)/2;
    float a0 =  2*mu3 - 3*mu2 + 1;
    float a1 =    mu3 - 2*mu2 + mu;
    float a2 =    mu3 -   mu2;
    float a3 = -2*mu3 + 3*mu2;
   return(a0*y1+a1*m0+a2*m1+a3*y2);
  }
//   float get(float index){
//     // for cubic/hermite interpolation
//     // add two extra points at the start and end of the sequence, the new points are created so that they have a slope equal to the slope of the start or end segment.    
//   }
// };

// three-point cubic interpolation of point between y1 and y2
// ref: http://www.ebyte.it/library/codesnippets/P3Interpolation.html
static float CubicInterpolate(float y0, float y1, float y2, float mu){
  float d1 = 0.5*(y2-y0);
  float d2 = y2-y1+y0-y1;
  float dx = mu;
  return y1 + dx*(d1+0.5*d2*dx);
}

class SamplerVoice : public AbstractSynth {
private:
  LinearSampleOscillator* osc;
  LinearAdsrEnvelope* env;
  float gain;
  Control<PARAMETER_D> attack_release = 0.5f;
  Control<PARAMETER_AB> decay = 0.0f;
  Control<PARAMETER_AC> sustain = 1.0f;
  // Control<PARAMETER_AE> portamento = 0.0f;
  Control<PARAMETER_E> duration = 1.0f;
  BiquadFilter* filter;
public:
  SamplerVoice(float sr, FloatArray buf)
    : gain(1.0f) {
    osc = LinearSampleOscillator::create(sr, buf);
    osc->setRepeatMode(LinearSampleOscillator::REPEAT_REVERSE);
    env = LinearAdsrEnvelope::create(sr);
    filter = BiquadFilter::create(sr, 2);
  }
  static SamplerVoice* create(float sr) {
    return new SamplerVoice(sr, FloatArray());
  }
  static SamplerVoice* create(float sr, FloatArray buf) {
    return new SamplerVoice(sr, buf);
  }
  static void destroy(SamplerVoice* obj) {
    LinearSampleOscillator::destroy(obj->osc);
    LinearAdsrEnvelope::destroy(obj->env);
    BiquadFilter::destroy(obj->filter);
    delete obj;
  }
  void setSample(FloatArray sample){
    osc->setSample(sample);
  }
  void setFrequency(float freq) {
    osc->setFrequency(freq);
    freq *= 30;
    freq = min(18000.0f, freq);
    // filter->setLowPass(freq, FilterStage::BUTTERWORTH_Q);
  }
  void setEnvelope() {
    float df = attack_release*3;
    constexpr float tmin = 0.002;
    int di = (int)df;
    df = df - di;
    float attack, release;
    switch(di){
      // a/d
    case 0: // l/s
      attack = 1- df + tmin;
      release = tmin;
      break;
    case 1: // s/s
      attack = tmin;
      release = df + tmin;
      break;
    case 2: // s/l
      attack = df + tmin;
      release = 1.0 + tmin;
      break;
      // l/l
    }
    env->setAttack(attack);
    env->setDecay(decay);
    env->setSustain(sustain);
    env->setRelease(release);
  }
  void setGain(float value) {
    gain = value;
  }
  void gate(bool state) {
    env->gate(state);
    if(state)
      osc->reset();
  }
  void trigger() {
    osc->reset();
    env->trigger();
  }
  float generate() {
    setEnvelope();
    osc->setDuration(duration);
    // 'duration' or trim should trim from start clockwise and end anti-clockwise
    float y = osc->generate();
    y *= env->generate();
    y = filter->process(y); // filter before or after envelope?
    y *= gain;
    return y;
  }
  void generate(FloatArray samples) {
    setEnvelope();
    osc->setDuration(duration);
    osc->generate(samples);
    env->process(samples, samples);
    filter->process(samples, samples);
    samples.multiply(gain);
  }
  float filter_fc = 8000;
  float filter_q = 4;
  float filter_gain = 0;
  virtual void setParameter(uint8_t parameter_id, float value){
    switch(parameter_id){
    case 1:
      filter_fc = value;
      filter->setPeak(filter_fc, filter_q, filter_gain);
      break;
    case 2:
      filter_q = value;
      break;
    case 3:
      filter_gain = value;
      break;
    }
  }
};


class SpiralSamplerPatch : public Patch {
private:
  typedef VoiceAllocatorSignalGenerator<PolyphonicProcessor<SamplerVoice, POLYPHONY>, SamplerVoice, POLYPHONY> Voices;
  Voices* voices;
  int basenote = 60;

  SineOscillator* lfo1;
  AgnesiOscillator* lfo2;
  PatchTapTempo<TRIGGER_LIMIT> tempo1;
  PatchTapTempo<TRIGGER_LIMIT> tempo2;
public:

  SpiralSamplerPatch() :
    tempo1(getSampleRate()*0.5), tempo2(getSampleRate()*0.25) {
    registerParameter(PARAMETER_A, "Frequency");
    setParameterValue(PARAMETER_A, 0.8);
    registerParameter(PARAMETER_AB, "Decay");
    registerParameter(PARAMETER_AC, "Sustain");
    //registerParameter(PARAMETER_AE, "Portamento");
    registerParameter(PARAMETER_AF, "Duration");

    registerParameter(PARAMETER_B, "Cutoff");
    setParameterValue(PARAMETER_B, 0.5);
    registerParameter(PARAMETER_C, "Peak");
    setParameterValue(PARAMETER_C, 0.5);

    // load sample
    Resource* resource = getResource(SAMPLE_NAME);
    WavFile wav(resource->getData(), resource->getSize());
    if(!wav.isValid())
      error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
    FloatArray sample = wav.createFloatArray(0);
    Resource::destroy(resource);
    
    // filter sample
    // BiquadFilter* filter = BiquadFilter::create(getSampleRate(), 2);
    // // filter->setHighPass();
    // filter->setLowPass(6000, FilterStage::BUTTERWORTH_Q);
    // filter->process(sample, sample);
    // BiquadFilter::destroy(filter);
    DcBlockingFilter* filter = DcBlockingFilter::create();
    filter->process(sample, sample);
    DcBlockingFilter::destroy(filter);

    // create voices
    voices = Voices::create(getBlockSize());
    for(size_t i=0; i<POLYPHONY; ++i)
      voices->setVoice(i, SamplerVoice::create(getSampleRate(), sample));
    
      // lfo
    lfo1 = SineOscillator::create(getBlockRate());
    // lfo2 = SineOscillator::create(getBlockRate());
    lfo2 = AgnesiOscillator::create(getBlockRate(), 0.5, 10);
    registerParameter(PARAMETER_F, "LFO1>");
    registerParameter(PARAMETER_G, "LFO2>");
  }

  ~SpiralSamplerPatch() {
    SineOscillator::destroy(lfo1);
    // SineOscillator::destroy(lfo2);
    AgnesiOscillator::destroy(lfo2);
    for(int i=0; i<POLYPHONY; ++i)
      SamplerVoice::destroy(voices->getVoice(i));
    Voices::destroy(voices);
  }
    
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    int note = 0;
    static int lastnote[4];
    switch(bid){
    case BUTTON_1:
      if(value){
	note = basenote;
	lastnote[0] = note;
	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(0, lastnote[0], 0));
      }
      break;
    case BUTTON_2:
      if(value){
	note = basenote+3;
	lastnote[1] = note;
	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
      }else{
	voices->noteOff(MidiMessage::note(0, lastnote[1], 0));
      }
      break;
    case BUTTON_3:
      tempo1.trigger(value, samples);
      if(value)
	lfo1->reset();
      // note = basenote+7;
      break;
    case BUTTON_4:
      tempo2.trigger(value, samples);
      if(value)
	lfo2->reset(); // todo: instead of hard reset, calculate to sync on next edge
      // note = basenote+12;
      break;
    // case BUTTON_3:
    //   if(value){
    // 	note = basenote+7;
    // 	lastnote[2] = note;
    // 	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
    //   }else{
    // 	voices->noteOff(MidiMessage::note(0, lastnote[2], 0));
    //   }
    //   break;
    // case BUTTON_4:
    //   if(value){
    // 	note = basenote+12;
    // 	lastnote[3] = note;
    // 	voices->noteOn(MidiMessage::note(0, note, BUTTON_VELOCITY));
    //   }else{
    // 	voices->noteOff(MidiMessage::note(0, lastnote[3], 0));
    //   }
    //   break;
    }
  }

  void processMidi(MidiMessage msg){
    voices->process(msg);
  }    

  void processAudio(AudioBuffer& buffer) {
    basenote = getParameterValue(PARAMETER_A)*48+40;

    float cutoff = getParameterValue(PARAMETER_B);
    cutoff = cutoff*cutoff*12000+400;
    float gain = getParameterValue(PARAMETER_C)*24-12;
    voices->setParameter(3, gain);
    voices->setParameter(1, cutoff);
    
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    voices->generate(left);
    left.tanh();
    right.copyFrom(left);

    // lfo
    tempo1.clock(getBlockSize());
    tempo2.clock(getBlockSize());
    float rate = getSampleRate()/tempo1.getSamples();
    lfo1->setFrequency(rate);
    setParameterValue(PARAMETER_F, lfo1->generate()*0.5+0.5);
    setButton(BUTTON_E, lfo1->getPhase() < M_PI);
    rate = getSampleRate()/tempo2.getSamples();
    lfo2->setFrequency(rate);
    // setParameterValue(PARAMETER_G, lfo2->generate()*0.5+0.5);
    setParameterValue(PARAMETER_G, lfo2->generate());
    setButton(BUTTON_F, lfo2->getPhase() < M_PI);
}
};

#endif // __SpiralSamplerPatch_hpp__
