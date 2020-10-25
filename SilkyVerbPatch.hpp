#ifndef __SilkyVerbPatch_hpp__
#define __SilkyVerbPatch_hpp__

#include "Patch.h"
#include "CircularBuffer.hpp"

/**
 
AUTHOR:
    (c) 1994-2012  Robert Bristow-Johnson
    rbj@audioimagination.com

 
LICENSE:
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 
DESCRIPTION:
    The core of the reverb algorithm implemented here is contained in
    a feedback "matrix" and a set of eight delay lines. This structure
    represents a generalized feedback network in which each delay line
    input receives a linear combination of each of the delay outputs and
    of the input signal to the reverberator.  It is based on the published
    work of Jot:
 
    Digital Delay Networks for Designing Artificial Reverberators
 
    Within the framework of Schroeder's parallel comb filter reverberator,
    a method is proposed for controlling the decay characteristics (avoiding
    unnatural resonances) and for compensating the frequency response. The
    method is extended to any recursive delay network having a unitary
    feedback matrix, and allows selection of the reverberator structure
    irrespective of reverberation time control.
 
    Author: Jot, Jean-Marc
    Affiliation: Antoine Chaigne, Enst, departement SIGNAL, Paris, France
    AES Convention: 90 (February 1991)   Preprint Number:3030
*/

#define PRIME_NUMBER_TABLE_SIZE 7600
#define CHUNK_SIZE    32                          // must be power of 2
#define CHUNK_SIZE_RATIO        ((float) 64.f/CHUNK_SIZE)   // ratio between original (64) and actual chunk size
#define BIG_DELAY_BUFFER_SIZE 65536                       // must be power of 2

#define MAX_REVERB_TIME   480000                      // 10 seconds at 48000 Hz
#define MIN_REVERB_TIME   441
#define MAX_ROOM_SIZE   7552
#define MIN_ROOM_SIZE   (4*CHUNK_SIZE)
#define MAX_CUTOFF    0.4975
#define MIN_CUTOFF    0.1134

#define SQRT8     2.82842712474619 // sqrtf(8)
#define ONE_OVER_SQRT8   0.353553390593274 //  1/sqrtf(8)
#define ALPHA     0.943722057435498 //  powf(3/2, -1/(8-1))
//       of the 8 delay lines, the longest is 3/2 times longer than the shortest.
//       the longest delay is coupled to the room size.
//       the delay lines then decrease exponentially in length.

  uint16_t  primeNumberTable[PRIME_NUMBER_TABLE_SIZE];
// the 7600th prime is 77351

#define BUFFER_LIMIT (65536/8)

int FindNearestPrime(uint16_t* prime_number_table, int number)
{
  while (prime_number_table[number] == 0)
    number--;
  return number;
}

void BuildPrimeTable(uint16_t* prime_number_table){
  int max_stride = (int)sqrtf((float)PRIME_NUMBER_TABLE_SIZE); 
  for(int i=0; i<PRIME_NUMBER_TABLE_SIZE; i++)
    prime_number_table[i] = 1;         // initial value of all entries is 1 
  prime_number_table[0] = 0;          // now we zero out any entry that is not prime
  prime_number_table[1] = 0;
  int stride = 2;             // start with stride set to the smallest prime
  while (stride <= max_stride){
      for(int i=2*stride; i<PRIME_NUMBER_TABLE_SIZE; i+=stride) // start at the 2nd multiple of this prime, NOT the prime number itself!!!
	prime_number_table[i] = 0;        // zero out table entries for all multiples of this prime number
      stride++;
      while (prime_number_table[stride] == 0)      // go to next non-zero entry which is the next prime
	stride++;
  }
}

class Node {
private:
  size_t delay_samples;
  float b0, a1, y1;
  FloatArray result;
  CircularBuffer* buffer;
public:
  Node(size_t bufsize):
    a1(-1), b0(-ONE_OVER_SQRT8), y1(0) {
    result = FloatArray::create(bufsize);
    buffer = CircularBuffer::create(BUFFER_LIMIT);
  }
  ~Node(){
    FloatArray::destroy(result);
  }
  float* getResult(){
    return result.getData();
  }
  void write(float sample){
    buffer->write(sample);
  }
  float filter(float x){
    y1 = y1 + b0*x + a1*y1; // y[n-1] + b0*x[n] + a1*y[n-1]
    return y1; // todo: remove -1 from a1 
  }
  void set(float beta, float fDelaySamples, float fCutoffCoef){
    float prime_value = FindNearestPrime(primeNumberTable, (int)fDelaySamples);
    // we subtract 1 CHUNK of delay, because this signal feeds back, causing an extra CHUNK delay
    delay_samples = prime_value - result.getSize();
    a1 = prime_value*fCutoffCoef - 1.0;
    b0 = ONE_OVER_SQRT8*expf(beta*prime_value)*a1;
  }
  void process(){
    for(size_t i=0; i<result.getSize(); ++i)
      result[i] = filter(buffer->read(-i+delay_samples));
  }
};

class SilkyVerbPatch : public Patch {
  // StereoBiquadFilter* highpass;
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  float fPreDelaySamples;
  int predelayL, predelayR;

  float   dry_coef;
  float   wet_coef0;
  float   wet_coef1;
  float   left_reverb_state;
  float   right_reverb_state;

  Node node0;
  Node node1;
  Node node2;
  Node node3;
  Node node4;
  Node node5;
  Node node6;
  Node node7;

public:
  SilkyVerbPatch() : node0(getBlockSize()),
		     node1(getBlockSize()),
		     node2(getBlockSize()),
		     node3(getBlockSize()),
		     node4(getBlockSize()),
		     node5(getBlockSize()),
		     node6(getBlockSize()),
		     node7(getBlockSize()) {
    delayBufferL = CircularBuffer::create(BUFFER_LIMIT);
    delayBufferR = CircularBuffer::create(BUFFER_LIMIT);    

    static const float delta = 0.05;
    roomSizeSeconds = getFloatParameter("Size", 0.15, 0.6, 0.4, 0.0, delta);
    reverbTimeSeconds = getFloatParameter("Decay", 1, 10, 5, 0.0, delta);
    cutoffFrequency = getFloatParameter("Damp", 16000, 1000, 8000, 0.0, delta); // reversed range 16k to 1k
    dryWet = getFloatParameter("Dry/Wet", 0, 1.0, 0.5, 0.95, delta);
    predelaySeconds = getFloatParameter("Predelay", 0, 0.16, 0.05, 0.0, delta);
 
    left_reverb_state = 0.0;
    right_reverb_state = 0.0;
 
    BuildPrimeTable(primeNumberTable);
  }

  void reverbSetParam(float fSampleRate, float fPercentWet, float fReverbTime, float fRoomSize, float fCutOffAbsorbsion, float fPreDelay){
    float wetCoef = fPercentWet/100.0;
    if (wetCoef > 1.0)
      wetCoef = 1.0;
    if (wetCoef < 0.0)
      wetCoef = 0.0;
 
    float fReverbTimeSamples = fReverbTime*fSampleRate;  // fReverbTime (expressed in seconds if fSampleRate is Hz) is the RT60 for the room
    if (fReverbTimeSamples > MAX_REVERB_TIME)
      fReverbTimeSamples = MAX_REVERB_TIME;
    if (fReverbTimeSamples < MIN_REVERB_TIME)
      fReverbTimeSamples = MIN_REVERB_TIME;
    
    float fRoomSizeSamples = fRoomSize*fSampleRate;   // fRoomSize is expressed in seconds = (room length)/(speed of sound)
    if (fRoomSizeSamples > MAX_ROOM_SIZE)
      fRoomSizeSamples = MAX_ROOM_SIZE;
    if (fRoomSizeSamples < MIN_ROOM_SIZE)
      fRoomSizeSamples = MIN_ROOM_SIZE;
 
    float fCutOff = fCutOffAbsorbsion/fSampleRate;
    if (fCutOff > MAX_CUTOFF)
      fCutOff = MAX_CUTOFF;
    if (fCutOff < MIN_CUTOFF)
      fCutOff = MIN_CUTOFF;
    
    fPreDelaySamples = fPreDelay*fSampleRate;   // fPreDelay is expressed in seconds if fSampleRate is Hz
    if (fPreDelaySamples > MAX_ROOM_SIZE)
      fPreDelaySamples = MAX_ROOM_SIZE;
    if (fPreDelaySamples < 0.0)
      fPreDelaySamples = 0.0;
 
    float fCutoffCoef  = expf(-6.28318530717959*fCutOff);
 
    dry_coef = 1.0 - wetCoef; 
    wetCoef *= SQRT8 * (1.0 - expf(-13.8155105579643*fRoomSizeSamples/fReverbTimeSamples));   // additional attenuation for small room and long reverb time  <--  expf(-13.8155105579643) = 10^(-60dB/10dB)
    //  toss in whatever fudge factor you need here to make the reverb louder
    wet_coef0 = wetCoef;
    wet_coef1 = -fCutoffCoef*wetCoef;
 
    fCutoffCoef /=  (float)FindNearestPrime(primeNumberTable, (int)fRoomSizeSamples);
 
    float fDelaySamples = fRoomSizeSamples;

    // 6.90775527898214 = logf(10^(60dB/20dB))  <-- fReverbTime is RT60
    float	beta = -6.90775527898214/fReverbTimeSamples;
  
    node0.set(beta, fDelaySamples, fCutoffCoef);
    fDelaySamples *= ALPHA;
    node1.set(beta, fDelaySamples, fCutoffCoef);
    fDelaySamples *= ALPHA;
    node2.set(beta, fDelaySamples, fCutoffCoef);
    fDelaySamples *= ALPHA;
    node3.set(beta, fDelaySamples, fCutoffCoef);
    fDelaySamples *= ALPHA;
    node4.set(beta, fDelaySamples, fCutoffCoef);
    fDelaySamples *= ALPHA;
    node5.set(beta, fDelaySamples, fCutoffCoef);
    fDelaySamples *= ALPHA;
    node6.set(beta, fDelaySamples, fCutoffCoef);
    fDelaySamples *= ALPHA; 
    node7.set(beta, fDelaySamples, fCutoffCoef);
  }

  void silky(FloatArray left_input, FloatArray right_input) {
    size_t len = left_input.getSize();

    delayBufferL->write(left_input); // button: toggle left/right
    delayBufferR->write(right_input);
 
    predelayL = (int)fPreDelaySamples - len;
    predelayR = (int)fPreDelaySamples - len;
    
    float* x0 = node0.getResult(); // lpf output
    float* x1 = node1.getResult();
    float* x2 = node2.getResult();
    float* x3 = node3.getResult();
    float* x4 = node4.getResult();
    float* x5 = node5.getResult();
    float* x6 = node6.getResult();
    float* x7 = node7.getResult();

    for(size_t i=0; i<len; i++){
      float acc = delayBufferL->read(-i+predelayL); // left_predelay.output[i];
      acc += x0[i];
      acc += x1[i];
      acc += x2[i];
      acc += x3[i];
      acc -= x4[i];
      acc -= x5[i];
      acc -= x6[i];
      acc -= x7[i];
      node0.write(acc); // delay input
    }

    for(size_t i=0; i<len; i++){
      float acc = delayBufferR->read(-i+predelayR); // right_predelay.output[i];
      acc += x0[i];
      acc += x1[i];
      acc -= x2[i];
      acc -= x3[i];
      acc += x4[i];
      acc += x5[i];
      acc -= x6[i];
      acc -= x7[i];
      node1.write(acc);
    }
 
    for(size_t i=0; i<len; i++){
      float acc = delayBufferR->read(-i+predelayR);
      acc += x0[i];
      acc += x1[i];
      acc -= x2[i];
      acc -= x3[i];
      acc -= x4[i];
      acc -= x5[i];
      acc += x6[i];
      acc += x7[i];
      node2.write(acc);
    }

    for(size_t i=0; i<len; i++){
      float acc = delayBufferL->read(-i+predelayL);
      acc += x0[i];
      acc -= x1[i];
      acc += x2[i];
      acc -= x3[i];
      acc += x4[i];
      acc -= x5[i];
      acc += x6[i];
      acc -= x7[i];
      node3.write(acc);
    }
 
    for(size_t i=0; i<len; i++){
      float acc = delayBufferR->read(-i+predelayR);
      acc += x0[i];
      acc -= x1[i];
      acc += x2[i];
      acc -= x3[i];
      acc -= x4[i];
      acc += x5[i];
      acc -= x6[i];
      acc += x7[i];
      node4.write(acc);
    }

    for(size_t i=0; i<len; i++){
      float acc = delayBufferL->read(-i+predelayL);
      acc += x0[i];
      acc -= x1[i];
      acc -= x2[i];
      acc += x3[i];
      acc += x4[i];
      acc -= x5[i];
      acc -= x6[i];
      acc += x7[i];
      node5.write(acc);
    }

    for(size_t i=0; i<len; i++){
      float acc = delayBufferL->read(-i+predelayL);
      acc += x0[i];
      acc -= x1[i];
      acc -= x2[i];
      acc += x3[i];
      acc -= x4[i];
      acc += x5[i];
      acc += x6[i];
      acc -= x7[i];
      node6.write(acc);
    }
 
    for(size_t i=0; i<len; i++){
      float acc = delayBufferR->read(-i+predelayR);
      acc += x0[i];
      acc += x1[i];
      acc += x2[i];
      acc += x3[i];
      acc += x4[i];
      acc += x5[i];
      acc += x6[i];
      acc += x7[i];
      node7.write(acc);
    }
 
    float* input = left_input;
    float* output = left_input;
    float reverb_output_state = left_reverb_state;
    for (int i=len; i>0; i--){
      float reverb_output = *(x0++) + *(x2++) + *(x4++) + *(x6++);
      float output_acc = dry_coef * (*input++);
      output_acc += wet_coef0 * reverb_output;
      output_acc += wet_coef1 * reverb_output_state;
      *output++ = output_acc;
      reverb_output_state = reverb_output;
    }
    left_reverb_state = reverb_output_state; 
 
    input = right_input;
    output = right_input;
    reverb_output_state = right_reverb_state;
    for (int i=len; i>0; i--){
      float reverb_output = *(x1++) + *(x3++) + *(x5++) + *(x7++);
      float output_acc = dry_coef * (*input++);
      output_acc += wet_coef0 * reverb_output;
      output_acc += wet_coef1 * reverb_output_state;
      *output++ = output_acc;
      reverb_output_state = reverb_output;
    }
    right_reverb_state = reverb_output_state;

    node0.process();
    node1.process();
    node2.process();
    node3.process();
    node4.process();
    node5.process();
    node6.process();
    node7.process();
  }

  void processAudio(AudioBuffer &buffer){
    reverbSetParam(getSampleRate(), dryWet*100, reverbTimeSeconds, roomSizeSeconds, cutoffFrequency, predelaySeconds);
    FloatArray bufL = buffer.getSamples(0);
    FloatArray bufR = buffer.getSamples(1);
    silky(bufL, bufR);
  }
    
private:
  FloatParameter cutoffFrequency;
  FloatParameter roomSizeSeconds;
  FloatParameter reverbTimeSeconds;
  FloatParameter dryWet;
  FloatParameter predelaySeconds;
};

#endif // __SilkyVerbPatch_hpp__
