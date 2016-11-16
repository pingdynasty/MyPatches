#ifndef __QuadPanningPatch_hpp__
#define __QuadPanningPatch_hpp__

#include "Patch.h"
#include "SmoothValue.h"
#include "Dbap.hpp"

// #define POSITION_CARTESIAN

class QuadPanningPatch : public Patch {
public:
  class Position {
  public:
    float x, y;
  };
  static constexpr unsigned int numSources = 4;
  static constexpr unsigned int numSpeakers = 4;
  static constexpr unsigned int SEND_INTERVAL = 0; // set parameters every X blocks
  float lambda;
  float lastB, lastC;
  Position sources[numSources];
  SmoothFloat outputValues[numSources][numSpeakers];
  // float outputValues[numSources][numSpeakers];
  PatchParameterId outputParameters[numSources][numSpeakers] = {
    { PARAMETER_AA, PARAMETER_AB, PARAMETER_AC, PARAMETER_AD},
    { PARAMETER_AE, PARAMETER_AF, PARAMETER_AG, PARAMETER_AH},
    { PARAMETER_BA, PARAMETER_BB, PARAMETER_BC, PARAMETER_BD},
    { PARAMETER_BE, PARAMETER_BF, PARAMETER_BG, PARAMETER_BH},
  };
  Dbap* dbap;
  uint32_t controlledSources;
  FloatArray gains;
  QuadPanningPatch()
    : controlledSources(0) {    
    dbap = Dbap::create(numSources, numSpeakers);
    dbap->homeSources();
    gains = FloatArray::create(numSpeakers);
    registerParameter(PARAMETER_A, "Source");
#ifdef POSITION_CARTESIAN
    registerParameter(PARAMETER_B, "X");
    registerParameter(PARAMETER_C, "Y");
#else
    registerParameter(PARAMETER_B, "Angle");
    registerParameter(PARAMETER_C, "Distance");
#endif
    registerParameter(PARAMETER_D, "Spread");
    registerParameter(PARAMETER_E, "Slew");
    lastB = getParameterValue(PARAMETER_B);
    lastC = getParameterValue(PARAMETER_C);
  }

  ~QuadPanningPatch(){
    Dbap::destroy(dbap);
  }
  
  void processAudio(AudioBuffer &buffer){
    lambda = getParameterValue(PARAMETER_E)*0.299+0.7;
#ifdef POSITION_CARTESIAN
    float b = getParameterValue(PARAMETER_B)*1.998-0.999;
    float c = getParameterValue(PARAMETER_C)*1.998-0.999;
#else
    // float b = 360 * (0.75 - getParameterValue(PARAMETER_B));
    float b = 360 * getParameterValue(PARAMETER_B);
    float c = getParameterValue(PARAMETER_C);
#endif
    float spread = 2 * getParameterValue(PARAMETER_D);
    static int ticker = 0;
    if(ticker++ == SEND_INTERVAL){
      updateControlledSources(getParameterValue(PARAMETER_A));
      // if(isButtonPressed(PUSHBUTTON)){
      	moveSources(b, c, spread);
      // }else{
	// moveSourcesRelative(b - lastB, c - lastC, spread);
      // }
      lastB = b;
      lastC = c;
      ticker = 0;
    }
  }

  void setOutputParameters(unsigned int source, FloatArray gains){
    if(source >= 3)
      return; // skipping outputs BE to BH (todo: remove!)
    for(unsigned int speaker = 0; speaker < numSpeakers; ++speaker){
      outputValues[source][speaker].lambda = lambda;
      // if(abs(outputValues[source][speaker] - gains[speaker]) > 0.01){
      outputValues[source][speaker] = gains[speaker];
      setParameterValue(outputParameters[source][speaker], outputValues[source][speaker]);
      // }
    }
  }

  void moveSourcesRelative(float b, float c, float spread){
    for(unsigned int n = 0; n < numSources; ++n){
      if(isSourceControlled(n)){
	DbapSource* src = dbap->getSource(n);
#ifdef POSITION_CARTESIAN
	b = src->getX() + b;
	c = src->getY() + c;
        dbap->setSourcePosition(n, b, c);
#else
	b = src->getAngle() + b;
	c = src->getDistance() + c;
        dbap->setSourcePolarPosition(n, b, c);
#endif
        dbap->setSourceSpread(n, spread);
	dbap->getAmplitudes(n, gains);
	setOutputParameters(n, gains);
      }
    }
  }

  void moveSources(float b, float c, float spread){
    for(unsigned int n = 0; n < numSources; ++n){
      if(isSourceControlled(n)){
#ifdef POSITION_CARTESIAN
        dbap->setSourcePosition(n, b, c);
#else
        dbap->setSourcePolarPosition(n, b, c);
#endif
        dbap->setSourceSpread(n, spread);
	dbap->getAmplitudes(n, gains);
	setOutputParameters(n, gains);
      }
    }
  }

  void updateControlledSources(float par){
    static float previousPar = -1;
    const int numStates = numSources+3;
    if(fabsf(par - previousPar) > 0.005){
      previousPar = par;
      unsigned int state = (int)(par * numStates);
      resetControlledSources();
      switch(state){
      case numStates - 3: // controls the first half	
        for(unsigned int n = 0; n < numSources / 2; ++n)
          controlSource(n);
        debugMessage("Controlling lower half");
      break;
      case numStates - 2: // controls the second half	
        for(unsigned int n =  numSources / 2; n < numSources; ++n)
          controlSource(n);
        debugMessage("Controlling upper half");
      break;
      case numStates - 1: // controls all sources	
        for(unsigned int n =  0; n < numSources; ++n)
          controlSource(n);
        debugMessage("Controlling all");
	break;
      default:
      // most states would control the sources with the
      // corresponding number 
        controlSource(state);
        debugMessage("Controlling", (int)state);
      }
    }
  }

  void resetControlledSources(){
    controlledSources = 0;
  }

  void controlSource(unsigned int newSource){
    controlledSources |= (1 << newSource);
  }

  bool isSourceControlled(unsigned int source){
    return controlledSources & (1 << source);
  }
};

#endif // __QuadPanningPatch_hpp__
