
#define STEREO_SAMPLER_BUFFER_SIZE 1024

class StereoSamplerPatch : public Patch {
private:
  const float VOLTAGE_MULTIPLIER = -4.40f;
  const float VOLTAGE_OFFSET = -0.0585f;
  bool recording = false;
  bool playing = false;
  const float NOISEGATE = 0.01;
  float gain = 0;
  float gm = 0;
  int recpos = 0;
  int readpos = 0;
  FloatArray samples[2];

  /*
    left in: pitch trigger
    right in: amplitude trigger
    A: l pitch
    B: r pitch
    C: l amplitude
    D: r amplitude
    hold button to sample left/right inputs
  */
public:
  StereoSamplerPatch() {
    registerParameter(PARAMETER_A, "Left Pitch");
    registerParameter(PARAMETER_B, "Right Pitch");
    registerParameter(PARAMETER_C, "Left Amplitude");
    registerParameter(PARAMETER_D, "Right Amplitude");
    samples[0] = FloatArray::create(64*1024);
    //    samples[1] = FloatArray::create(8192);
  }

  float sample2volts(float s){
    return (s-VOLTAGE_OFFSET) * VOLTAGE_MULTIPLIER;
  }

  void processAudio(AudioBuffer& buffer){

    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    //FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();

    if(recording){
      if(!isButtonPressed(PUSHBUTTON)){
	// todo: apply window
	recording = false;
	readpos = 0;
	debugMessage("finished sampling");
      }else{
	int len = min(samples[0].getSize()-recpos, size);
	samples[0].insert(left, recpos, 0, len);
	recpos += len;
      }
    }else{
      if(isButtonPressed(PUSHBUTTON)){
	int i = 1;
	// fast forward to zero crossing
	if(left[0] > 0)
	  while(i < size && left[i++] > 0);
	else
	  while(i < size && left[i++] < 0);
	// fast forward to non-zero samples
	while(i < size && abs(left[i++]) < NOISEGATE);
	if(i < size){
	  recording = true;
	  int len = min(samples[0].getSize(), size);
	  samples[0].copyFrom(left, len);
	  recpos = len;
	}
      }
    }
    if(!recording){
      for(int i=0; i<size; ++i){
	if(!playing){
	  if(sample2volts(left[i]) > 1.0){
	    playing = true;
	    readpos = 0;
	    gm = 2;
	    gain = 0.1;
	  }
	}
	if(playing){
	  if(readpos < recpos)
	    left[i] = gain*samples[0][readpos++];
	  else
	    left[i] = 0;
	  if(sample2volts(left[i]) < 1.0)
	    gm = 0.8;
	  gain *= gm;
	  if(gain > 1){
	    gain = 1;
	    gm = 1;
	  }else if(gain < 0){
	    gain = 0;
	    gm = 0;
	    playing = false;
	    debugMessage("finished playing");
	  }
	}
      }
    }
  }
};
