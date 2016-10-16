#ifndef __QuadPanningPatch_hpp__
#define __QuadPanningPatch_hpp__

#include "Patch.h"
#include "Dbap.hpp"

class QuadPanningPatch : public Patch {
public:
  static constexpr unsigned int numSources = 4;
  static constexpr unsigned int numSpeakers = 4;
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
    dbap = Dbap::create(numSources);
    dbap->homeSources();
    gains = FloatArray::create(numSpeakers);
    registerParameter(PARAMETER_A, "Source");
    registerParameter(PARAMETER_B, "Angle");
    registerParameter(PARAMETER_C, "Distance");
    registerParameter(PARAMETER_D, "Spread");
  }

  ~QuadPanningPatch(){
    Dbap::destroy(dbap);
  }
  
  void processAudio(AudioBuffer &buffer){
    float angle = 360 * (0.75 - getParameterValue(PARAMETER_B));
    float distance = getParameterValue(PARAMETER_C);
    float spread = 2 * getParameterValue(PARAMETER_D);

    // int selected = getParameterValue(PARAMETER_A)*4;
    // dbap->setSourcePolarPosition(selected, angle, distance);
    // dbap->setSourceSpread(selected, spread);
    // dbap->getAmplitudes(selected, gains);
    // setOutputParameters(selected, gains);

    updateControlledSources(getParameterValue(PARAMETER_A));
    moveSources(angle, distance, spread);
  }

  void setOutputParameters(unsigned int source, FloatArray gains){
    for(unsigned int speaker = 0; speaker < gains.getSize(); ++speaker){
      setParameterValue(outputParameters[source][speaker], gains[speaker]);
    }
  }

  void moveSources(float angle, float distance, float spread){
    for(unsigned int n = 0; n < numSources; ++n){
      if(isSourceControlled(n)){
        dbap->setSourcePolarPosition(n, angle, distance);
        dbap->setSourceSpread(n, spread);
	dbap->getAmplitudes(n, gains);
	setOutputParameters(n, gains);
      }
    }
  }

  void updateControlledSources(float par){
    static float previousPar = -1;
    const int numStates = 6;
    if(fabsf(par - previousPar) > 0.005){
      previousPar = par;
      unsigned int state = (int)(par * numStates);
      resetControlledSources();
      switch(state){
      case numStates - 2:
	// The last but one state controls the first half sources
        for(unsigned int n = 0; n < numSources / 2; ++n){
          controlSource(n);
        }
        debugMessage("Controlling lower half");
      break;
      case numStates - 1:
	// The last but one state controls the second half sources
        for(unsigned int n =  numSources / 2; n < numSources; ++n){
          controlSource(n);
        }
        debugMessage("Controlling upper half");
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
