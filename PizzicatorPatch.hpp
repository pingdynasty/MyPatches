class PizzicatorPatch : public Patch {
private:
  const float VOLTAGE_MULTIPLIER = -4.40f;
  const float VOLTAGE_OFFSET = -0.0585f;

  /*
    A: pitch
    B: pitch pattern: scale + range + sequence + number of unique notes
    C: number of and distribution of beats
    D: tempo
    Left in: pitch trigger
    Right in: amplitude trigger
    Left out: pitch trigger
    Right out: amplitude trigger
  */
public:
  const float maxincr;
  const float MAXTEMPO = 480; // 360;
  PizzicatorPatch() :
    maxincr((1<<24)/getSampleRate()/(MAXTEMPO/60)) {
    registerParameter(PARAMETER_A, "Pitch");
    registerParameter(PARAMETER_B, "Notes");
    registerParameter(PARAMETER_C, "Beats");
    registerParameter(PARAMETER_D, "Tempo");
    
  }

  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }
  float volts2sample(float s){
    return s / VOLTAGE_MULTIPLIER + VOLTAGE_OFFSET;
  }

  int patterns[5][7] = { { 0, 1, 2, 3, 4, 5, 6 },
			 { 0, 2, 4, 5, 7, 9, 11 },
			 { 0, 1, 3, 5, 6, 8, 10 },
			 { 0, 2, 3, 5, 7, 8, 10 },
			 { 0, 2, 4, 5, 7, 9, 10 } };
  bool playing = false;
  float basepitch;
  float multiplier;
  int pos = 0;
  int incr = 0;
  const int maxpos = 7<<24;

  float cent = 0.83; // One cent at 1V/Octave is 0.83 mV
  float semitone = cent*100;

  void processAudio(AudioBuffer& buffer){
    float pitch = getParameterValue(PARAMETER_A)*2-1;
    int pattern = getParameterValue(PARAMETER_B)*5;
    incr = (int)(getParameterValue(PARAMETER_D)*maxincr);
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    for(int i=0; i<size; ++i){
      float v = sample2volts(left[i]);
      float t = sample2volts(right[i]);
      if(playing && t < 1.0){
	playing = false;
      }else if(!playing && t > 1.0){
	playing = true;
	pos = 0;
      }
      int index = pos>>24;
      if(playing){
	//	debugMessage("pos/incr/index", pos, incr, index);
	int tone = patterns[pattern][index];
	left[i] = volts2sample(v + tone*semitone + pitch);
	if(pos & (1<<23) == 0)
	  right[i] = 1;
	else
	  right[i] = -1;
	pos += incr;
	if(pos > maxpos)
	  pos = 0;
      }else{
	left[i] = 0;
	right[i] = 0;
      }
    }
  }
};
