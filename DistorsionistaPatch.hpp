#ifndef __DistorsionistaPatch_hpp__
#define __DistorsionistaPatch_hpp__

#include "SmoothValue.h"

//#define _LISTEN // for test purposes
#define CYCLES_6 6
#define MPY_FACTOR CYCLES_6 * M_PI
#define QFactor 0.7071f // butterworth
// #define QFactor 0.9565f // Chebychev
#define QFactor_1 0.5412f // Q factors for two cascaded butterworth filters
#define QFactor_2 1.3066f
#define QFactorPeak 2 // for peaking filter
#define PeakGain 9 // gain in db's

// Bi-Quad filter from MusicDsp site
class BiQuad
{
public:
  BiQuad() 
  {
  }
    
private:
  float c, a1, a2, a3, b1, b2, v, n;
  float inputHistory[2] = {0,};
  float outputHistory[3] = {0,};
  
public: 
  void SetLowPass(float freq, float res, float sampleRate)
  {
      c = 1.0 / tan(M_PI * freq / sampleRate);

      a1 = 1.0f / ( 1.0f + res * c + c * c);
      a2 = 2.0f * a1;
      a3 = a1;
      b1 = 2.0f * ( 1.0f - c * c) * a1;
      b2 = ( 1.0f - res * c + c * c) * a1;
  }
  
  void SetHighPass(float freq, float res, float sampleRate)
  {
      c = tan(M_PI * freq / sampleRate);

      a1 = 1.0f / ( 1.0f + res * c + c * c);
      a2 = -2.0f * a1;
      a3 = a1;
      b1 = 2.0f * ( c * c - 1.0f) * a1;
      b2 = ( 1.0f - res * c + c * c) * a1;
  }
  
  // peak filter
  void SetPeak(float freq, float res, float gain, float sampleRate)
  {
      c = tan(M_PI * freq / sampleRate);
      v = powf(10.0f,(gain / 20.0f));

      n = 1.0f / (1.0f + 1.0f / res * c + c * c);
      a1 = (1.0f + v/res * c + c * c) * n;
      a2 = 2.0f * (c * c - 1.0f) * n;
      a3 = (1.0f - v/res * c + c * c) * n;
      b1 = a2;
      b2 = (1.0f - 1.0f/res * c + c * c) * n;
  }
  
  void Update(float newInput)
  {
    float newOutput = a1 * newInput + a2 * inputHistory[0] + a3 
    * inputHistory[1] - b1 * outputHistory[0] - b2 * outputHistory[1];
    
    inputHistory[1] = inputHistory[0];
    inputHistory[0] = newInput;
    
    outputHistory[2] = outputHistory[1];
    outputHistory[1] = outputHistory[0];
    outputHistory[0] = newOutput;
    
  }
  // get filtered data
  float GetOutput(void)
  {
    return outputHistory[0];
  }
};

// DC Blocker algorithm from Stanford University
class DCBlocker
{
public:
    DCBlocker() {};
    
    float xd = 0.0f;
    float yd = 0.0f;
    float y;
    
    // process data (High pass filter)
    float Update(float x)
    {
        y = x - xd + 0.995f * yd;
        xd = x; // save sample data
        yd = y;
        return y;
    }
};


// class based on algorithm from Felix Eichas, Stephan Moller & Udo Zolzer
// see 'Block-orientated modelling of distortion audio effects 
// minimal minimization'

class TunableWaveshaper
{
private:
    float kp, kn, kpt, knt, kps, kns, gp, gn, tn, tp, driven; 
    
    // tanh approximation - save processing bandwidth
    float FastTanh(float x)
    {
      
      if (x < -3.0f) // limit out of range data
        x = -1.0f;
      else if (x > 3.0f)
        x = 1.0f;
      else
        x = x * (27 + x * x) / (27 + 9 * x * x);
      return x;
    }
    
// UpdateParameters - set waveshaper operatng parameters
// dgp = 'knee slope - positive'
// dgn = 'knee slope - negative'
// dkp = 'positive threshold'
// dkn = 'negative threshold'
// dr = drive
public:
    void UpdateParameters(float dgp, float dgn, float dkp, float dkn, float dr)
    {
        gp = dgp;
        gn = dgn;
        kp = dkp;
        kn = dkn;
        driven = dr;
        
        kpt = FastTanh(kp * driven);
	    knt = FastTanh(kn * driven);
	    kps = FastTanh(kp);
	    kps *= kps;
	    kns = FastTanh(kn);
	    kns *= kns;
    }
    
    // main wave-shaper function
    float ShapeData(float x)
    {
      if (x > kp) // test past positive threshold
	  {
			tp = (((kps - 1) / gp) * FastTanh((gp * x) - kp));
			x = kpt - tp;
	  }
	  else if (x < -kn) // test past negative threshold
	  {
			tn = (((kns - 1) / gn) * FastTanh((gn * x) + kn));
			x = -knt -tn;
	  }
	  else
	  {
			x = FastTanh(x * driven); // process audio between limiting points
	  }
	  return x;
    }
    
};

// main processing class
class Distorsionista
{
private:
  float p1, p2, p3;
  float g, g1, g2, g3;
  float l, m, h;
  float fDrive = 0.5f;
  float a;
  float kp,kn;
  bool bSecondMode = true;
  
  BiQuad LowPass; // low frequency band
  BiQuad HighPass; // high frequency band
  BiQuad Filter1; // anti-aliasing filter sections
  BiQuad Filter2;
  BiQuad Filter3; // peaking filter to enhance distortion
  DCBlocker OnePole1; // DC blockers
  DCBlocker OnePole2;
  DCBlocker OnePole3;
  TunableWaveshaper shaper; // wave-shaper 2  
  
public:
  Distorsionista(float sr) 
  {    
    LowPass.SetLowPass(220,QFactor,sr); // low corner frequency
    HighPass.SetHighPass(440,QFactor,sr); // high corner frequency
    Filter1.SetLowPass(8000,QFactor_1,sr); // anti-aliasing pair (1)
    Filter2.SetLowPass(8000,QFactor_2,sr); // anti-aliasing (2)
    Filter3.SetPeak(4500,QFactorPeak,PeakGain,sr);
    shaper.UpdateParameters(50.0f,50.0f,1.0f,1.0f,3.0f); // set initial values
  }
  
  float clip(float in) // general purpose clipping function
  {
     if (in >= 1.0)
        return 0.99999999;
     else if (in < -1.0)
        return -1.0;
     return in;
  }
  
  
  // sin approximation - save processing bandwidth
  float FastSin(float x)
  {
      if (x < 3.14159265f) // test wrap around conditions
      { 
          while (x < 3.14159265f)
            x += 6.28318531f;
      }
      else if (x > 3.14159265f)
      { 
          while (x > 3.14159265f)
            x -= 6.28318531f;
      }
      
      if (x < 0) // approximation algorithm
        x = 1.27323954f * x + 0.405284735f * x * x;
      else
        x = 1.27323954f * x - 0.405284735f * x * x;
        
      return x;
  }
  
  
  void setParameters(float pp1, float pp2, float pp3, float pfDrive)
  {
    p1 = pp1;
    p2 = pp2;
    p3 = pp3;
    fDrive = pfDrive;
    
    if (fDrive >= 0.5f) // see if distortion knob past 12 o'clock
    {
        fDrive -= 0.5f;
        fDrive *= 2.0f;
        bSecondMode = true;
       
        kp = 1.0f - fDrive;
        kp *= 0.85f;
        kp += 0.15f;
        kn = kp;
        
	    shaper.UpdateParameters(70.0f,70.0f,kp,kn,3.0f); // set shaper 2
	    
	    p1 *= 40.0f; // amplify signal levels
        p2 *= 40.0f;
        p3 *= 40.0f;
    }
    else // knob set anti-clockwise
    {
        fDrive *= 2.0f;
        fDrive = 1.0f - fDrive;
        bSecondMode = false;
        
        kp = 1.0f - fDrive;
        kp *= 0.85f;
        kp += 0.15f;
        kn = 0.9f; // asynchronous clipping
        
        shaper.UpdateParameters(60.0f,20.0f,kp,kn,3.0f); // set shaper 2
        
	    p1 *= 30.0f;
        p2 *= 30.0f;
        p3 *= 30.0f;
    }
    
    
}

public:

  float WaveShaper(float x, float intensity)
  {
	  float y = FastSin(MPY_FACTOR * x) / MPY_FACTOR;
	  y *= intensity;
	  y += x;
      return x;
  }
  

  void process(FloatArray buf){
    int samples = buf.getSize();
    for(size_t i = 0; i < samples; i++){
      a = buf[i]; // iterate through the samples
      
      LowPass.Update(a);
      HighPass.Update(a);
   
      l = LowPass.GetOutput();
      h = HighPass.GetOutput();
      m = a - h; // get frequencies below high cut-off
      m = m - l; // subtract frequencies below low cut-off to get mids
     
#ifdef _LISTEN // for testing
      a = l + m  + h; // add each frequency band
      buf[i] = a; // write back to buffer
#else
	
      if (true == bSecondMode) // knob past 12 o'clock - more aggresive distortion sound
	{
	  h = WaveShaper(h, 1.0f); // apply first level of distortion
	  h = OnePole1.Update(h); // remove DC component
	  m = WaveShaper(m, 1.0f);
	  m = OnePole2.Update(m); // remove DC component
	  g1 = shaper.ShapeData(l * p1); // apply second distortion stage
	  g2 = shaper.ShapeData(m * p2);
	  g3 = shaper.ShapeData(h * p3);
	  g = (g1 / 20.0f) + (g2 / 30.0f) + (g3 / 30.0f); // mix frequency bands
	}
      else // first distortion mode
	{
	  h = WaveShaper(h,fDrive * 0.8f); 
	  h = OnePole1.Update(h); // remove DC component
	  m = WaveShaper(m,fDrive * 0.6f);
	  m = OnePole2.Update(m); // remove DC component
	  g1 = shaper.ShapeData(l * p1); // second distortion stage
	  g2 = shaper.ShapeData(m * p2);
	  g3 = shaper.ShapeData(h * p3);
	  g = (g1 / 20.0f) + (g2 / 30.0f) + (g3 / 30.0f); // add each frequency band
	}
	  
      g = OnePole3.Update(g); // remove any DC component
      g = clip(g); // ensure signal cannot go beyond limits
      Filter3.Update(g); // accentuate distortion content using peaking filter
      g = Filter3.GetOutput();
      Filter1.Update(g); // cascaded bi-quads for anti-aliasing (24 db / octave)
      g = Filter1.GetOutput();
      Filter2.Update(g); // cascaded bi-quad 
      g = Filter2.GetOutput(); 
      buf[i] = g; // write the modified samples back
#endif // _LISTEN
     
    }
  }
};

class DistorsionistaPatch : public Patch 
{
private:
  Distorsionista left;
  Distorsionista right;
  SmoothFloat p1, p2, p3, fDrive;
public:
  DistorsionistaPatch(): left(getSampleRate()), right(getSampleRate()) {
    registerParameter(PARAMETER_A, "Drive Low");
    registerParameter(PARAMETER_B, "Drive Mid");
    registerParameter(PARAMETER_C, "Drive High");
    registerParameter(PARAMETER_D, "Distortion");
  }
    
  void processAudio(AudioBuffer &buffer) {
    p1 = getParameterValue(PARAMETER_A); // low freq
    p2 = getParameterValue(PARAMETER_B); // medium freq
    p3 = getParameterValue(PARAMETER_C); // high freq
    fDrive = getParameterValue(PARAMETER_D); // distortion level

    left.setParameters(p1, p2, p3, fDrive);
    right.setParameters(p1, p2, p3, fDrive);

    left.process(buffer.getSamples(0));
    right.process(buffer.getSamples(1));
    setButton(GREEN_BUTTON, clamp((int)(buffer.getSamples(0).getRms()*4096*10), 512, 4096));
    setParameterValue(PARAMETER_F, buffer.getSamples(0).getRms());
    setParameterValue(PARAMETER_G, buffer.getSamples(1).getRms());
  }
};

#endif //__DistorsionistaPatch_hpp__
