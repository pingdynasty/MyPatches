#ifndef __LorenzFMPatch_hpp__
#define __LorenzFMPatch_hpp__

#include "Patch.h"
#include "CircularBuffer.hpp"
#include "SineOscillator.h"
#include "BiquadFilter.h"

/*
 * The Lorenz attractor is a set of chaotic solutions of the Lorenz system.
 * http://en.wikipedia.org/wiki/Lorenz_system
 */
class LorenzFMPatch : public Patch {
public:
  float x, y, z;
  float dt;
  const float a = 10.0;
  const float b = 8.0/3.0;
  const float c = 28.0;
  SmoothFloat freq;
  SmoothFloat index;
  StereoBiquadFilter* lowpass;

  size_t memory = 512;
  float P[3];
  float Pprime[3];
  float R[3][3];
  CircularBuffer* xpos;
  CircularBuffer* ypos;
  const size_t MAX_MEMORY_SIZE = 16*1024;

  SineOscillator xosc, yosc;
  FloatArray xbuf, ybuf;
  SineOscillator lfo2;

  LorenzFMPatch(): xosc(getSampleRate()), yosc(getSampleRate()),
		   lfo2(getSampleRate()/getBlockSize())
  {
    registerParameter(PARAMETER_A, "Rate");
    setParameterValue(PARAMETER_A, 0.1);
    registerParameter(PARAMETER_AA, "Depth");
    setParameterValue(PARAMETER_AA, 0.1);

    registerParameter(PARAMETER_C, "Rotate X");
    registerParameter(PARAMETER_D, "Rotate Y");
    registerParameter(PARAMETER_AC, "Rotate Z");
    registerParameter(PARAMETER_AD, "Zoom");
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0.5);
    setParameterValue(PARAMETER_AC, 0.5);
    setParameterValue(PARAMETER_AD, 0.2);

    registerParameter(PARAMETER_E, "LFO1 Speed");
    registerParameter(PARAMETER_F, "LFO2 Speed");
    registerParameter(PARAMETER_AE, "LFO1>");
    registerParameter(PARAMETER_AF, "LFO2>");
    setParameterValue(PARAMETER_E, 0.5);
    setParameterValue(PARAMETER_F, 0.5);

    registerParameter(PARAMETER_G, "Freq");
    registerParameter(PARAMETER_H, "FM");
    registerParameter(PARAMETER_AG, "X>");
    registerParameter(PARAMETER_AH, "Y>");
    setParameterValue(PARAMETER_G, 0.2);
    setParameterValue(PARAMETER_H, 0.1);

    xpos = CircularBuffer::create(MAX_MEMORY_SIZE);
    ypos = CircularBuffer::create(MAX_MEMORY_SIZE);

    xbuf = FloatArray::create(getBlockSize());
    ybuf = FloatArray::create(getBlockSize());

    lowpass = StereoBiquadFilter::create(1);
    lowpass->setLowPass(8000/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);

    reset();
  }
  ~LorenzFMPatch(){
    CircularBuffer::destroy(xpos);
    CircularBuffer::destroy(ypos);
    FloatArray::destroy(xbuf);
    FloatArray::destroy(ybuf);
    StereoBiquadFilter::destroy(lowpass);
  }

  void reset(){
    x = 0.1;
    y = 0.0;
    z = 0.0;
  }

  void processAttractor(FloatArray xbuffer, FloatArray ybuffer){
    dt = getParameterValue(PARAMETER_A)*getParameterValue(PARAMETER_A)*0.00250;
    float rotateX = getParameterValue(PARAMETER_C)*M_PI;
    float rotateY = getParameterValue(PARAMETER_D)*M_PI;
    float rotateZ = getParameterValue(PARAMETER_AC)*M_PI;
    float gainL, gainR;
    gainL = gainR = getParameterValue(PARAMETER_AD)*2/25.0;
    float dx, dy, dz;
    updateMatrix(rotateX, rotateY, rotateZ);
    int size = getBlockSize();
    for(int i=0;i<size;i++){
      dx = a*(y - x);
      dy = (x * (c - z) - y);
      dz = (x*y - b * z);
      x += dx*dt;
      y += dy*dt;
      z += dz*dt;
      P[0] = x;
      P[1] = y;
      P[2] = z-25; // centre on z axis
      rotateP();
      xbuffer[i] = Pprime[0] * gainL;
      ybuffer[i] = Pprime[1] * gainR;
      if((i & 0x7) == 0){
	xpos->write(xbuffer[i]);
	ypos->write(ybuffer[i]);
      }
    }
    // debugMessage("x/y/z", x, y, z);
    memory = getParameterValue(PARAMETER_AA)*MAX_MEMORY_SIZE;
  }
  
  void processAudio(AudioBuffer &buffer){
    if(isButtonPressed(PUSHBUTTON))
      reset();

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    freq = getParameterValue(PARAMETER_G)*1000+20;
    xosc.setFrequency(freq);
    yosc.setFrequency(freq);
    processAttractor(xbuf, ybuf);

    setParameterValue(PARAMETER_AG, xbuf[0]*12.5 + .5);
    setParameterValue(PARAMETER_AH, ybuf[0]*12.5 + .5);

    index = getParameterValue(PARAMETER_H)*0.1;
    xbuf.multiply(index);
    ybuf.multiply(index);
    xosc.getSamples(left, xbuf);
    yosc.getSamples(right, ybuf);    

    
    static float lfo1 = 0;
    if(lfo1 > 1.0){
      lfo1 = 0;
    }
    float tempo = getParameterValue(PARAMETER_E)+0.01;
    lfo1 += tempo * getBlockSize() / getSampleRate();
    setParameterValue(PARAMETER_AE, lfo1);
    tempo = getParameterValue(PARAMETER_F)*2;
    lfo2.setFrequency(tempo);
    setParameterValue(PARAMETER_AF, lfo2.getNextSample()*0.5+0.5);

    lowpass->process(buffer);
}
  
  void updateMatrix(float rotateX, float rotateY, float rotateZ){
  // rotation around the x axis
    float alpha = rotateX;
  // rotation around the y axis
    float beta = rotateY;
  // rotation around the z axis
    float gamma = rotateZ;
    float sa = sinf(alpha);
    float sb = sinf(beta);
    float sg = sinf(gamma);
    float ca = cosf(alpha);
    float cb = cosf(beta);
    float cg = cosf(gamma);

    // compute rotation matrix.
    // We rotate about x, then y, then z
    R[0][0] = cb * cg;
    R[0][1] = cg * sa * sb - ca * sg;
    R[0][2] = ca * cg * sb + sa * sg;
    
    R[1][0] = cb * sg;
    R[1][1] = ca * cg + sa * sb * sg;
    R[1][2] = -cg * sa + ca * sb * sg;
   
    R[2][0] = -sb;
    R[2][1] = cb * sa;
    R[2][2] = ca * cb;
  }
  
  void rotateP(){
   // apply rotation matrix 
    for(int n = 0; n < 3; ++n){
      float value = 0;
      for(int k = 0; k < 3; ++k){
        value += R[n][k] * P[k];
      }
      Pprime[n] = value;
    }
  }

#ifdef OWL_MAGUS
  void processScreen(ScreenBuffer& screen){
    screen.clear();
    int w = screen.getWidth();
    int h = screen.getHeight();
    for(int i=0; i<memory; ++i){
      int x = xpos->read(i) * w + w/2;
      int y = ypos->read(i) * h + h/2;
      screen.setPixel(x, y, WHITE);
    }
  }
#endif
};

#endif // __LorenzFMPatch_hpp__
