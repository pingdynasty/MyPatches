#ifndef __LorenzAttractorPatch_hpp__
#define __LorenzAttractorPatch_hpp__

#include "MonochromeScreenPatch.h"
#include "CircularBuffer.hpp"
#include "SineOscillator.h"
#include "BiquadFilter.h"
#include "SmoothValue.h"

/*
 * The Lorenz attractor is a set of chaotic solutions of the Lorenz system.
 * http://en.wikipedia.org/wiki/Lorenz_system
 */
class LorenzAttractorPatch : public MonochromeScreenPatch {
public:
  float x, y, z;
  float dt;
  const float a = 10.0;
  const float b = 8.0/3.0;
  const float c = 28.0;
  SmoothFloat freqL, freqR;

  float amplitude = 0.8;
  SmoothFloat index;
  StereoBiquadFilter* lowpass;

  size_t memory = 512;
  float P[3];
  float Pprime[3];
  float R[3][3];
  CircularBuffer* xpos;
  CircularBuffer* ypos;
  const size_t MAX_MEMORY_SIZE = 16*1024;

  SineOscillator* xosc;
  SineOscillator* yosc;
  FloatArray xbuf, ybuf;

  LorenzAttractorPatch() :
    xosc(SineOscillator::create(getSampleRate())),
    yosc(SineOscillator::create(getSampleRate()))
  {
    registerParameter(PARAMETER_A, "Freq L");
    registerParameter(PARAMETER_B, "Freq R");
    registerParameter(PARAMETER_C, "Rotate X");
    registerParameter(PARAMETER_D, "Rotate Y");
    registerParameter(PARAMETER_E, "Rotate Z");
    registerParameter(PARAMETER_F, "Zoom");
    registerParameter(PARAMETER_G, "Rate");
    registerParameter(PARAMETER_H, "FM Amount");
    registerParameter(PARAMETER_AA, "Depth");
    setParameterValue(PARAMETER_A, 0.1);
    setParameterValue(PARAMETER_B, 0.1);
    setParameterValue(PARAMETER_C, 0.5);
    setParameterValue(PARAMETER_D, 0.5);
    setParameterValue(PARAMETER_E, 0.5);
    setParameterValue(PARAMETER_F, 0.2);
    setParameterValue(PARAMETER_G, 0.15);
    setParameterValue(PARAMETER_H, 0.1);
    setParameterValue(PARAMETER_AA, 0.2);
    registerParameter(PARAMETER_AB, "X>");
    registerParameter(PARAMETER_AC, "Y>");
    registerParameter(PARAMETER_AD, "Z>");

    xpos = CircularBuffer::create(MAX_MEMORY_SIZE);
    ypos = CircularBuffer::create(MAX_MEMORY_SIZE);

    xbuf = FloatArray::create(getBlockSize());
    ybuf = FloatArray::create(getBlockSize());

    lowpass = StereoBiquadFilter::create(getSampleRate(), 2);
    lowpass->setLowPass(8000, FilterStage::BUTTERWORTH_Q);

    reset();
  }
  ~LorenzAttractorPatch(){
    CircularBuffer::destroy(xpos);
    CircularBuffer::destroy(ypos);
    FloatArray::destroy(xbuf);
    FloatArray::destroy(ybuf);
    StereoBiquadFilter::destroy(lowpass);
    SineOscillator::destroy(xosc);
    SineOscillator::destroy(yosc);
  }

  void reset(){
    x = 0.1;
    y = 0.0;
    z = 0.0;
  }

  void processAttractor(FloatArray xbuffer, FloatArray ybuffer){
    float rotateX = getParameterValue(PARAMETER_C)*M_PI;
    float rotateY = getParameterValue(PARAMETER_D)*M_PI;
    float rotateZ = getParameterValue(PARAMETER_E)*M_PI;
    float zoom = getParameterValue(PARAMETER_F)*2/25.0;
    float rate = getParameterValue(PARAMETER_G);
    float dx, dy, dz;
    dt = rate*rate*0.00125;
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
      xbuffer[i] = Pprime[0] * zoom;
      ybuffer[i] = Pprime[1] * zoom;
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

    freqL = getParameterValue(PARAMETER_A)*1705+55;
    freqR = getParameterValue(PARAMETER_B)*1705+55;
    xosc->setFrequency(freqL);
    yosc->setFrequency(freqR);

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);

    // setParameterValue(PARAMETER_AG, xbuf[0]*12.5 + .5);
    // setParameterValue(PARAMETER_AH, ybuf[0]*12.5 + .5);
    setParameterValue(PARAMETER_AB, clamp(x/50 + 0.5f, 0.0f, 1.0f));
    setParameterValue(PARAMETER_AC, clamp(y/50 + 0.5f, 0.0f, 1.0f));
    setParameterValue(PARAMETER_AD, clamp(z/50, 0.0f, 1.0f));

    index = getParameterValue(PARAMETER_H)*2;
    left.multiply(0.5);
    right.multiply(0.5);
    xbuf.add(left);
    ybuf.add(right);
    xbuf.multiply(index);
    ybuf.multiply(index);
    xosc->generate(left, xbuf);
    yosc->generate(right, ybuf);    

    processAttractor(xbuf, ybuf);

    lowpass->process(buffer, buffer);
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

  void processScreen(MonochromeScreenBuffer& screen){
    // screen.clear();
    int w = screen.getWidth();
    int h = screen.getHeight();
    for(int i=0; i<memory; ++i){
      int x = xpos->read(i) * w + w/2;
      int y = ypos->read(i) * h + h/2;
      screen.setPixel(x, y, WHITE);
    }
  }
};

#endif // __LorenzAttractorPatch_hpp__
