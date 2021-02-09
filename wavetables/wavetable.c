/**
 * Robert Bristow-Johnson's wavetable oscillator core posted here:
 * https://www.earlevel.com/main/2020/01/04/further-thoughts-on-wave-table-oscillators/
 *
 * "I hope you don’t mind if I simply paste in here the core code for the wavetable oscillator that I wrote. It is not a complete solution, just the inner core code. It shows how to interpolate between wavetables in up to three dimensions. one of those dimensions will be the keyboard or MIDI note (so wavetables along that dimension will be losing harmonics as you go up in pitch)."
 * "I just wanted to share some fundamental wavetable code, the actual synthesis code, to sorta demonstrate how interpolation between wavetables along different dimensions can be done. When we were trying this out, usually one dimension was the MIDI pitch, another dimension was elapsed time after NoteOn, and if we needed a modwheel or pedal or key velocity then we had a third dimension of interpolation. We also tried out a method to detune some harmonics slightly using "cylindrical coordinates" (one of the dimensions was circular, not linear) that could cause some harmonics to detune more than others."

 */

//
//  This typedef in wavetable_oscillator.h
//
typedef struct
{
  float* output_ptr;
  int samples_per_block;

  unsigned long phase;
  long phaseIncrement;
  long frequencyIncrement;

  float scaler_fractionalBits;		// 2^(-num_fractionalBits)
  unsigned int num_fractionalBits;
  unsigned long mask_fractionalBits;	// 2^num_fractionalBits - 1
  unsigned int mask_waveIndex;

  float fadeDim1;
  float fadeDim1Increment;
  float fadeDim2;
  float fadeDim2Increment;
  float fadeDim3;
  float fadeDim3Increment;

  float* wave000;
  float* wave001;
  float* wave010;
  float* wave011;
  float* wave100;
  float* wave101;
  float* wave110;
  float* wave111;
} wavetable_oscillator_data;
//
// #include "wavetable_oscillator.h"
//


void wavetable_0dimensional_oscillator(wavetable_oscillator_data* this_oscillator)
{
  float* out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;
	
  unsigned long phase = this_oscillator->phase;
  long phaseIncrement = this_oscillator->phaseIncrement;
  long frequencyIncrement = this_oscillator->frequencyIncrement;
	
  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  unsigned long mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;
		
  float* wave000 = this_oscillator->wave000;
	
  while (num_samples_remaining-- > 0)
    {
      unsigned int waveIndex0 = (unsigned int)(phase>>num_fractionalBits) & mask_waveIndex;
      unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
      float linearGain1 = scaler_fractionalBits * (float)(phase & mask_fractionalBits);
      float linearGain0 = 1.0f - linearGain1;
		
      float _wave000 = wave000[waveIndex0]*linearGain0 + wave000[waveIndex1]*linearGain1;
		
      phase += phaseIncrement;
      phaseIncrement += frequencyIncrement;
		
      *out++ = _wave000;
    }
	
  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}


void wavetable_1dimensional_oscillator(wavetable_oscillator_data* this_oscillator)
{
  float* out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;
	
  unsigned long phase = this_oscillator->phase;
  long phaseIncrement = this_oscillator->phaseIncrement;
  long frequencyIncrement = this_oscillator->frequencyIncrement;
	
  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  unsigned long mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;
	
  float fadeDim1 = this_oscillator->fadeDim1;
  float fadeDim1Increment = this_oscillator->fadeDim1Increment;
	
  float* wave000 = this_oscillator->wave000;
  float* wave001 = this_oscillator->wave001;
	
  while (num_samples_remaining-- > 0)
    {
      unsigned int waveIndex0 = (unsigned int)(phase>>num_fractionalBits) & mask_waveIndex;
      unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
      float linearGain1 = scaler_fractionalBits * (float)(phase & mask_fractionalBits);
      float linearGain0 = 1.0f - linearGain1;
		
      float _wave000 = wave000[waveIndex0]*linearGain0 + wave000[waveIndex1]*linearGain1;
      float _wave001 = wave001[waveIndex0]*linearGain0 + wave001[waveIndex1]*linearGain1;
		
      _wave000 += (_wave001 - _wave000)*fadeDim1;
		
      fadeDim1 += fadeDim1Increment;
		
      phase += phaseIncrement;
      phaseIncrement += frequencyIncrement;
		
      *out++ = _wave000;
    }
	
  this_oscillator->fadeDim1 = fadeDim1;
	
  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}


void wavetable_2dimensional_oscillator(wavetable_oscillator_data* this_oscillator)
{
  float* out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;
	
  unsigned long phase = this_oscillator->phase;
  long phaseIncrement = this_oscillator->phaseIncrement;
  long frequencyIncrement = this_oscillator->frequencyIncrement;
	
  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  unsigned long mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;
	
  float fadeDim1 = this_oscillator->fadeDim1;
  float fadeDim1Increment = this_oscillator->fadeDim1Increment;
  float fadeDim2 = this_oscillator->fadeDim2;
  float fadeDim2Increment = this_oscillator->fadeDim2Increment;
	
  float* wave000 = this_oscillator->wave000;
  float* wave001 = this_oscillator->wave001;
  float* wave010 = this_oscillator->wave010;
  float* wave011 = this_oscillator->wave011;
	
  while (num_samples_remaining-- > 0)
    {
      unsigned int waveIndex0 = (unsigned int)(phase>>num_fractionalBits) & mask_waveIndex;
      unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
      float linearGain1 = scaler_fractionalBits * (float)(phase & mask_fractionalBits);
      float linearGain0 = 1.0f - linearGain1;
		
      float _wave000 = wave000[waveIndex0]*linearGain0 + wave000[waveIndex1]*linearGain1;
      float _wave001 = wave001[waveIndex0]*linearGain0 + wave001[waveIndex1]*linearGain1;
      float _wave010 = wave010[waveIndex0]*linearGain0 + wave010[waveIndex1]*linearGain1;
      float _wave011 = wave011[waveIndex0]*linearGain0 + wave011[waveIndex1]*linearGain1;
		
      _wave000 += (_wave010 - _wave000)*fadeDim2;
      _wave001 += (_wave011 - _wave001)*fadeDim2;

      _wave000 += (_wave001 - _wave000)*fadeDim1;
		
      fadeDim2 += fadeDim2Increment;
      fadeDim1 += fadeDim1Increment;
		
      phase += phaseIncrement;
      phaseIncrement += frequencyIncrement;
		
      *out++ = _wave000;
    }
	
  this_oscillator->fadeDim1 = fadeDim1;
  this_oscillator->fadeDim2 = fadeDim2;
	
  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}


void wavetable_3dimensional_oscillator(wavetable_oscillator_data* this_oscillator)
{
  float* out = this_oscillator->output_ptr;
  int num_samples_remaining = this_oscillator->samples_per_block;
	
  unsigned long phase = this_oscillator->phase;
  long phaseIncrement = this_oscillator->phaseIncrement;
  long frequencyIncrement = this_oscillator->frequencyIncrement;
	
  float scaler_fractionalBits = this_oscillator->scaler_fractionalBits;
  unsigned int num_fractionalBits = this_oscillator->num_fractionalBits;
  unsigned long mask_fractionalBits = this_oscillator->mask_fractionalBits;
  unsigned int mask_waveIndex = this_oscillator->mask_waveIndex;
	
  float fadeDim1 = this_oscillator->fadeDim1;
  float fadeDim1Increment = this_oscillator->fadeDim1Increment;
  float fadeDim2 = this_oscillator->fadeDim2;
  float fadeDim2Increment = this_oscillator->fadeDim2Increment;
  float fadeDim3 = this_oscillator->fadeDim3;
  float fadeDim3Increment = this_oscillator->fadeDim3Increment;
	
  float* wave000 = this_oscillator->wave000;
  float* wave001 = this_oscillator->wave001;
  float* wave010 = this_oscillator->wave010;
  float* wave011 = this_oscillator->wave011;
  float* wave100 = this_oscillator->wave100;
  float* wave101 = this_oscillator->wave101;
  float* wave110 = this_oscillator->wave110;
  float* wave111 = this_oscillator->wave111;
	
  while (num_samples_remaining-- > 0)
    {
      unsigned int waveIndex0 = (unsigned int)(phase>>num_fractionalBits) & mask_waveIndex;
      unsigned int waveIndex1 = (waveIndex0 + 1) & mask_waveIndex;
      float linearGain1 = scaler_fractionalBits * (float)(phase & mask_fractionalBits);
      float linearGain0 = 1.0f - linearGain1;
		
      float _wave000 = wave000[waveIndex0]*linearGain0 + wave000[waveIndex1]*linearGain1;
      float _wave001 = wave001[waveIndex0]*linearGain0 + wave001[waveIndex1]*linearGain1;
      float _wave010 = wave010[waveIndex0]*linearGain0 + wave010[waveIndex1]*linearGain1;
      float _wave011 = wave011[waveIndex0]*linearGain0 + wave011[waveIndex1]*linearGain1;
      float _wave100 = wave100[waveIndex0]*linearGain0 + wave100[waveIndex1]*linearGain1;
      float _wave101 = wave101[waveIndex0]*linearGain0 + wave101[waveIndex1]*linearGain1;
      float _wave110 = wave110[waveIndex0]*linearGain0 + wave110[waveIndex1]*linearGain1;
      float _wave111 = wave111[waveIndex0]*linearGain0 + wave111[waveIndex1]*linearGain1;
		
      _wave000 += (_wave100 - _wave000)*fadeDim3;
      _wave001 += (_wave101 - _wave001)*fadeDim3;
      _wave010 += (_wave110 - _wave010)*fadeDim3;
      _wave011 += (_wave111 - _wave011)*fadeDim3;

      _wave000 += (_wave010 - _wave000)*fadeDim2;
      _wave001 += (_wave011 - _wave001)*fadeDim2;

      _wave000 += (_wave001 - _wave000)*fadeDim1;
		
      fadeDim3 += fadeDim3Increment;
      fadeDim2 += fadeDim2Increment;
      fadeDim1 += fadeDim1Increment;
		
      phase += phaseIncrement;
      phaseIncrement += frequencyIncrement;
		
      *out++ = _wave000;
    }
	
  this_oscillator->fadeDim1 = fadeDim1;
  this_oscillator->fadeDim2 = fadeDim2;
  this_oscillator->fadeDim3 = fadeDim3;
	
  this_oscillator->phase = phase;
  this_oscillator->phaseIncrement = phaseIncrement;
}
