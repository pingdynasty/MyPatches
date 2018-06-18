#ifndef __BeatCounterPatch_hpp__
#define __BeatCounterPatch_hpp__

#include "Patch.h"
#include "SerialBuffer.hpp"

/**
 * Beat detector / counter patch based on Teragon Audio Beat Counter
 * https://github.com/teragonaudio/BeatCounter
 */
// Effectively downsamples input from 48kHz -> 12kHz
static const long kDownsampleFactor = 4;
static const float kSilenceThreshold = 0.1;
static const float kMinimumTempo = 60.0;
static const float kMaximumTempo = 180.0;
static const float kDefaultTempo = 120.0;
static const float kHostTempoLinkToleranceInBpm = 16.0;

static const int kParamToleranceMinValue = 1;
static const int kParamToleranceMaxValue = 100;
static const int kParamToleranceDefaultValue = 75;
static const float kParamPeriodMinValue = 1.0f;
static const float kParamPeriodMaxValue = 10.0f;
static const float kParamPeriodDefaultValue = 2.0f;
static const float kParamFilterMinValue = 50.0f;
static const float kParamFilterMaxValue = 500.0f;
static const float kParamFilterDefaultValue = 450.0f;

class BeatCounterPatch : public Patch {
private:
public:
  BeatCounterPatch() {
    tolerance = getIntParameter("Tolerance", kParamToleranceMinValue, kParamToleranceMaxValue, kParamToleranceDefaultValue);
    period = getFloatParameter("Period", kParamPeriodMinValue, kParamPeriodMaxValue, kParamPeriodDefaultValue);
    filterEnabled = true;
    filterFrequency = getFloatParameter("Filter Frequency", kParamFilterMinValue, kParamFilterMaxValue, kParamFilterDefaultValue); // todo: exp range
    useHostTempo = false;    

    minimumAllowedBpm = kMinimumTempo;
    maximumAllowedBpm = kMaximumTempo;
    cooldownPeriodInSamples = (unsigned long)(getSampleRate() * (60.0f / (float)maximumAllowedBpm));
    samplesToSkip = kDownsampleFactor;
    filterOutput = 0.0;
    filterConstant = 0.0;
    clearBpmHistory();
  }

  void beatCallback(float bpm, float runningBpm){
    static bool isOn;
    setButton(PUSHBUTTON, isOn?4095:0);
    isOn = !isOn;
    debugMessage("BPM/runningBpm/available", bpm, runningBpm, (float)bpmHistory.available());
  }

  // Cached parameters
  IntParameter tolerance;
  FloatParameter period;
  bool filterEnabled;
  FloatParameter filterFrequency;
  bool useHostTempo;

  // Variables used by the lopass filter
  float filterOutput;
  float filterConstant;

  // Used to calculate the running BPM
  // std::vector<float> bpmHistory;
  SerialBuffer<float, 2048> bpmHistory;
  
  // Running BPM shown in GUI
  float runningBpm;
  // State of the processing algorithm, will be true if the current sample is part of the beat
  bool currentlyInsideBeat;
  // Highest known amplitude found since initialization (or reset)
  float highestAmplitude;
  // Highest known amplitude found within a period
  float highestAmplitudeInPeriod;
  // Running average of the number of samples found between beats. Used to calculate the actual BPM.
  float beatLengthRunningAverage;
  // Used to calculate the BPM in combination with beatLengthRunningAverage
  unsigned long numSamplesSinceLastBeat;
  // Smallest possible BPM allowed, can improve accuracy if input source known to be within a given BPM range
  float minimumAllowedBpm;
  // Largest possible BPM allowed, can improve accuracy if input source known to be within a given BPM range
  float maximumAllowedBpm;
  // Poor man's downsampling
  unsigned long samplesToSkip;
  // Used to calculate the period, which in turn effects the total accuracy of the algorithm
  unsigned long numSamplesProcessed;
  // Wait at least this many samples before another beat can be detected. Used to reduce the possibility
  // of crazy fast tempos triggered by static wooshing and other such things which may fool the trigger
  // detection algorithm.
  unsigned long cooldownPeriodInSamples;

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON){
      debugMessage("push event", bid, value, samples);
      clearBpmHistory();
    }
  }

  void clearBpmHistory() {
    // bpmHistory.clear();
    bpmHistory.reset();    
    filterOutput = 0.0f;
    numSamplesProcessed = 0;
    highestAmplitude = 0.0;
    highestAmplitudeInPeriod = 0.0;
    currentlyInsideBeat = false;
    beatLengthRunningAverage = 0;
    numSamplesSinceLastBeat = 0;
    // parameters.set("Current BPM", 0.0);
    // parameters.set("Running BPM", 0.0);
    runningBpm = 0.0;
  }

  float calculateFilterConstant(float sampleRate, float frequency) const {
    return sampleRate / (2.0f * M_PI * frequency);
  }

  void processAudio(AudioBuffer &buffer) {
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    float gain = getParameterValue(PARAMETER_A)*2;
    int size = buffer.getSize();
    for(int i=0; i<size; ++i){
      // debugMessage("gain/min/max", gain, left.getMinValue(), left.getMaxValue());
      // for(int i = 0; i < buffer.getNumSamples(); ++i) {
      float currentSample = left[i];
      float currentSampleAmplitude;

      if(filterEnabled) {
	// This relies on the sample rate which may not be available during initialization
	if(filterConstant == 0.0) {
	  filterConstant = calculateFilterConstant(getSampleRate(), filterFrequency);
	}
	// Basic lowpass filter (feedback)
	filterOutput += (currentSample - filterOutput) / filterConstant;
	currentSampleAmplitude = fabs(filterOutput);
      }
      else {
	currentSampleAmplitude = fabs(currentSample);
      }

      // Find highest peak in the current period
      if(currentSampleAmplitude > highestAmplitudeInPeriod) {
	highestAmplitudeInPeriod = currentSampleAmplitude;

	// Is it also the highest value since we started?
	if(currentSampleAmplitude > highestAmplitude) {
	  highestAmplitude = currentSampleAmplitude;
	}
      }

      // Downsample by skipping samples
      if(--samplesToSkip <= 0) {

	// Beat amplitude trigger has been detected
	if(highestAmplitudeInPeriod >= (highestAmplitude * tolerance / 100.0) &&
	   highestAmplitudeInPeriod > kSilenceThreshold) {

	  // First sample inside of a beat?
	  if(!currentlyInsideBeat && numSamplesSinceLastBeat > cooldownPeriodInSamples) {
	    currentlyInsideBeat = true;
	    float bpm = (getSampleRate() * 60.0f) / ((beatLengthRunningAverage + numSamplesSinceLastBeat) / 2);

	    // Check for half-beat patterns. For instance, a song which has a kick drum
	    // at around 70 BPM but an actual tempo of 140 BPM (hello, dubstep!).
	    float floatdBpm = bpm * 2.0;
	    if(floatdBpm > minimumAllowedBpm && floatdBpm < maximumAllowedBpm) {
	      bpm = floatdBpm;
	    }

	    beatLengthRunningAverage += numSamplesSinceLastBeat;
	    beatLengthRunningAverage /= 2;
	    numSamplesSinceLastBeat = 0;

	    // Check to see that this tempo is within the limits allowed
	    if(bpm > minimumAllowedBpm && bpm < maximumAllowedBpm) {
	      // bpmHistory.push_back(bpm);
	      bpmHistory.push(bpm);
	      beatCallback(bpm, runningBpm);
	      // parameters.set("Beat Triggered", 1.0f);
	      // parameters.set("Current BPM", bpm);

	      // Do total BPM and Reset?
	      if(numSamplesProcessed > period * getSampleRate()) {
		// Take advantage of this trigger point to do a tempo check and adjust the minimum
		// and maximum BPM ranges accordingly.
		// if(useHostTempo) {
		//   minimumAllowedBpm = getHostTempo() - kHostTempoLinkToleranceInBpm;
		//   maximumAllowedBpm = getHostTempo() + kHostTempoLinkToleranceInBpm;
		//   cooldownPeriodInSamples = (unsigned long)(getSampleRate() * (60.0 / maximumAllowedBpm));
		// }

		runningBpm = 0.0;
		// for(unsigned int historyIndex = 0; historyIndex < bpmHistory.size(); ++historyIndex) {
		//   runningBpm += bpmHistory.at(historyIndex);
		// }
		// runningBpm /= (float)bpmHistory.size();
		// bpmHistory.clear();
		int bpmHistoryLen = bpmHistory.available();
		while(bpmHistory.notEmpty())
		  runningBpm += bpmHistory.pull();
		runningBpm /= bpmHistoryLen;
		bpmHistory.reset();
		numSamplesProcessed = 0;
		// parameters.set("Running BPM", runningBpm);
		// debugMessage("Running BPM", runningBpm);
	      }
	    }
	    else {
	      // Outside of bpm threshold, ignore
	    }
	  }
	  else {
	    // Not the first beat mark
	    currentlyInsideBeat = false;
	  }
	}
	else {
	  // Were we just in a beat?
	  if(currentlyInsideBeat) {
	    currentlyInsideBeat = false;
	  }
	}
	samplesToSkip = kDownsampleFactor;
	highestAmplitudeInPeriod = 0.0;
      }
      ++numSamplesProcessed;
      ++numSamplesSinceLastBeat;
    }
  }
};

#endif   // __BeatCounterPatch_hpp__
