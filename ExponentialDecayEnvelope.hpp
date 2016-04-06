#ifndef __ExponentialDecayEnvelopePatch_hpp__
#define __ExponentialDecayEnvelopePatch_hpp__

class ExponentialDecayEnvelope {
private:
  const float fs;
  float value;
  float incr;
public:
  ExponentialDecayEnvelope(float sr)
    : fs(sr){}
  void setRate(float r){
    if(r < 0)
      incr = 1.0f - 100*(1/(1-r))/fs;
    else
      incr = 1.0f + 100*r/fs;
  }
  void setDecay(float d){
    setRate(-(d+1/fs));
  }
  void trigger(){
    value = 1.0;
  }
  float getNextSample(){
    float sample = value;
    value *= incr;
    return sample;
  }
};

#endif   // __ExponentialDecayEnvelopePatch_hpp__
