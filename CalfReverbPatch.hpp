#ifndef __CalfReverbPatch_hpp__
#define __CalfReverbPatch_hpp__

#include "Patch.h"

/**********************************************************************
 * Adapted from Calf Studio Gear
 * http://calf.sourceforge.net
 **********************************************************************/

/**********************************************************************
 * REVERB by Krzysztof Foltman
**********************************************************************/


#define FAKE_INFINITY (65536.0 * 65536.0)
#define IS_FAKE_INFINITY(value) (fabs(value-FAKE_INFINITY) < 1.0)
#include "calfprimitives.h"
#include <complex>
#include "calf/src/calf/fixed_point.h"
#include "calf/src/calf/delay.h"

namespace dsp {

/**
 * A classic allpass loop reverb with modulated allpass filter.
 * Just started implementing it, so there is no control over many
 * parameters.
 */
  class reverb //: public audio_effect
{
    simple_delay<2048, float> apL1, apL2, apL3, apL4, apL5, apL6;
    simple_delay<2048, float> apR1, apR2, apR3, apR4, apR5, apR6;
    fixed_point<uint32_t, 25> phase, dphase;
    sine_table<int32_t, 128, 10000> sine;
    onepole<float> lp_left, lp_right;
    float old_left, old_right;
    int type;
    float time, fb, cutoff, diffusion;
    int tl[6], tr[6];
    float ldec[6], rdec[6];
    int sr;
public:
    reverb()
    {
        phase = 0.0;
        time = 1.0;
        cutoff = 9000;
        type = 2;
        diffusion = 1.f;
        setup(48000);
    }
    virtual void setup(int sample_rate) {
        sr = sample_rate;
        set_time(time);
        set_cutoff(cutoff);
        phase = 0.0;
        dphase = 0.5*128/sr;
        update_times();
    }
    void update_times();
    float get_time() const {
        return time;
    }
    void set_time(float time) {
        this->time = time;
        // fb = pow(1.0f/4096.0f, (float)(1700/(time*sr)));
        fb = 1.0 - 0.3 / (time * sr / 48000.0);
    }
    int get_type() const {
        return type;
    }
    void set_type(int type) {
        this->type = type;
        update_times();
    }
    float get_diffusion() const {
        return diffusion;
    }
    void set_diffusion(float diffusion) {
        this->diffusion = diffusion;
        update_times();
    }
    void set_type_and_diffusion(int type, float diffusion) {
        this->type = type;
        this->diffusion = diffusion;
        update_times();
    }
    float get_fb() const
    {
        return this->fb;
    }
    void set_fb(float fb)
    {
        this->fb = fb;
    }
    float get_cutoff() const {
        return cutoff;
    }
    void set_cutoff(float cutoff) {
        this->cutoff = cutoff;
        lp_left.set_lp(cutoff,sr);
        lp_right.set_lp(cutoff,sr);
    }
    void reset();
    void process(float &left, float &right);
    void extra_sanitize()
    {
        lp_left.sanitize();
        lp_right.sanitize();
    }
};

void reverb::update_times()
{
    switch(type)
    {
    case 0:
        tl[0] =  397 << 16, tr[0] =  383 << 16;
        tl[1] =  457 << 16, tr[1] =  429 << 16;
        tl[2] =  549 << 16, tr[2] =  631 << 16;
        tl[3] =  649 << 16, tr[3] =  756 << 16;
        tl[4] =  773 << 16, tr[4] =  803 << 16;
        tl[5] =  877 << 16, tr[5] =  901 << 16;
        break;
    case 1:
        tl[0] =  697 << 16, tr[0] =  783 << 16;
        tl[1] =  957 << 16, tr[1] =  929 << 16;
        tl[2] =  649 << 16, tr[2] =  531 << 16;
        tl[3] = 1049 << 16, tr[3] = 1177 << 16;
        tl[4] =  473 << 16, tr[4] =  501 << 16;
        tl[5] =  587 << 16, tr[5] =  681 << 16;
        break;
    case 2:
    default:
        tl[0] =  697 << 16, tr[0] =  783 << 16;
        tl[1] =  957 << 16, tr[1] =  929 << 16;
        tl[2] =  649 << 16, tr[2] =  531 << 16;
        tl[3] = 1249 << 16, tr[3] = 1377 << 16;
        tl[4] = 1573 << 16, tr[4] = 1671 << 16;
        tl[5] = 1877 << 16, tr[5] = 1781 << 16;
        break;
    case 3:
        tl[0] = 1097 << 16, tr[0] = 1087 << 16;
        tl[1] = 1057 << 16, tr[1] = 1031 << 16;
        tl[2] = 1049 << 16, tr[2] = 1039 << 16;
        tl[3] = 1083 << 16, tr[3] = 1055 << 16;
        tl[4] = 1075 << 16, tr[4] = 1099 << 16;
        tl[5] = 1003 << 16, tr[5] = 1073 << 16;
        break;
    case 4:
        tl[0] =  197 << 16, tr[0] =  133 << 16;
        tl[1] =  357 << 16, tr[1] =  229 << 16;
        tl[2] =  549 << 16, tr[2] =  431 << 16;
        tl[3] =  949 << 16, tr[3] = 1277 << 16;
        tl[4] = 1173 << 16, tr[4] = 1671 << 16;
        tl[5] = 1477 << 16, tr[5] = 1881 << 16;
        break;
    case 5:
        tl[0] =  197 << 16, tr[0] =  133 << 16;
        tl[1] =  257 << 16, tr[1] =  179 << 16;
        tl[2] =  549 << 16, tr[2] =  431 << 16;
        tl[3] =  619 << 16, tr[3] =  497 << 16;
        tl[4] = 1173 << 16, tr[4] = 1371 << 16;
        tl[5] = 1577 << 16, tr[5] = 1881 << 16;
        break;
    }

    float fDec=1000 + 2400.f * diffusion;
    for (int i = 0 ; i < 6; i++) {
        ldec[i]=exp(-float(tl[i] >> 16) / fDec),
        rdec[i]=exp(-float(tr[i] >> 16) / fDec);
    }
}

void reverb::reset()
{
    apL1.reset();apR1.reset();
    apL2.reset();apR2.reset();
    apL3.reset();apR3.reset();
    apL4.reset();apR4.reset();
    apL5.reset();apR5.reset();
    apL6.reset();apR6.reset();
    lp_left.reset();lp_right.reset();
    old_left = 0; old_right = 0;
}

void reverb::process(float &left, float &right)
{
    unsigned int ipart = phase.ipart();

    // the interpolated LFO might be an overkill here
    // int lfo = phase.lerp_by_fract_int<int32_t, 14, int32_t>(sine.data[ipart], sine.data[ipart+1]) >> 2;
    int lfo = sine.data[ipart]>>2;
    phase += dphase;

    left += old_right;
    left = apL1.process_allpass_comb_lerp16(left, tl[0] - 45*lfo, ldec[0]);
    left = apL2.process_allpass_comb_lerp16(left, tl[1] + 47*lfo, ldec[1]);
    float out_left = left;
    left = apL3.process_allpass_comb_lerp16(left, tl[2] + 54*lfo, ldec[2]);
    left = apL4.process_allpass_comb_lerp16(left, tl[3] - 69*lfo, ldec[3]);
    left = apL5.process_allpass_comb_lerp16(left, tl[4] + 69*lfo, ldec[4]);
    left = apL6.process_allpass_comb_lerp16(left, tl[5] - 46*lfo, ldec[5]);
    old_left = lp_left.process(left * fb);
    sanitize(old_left);

    right += old_left;
    right = apR1.process_allpass_comb_lerp16(right, tr[0] - 45*lfo, rdec[0]);
    right = apR2.process_allpass_comb_lerp16(right, tr[1] + 47*lfo, rdec[1]);
    float out_right = right;
    right = apR3.process_allpass_comb_lerp16(right, tr[2] + 54*lfo, rdec[2]);
    right = apR4.process_allpass_comb_lerp16(right, tr[3] - 69*lfo, rdec[3]);
    right = apR5.process_allpass_comb_lerp16(right, tr[4] + 69*lfo, rdec[4]);
    right = apR6.process_allpass_comb_lerp16(right, tr[5] - 46*lfo, rdec[5]);
    old_right = lp_right.process(right * fb);
    sanitize(old_right);

    left = out_left, right = out_right;
}

};

class CalfReverbPatch : public Patch {
private:
  dsp::reverb reverb;
  int type;
  bool toggling;
public:
  CalfReverbPatch(){
    reverb.setup(getSampleRate());
    registerParameter(PARAMETER_A, "Time");
    registerParameter(PARAMETER_B, "Cutoff");
    registerParameter(PARAMETER_C, "Diffusion");
    registerParameter(PARAMETER_D, "Dry/Wet");
    type = reverb.get_type();
  }
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON && value){
      if(type++ > 5)
	type = 0;
      debugMessage("Room", type);
    }
  }

  void processAudio(AudioBuffer &buffer){
    // float time = getParameterValue(PARAMETER_A)*5;
    float decay = getParameterValue(PARAMETER_A)*14.6 + 0.4;
    float cutoff = getParameterValue(PARAMETER_B)*19000+1000;
    float diffusion = getParameterValue(PARAMETER_C);
    float wet = getParameterValue(PARAMETER_D);

    reverb.set_time(decay);
    reverb.set_cutoff(cutoff);
    reverb.set_type_and_diffusion(type, diffusion);

    int size = buffer.getSize();
    float* lins = buffer.getSamples(LEFT_CHANNEL);
    float* rins = buffer.getSamples(RIGHT_CHANNEL);
    float leftAC, rightAC;
    for(int i=0; i<size; ++i){
      leftAC = lins[i];
      rightAC = rins[i];
      reverb.process(leftAC, rightAC);
      lins[i] = leftAC * wet + lins[i] * (1-wet);
      rins[i] = rightAC * wet + rins[i] * (1-wet);
    } // cycle through samples
  }
};

#endif // __CalfReverbPatch_hpp__
