#ifndef _Dbap_h_
#define _Dbap_h_
//#include "Patch.h"
#include "FloatArray.h"
#include <math.h>
#include <stdio.h>

class DbapSource{
public:
  DbapSource()
    : DbapSource(6) {}

  DbapSource(float rolloff)
    : spread(0),    
      ys(0),
      rSquared(0),
      a(rolloff / (20 * log10f(2))) {}

  void setSpread(float newSpread){
    rSquared = max(0.001, newSpread * newSpread); // avoid div by zero caused by zero distance
  }
  void setPosition(float newX, float newY){
    xs = newX;
    ys = newY;
  }
  void setPolarPosition(float angle, float r){
    xs = r * cosf(angle / 360 * 2*M_PI);
    ys = r * sinf(angle / 360 * 2*M_PI);
  }
  float getX(){
    return xs;
  }
  float getY(){
    return ys;
  }
  float getSpread(){
    return spread;
  }
  float getAngle(){
    return ys == 0 ? 0 : atanf(xs/ys)*360/(2*M_PI);
  }
  float getDistance(){
    return sqrtf(xs*xs+ys*ys);
  }

  void getAmplitudes(FloatArray outputs, FloatArray speakersX, FloatArray speakersY){
    // Assumes all the FloatArrays have the same size.
    float sum = 0;
    for(unsigned int n = 0; n < outputs.getSize(); ++n){
      float xi = speakersX[n];
      float yi = speakersY[n];
      // use output as temporary storage
      // for distances d_i
      float xDiff = xi - xs;
      float yDiff = yi - ys;
      outputs[n] = sqrtf(xDiff * xDiff + yDiff * yDiff + rSquared);
      sum += 1 / powf(outputs[n], 2 * a);
    }
    // k = \frac{1}{sqrt{sum{1}{N} \frac{1}{d_i^(2a)}}}
    float k = 1 / sqrtf(sum);
    for(unsigned int n = 0; n < outputs.getSize(); ++n){
      // v_i = \frac{k}{d_i^a}
      outputs[n] = k / powf(outputs[n], a);
    }
  }
private:
  float spread;
  float xs;
  float ys;
  float rSquared;
  float a;
};

class Dbap{
public:
  Dbap() : sources(NULL), numSources(0) {}
  Dbap(DbapSource* src, unsigned int num, FloatArray spkX, FloatArray spkY)
    : sources(src), numSources(num), speakersX(spkX), speakersY(spkY)
  {}

  void setSources(DbapSource* src, unsigned int num){
    sources = src;
    numSources = num;
  }

  void setSpeakers(FloatArray newSpeakersX, FloatArray newSpeakersY){
    // Argument check: pick the smallest of the two sizes to
    // ensure they have the same size.
    unsigned int size = min(speakersX.getSize(), speakersY.getSize());
    speakersX = FloatArray(newSpeakersX.getData(), size);
    speakersY = FloatArray(newSpeakersY.getData(), size);
  }

  void setSpeakerPosition(int speaker, float x, float y){
    if(speaker < speakersX.getSize())
      speakersX[speaker] = x;
    if(speaker < speakersY.getSize())
      speakersY[speaker] = y;
  }
  

  DbapSource* getSource(unsigned int source){
    if(source < numSources)
      return &sources[source];
  }

  void setSourcePosition(unsigned int source, float x, float y){
    if(source < numSources)
      sources[source].setPosition(x, y);
  }

  void setSourcePolarPosition(unsigned int source, float angle, float r){
    if(source < numSources)
      sources[source].setPolarPosition(angle, r);
  }

  void setSourceSpread(unsigned int source, float spread){
    if(source < numSources)
      sources[source].setSpread(spread);	
  }

  void getAmplitudes(unsigned int source, FloatArray output){
    // TODO: check that output.getSize() <= speakersX.getSize()
    if(source < numSources){
      sources[source].getAmplitudes(output, speakersX, speakersY);
    }
  }

  void homeSources(){
    for(unsigned int n = 0; n < numSources; ++n){
      sources[n].setPosition(0, 0);
    }
  }

  static Dbap* create(unsigned int sources, unsigned int speakers){
    // creates  DBAP configuration with the given number of
    // speakers evenly spaced on the unity circle
    FloatArray x = FloatArray::create(speakers);
    FloatArray y = FloatArray::create(speakers);
    DbapSource* src = new DbapSource[sources];
    float angle = 360/sources;
    for(int i=0; i<sources; ++i)
      src->setPolarPosition(angle*i, 1.0f);
    angle = (2*M_PI)/speakers;
    for(int i=0; i<speakers; ++i){
      x[i] = cosf(angle*i+angle/2);
      y[i] = sinf(angle*i+angle/2);
    }
    Dbap* dbap = new Dbap(src, sources, x, y);
    return dbap;
  }

  static Dbap* create(unsigned int sources){
    // create a default 4-way DBAP configuration
    FloatArray x = FloatArray::create(4);
    FloatArray y = FloatArray::create(4);
    x[0] = -1.0; y[0] = -1.0;
    x[1] = -1.0; y[1] = 1.0;
    x[2] = 1.0; y[2] = 1.0;
    x[3] = 1.0; y[3] = -1.0;
    DbapSource* src = new DbapSource[sources];
    Dbap* dbap = new Dbap(src, sources, x, y);
    return dbap;
  }

  static void destroy(Dbap* dbap){
    FloatArray::destroy(dbap->speakersX);
    FloatArray::destroy(dbap->speakersY);
    delete dbap->sources;
    delete dbap;
  }
private:
  DbapSource* sources;
  unsigned int numSources;
  FloatArray speakersX;
  FloatArray speakersY;
};
#endif /*  _Dbap_h_ */
