#ifndef __WaveBank_h
#define __WaveBank_h

#include "Oscillator.h"

template<size_t X, size_t Y, size_t Z, size_t SIZE>
class WaveBank {
protected:
  float waves[X][Y][Z][SIZE];
public:
  float* getWave(size_t x, size_t y, size_t z){
    return waves[x%X][y%Y][z];
  }
  static WaveBank<X, Y, Z, SIZE>* create(FloatArray wavetable);
  static void destroy(WaveBank<X, Y, Z, SIZE>* obj);
};

template<size_t X, size_t Y, size_t Z, size_t SIZE>
class WaveBankFactory {
private:
  FastFourierTransform *fourier;	
  ComplexFloatArray fft;
  ComplexFloatArray tmp;
  FloatArray dest;
  FloatArray zeros;	
public:
  WaveBankFactory(size_t len) {
    fourier = FastFourierTransform::create(len);
    fft = ComplexFloatArray::create(len);
    tmp = ComplexFloatArray::create(len);
    dest = FloatArray::create(len);
    zeros = FloatArray::create(len);
  }
  ~WaveBankFactory(){
    ComplexFloatArray::destroy(fft);
    ComplexFloatArray::destroy(tmp);
    FloatArray::destroy(dest);
    FloatArray::destroy(zeros);
    FastFourierTransform::destroy(fourier);
  }
  void generate(WaveBank<X, Y, Z, SIZE>* bank, FloatArray sample, size_t x, size_t y){
    fft.clear();
    dest.clear();
    zeros.clear();
    FloatArray dest(bank->getWave(x, y, 0), SIZE); // we borrow this to copy the input sample
    dest.copyFrom(sample); // zero-padding if sample.getSize() < SIZE
    fourier->fft(dest, fft); // destructive
    int fftoffs = fft.getSize();
    for(size_t i=0; i<Z; i++){
      dest = FloatArray(bank->getWave(x, y, i), SIZE);
      fftoffs /= 2;
      fft.setMagnitude(zeros, fftoffs, (fft.getSize())-fftoffs);
      tmp.copyFrom(fft);
      fourier->ifft(tmp, dest);
      // fftoffs /= 2;
      // fft.setMagnitude(zeros, fftoffs, (fft.getSize())-fftoffs);
    }
  }
  void makeMatrix(WaveBank<X, Y, Z, SIZE>* bank, FloatArray samples) {
    FloatArray sample;
    for(size_t x=0 ; x<X ; x++){
      for (size_t y=0 ; y<Y; y++){
	sample = samples.subArray((x*Y + y)*SIZE, SIZE);
	generate(bank, sample, x, y);
      } 
    }
  }
}; 

template<size_t X, size_t Y, size_t Z, size_t SIZE>
class WaveBankOscillator : public Oscillator {
protected:
  const float sr;
  WaveBank<X, Y, Z, SIZE>* waves;
  float x = 0;
  float y = 0;
  float z = 0;
  float phase = 0;
  float incr = 0;
public:
  WaveBankOscillator(WaveBank<X, Y, Z, SIZE>* wavebank, float sr): sr(sr), waves(wavebank) {}
  WaveBank<X, Y, Z, SIZE>* getBank(){
    return waves;
  }
  void setFrequency(float freq){
    incr = freq*SIZE/sr;
    /* z = freq*Z/sr; // really? find which wavetable corresponds to frequency */
    // base frequency: incr=1 (incr=0 up to 187.5 Hz at SIZE=256, incr=6 up to 1312.5 Hz )
    // incr=64 up to 12kHz, log2(64) == 6
    // z = min(Z-1, log2f(incr+1));
    z = max(0.0f, min(Z-1.0f, log2f(freq/SIZE+1)));
    // debugMessage("freq/incr/z/", freq, incr, z);
  }
  float getFrequency(){
    return incr*sr/SIZE;
  }
  void setPhase(float ph){
    phase = ph*SIZE/(2*M_PI);
  }
  float getPhase(){
    return phase*2*M_PI/SIZE;
  }
  void reset(){
    phase = 0;
  }
  void set(float x, float y){
    this->x = x;
    this->y = y;
  }
  void setMorphX(float x){
    this->x = x*X;
  }
  void setMorphY(float y){
    this->y = y*Y;
  }
  float getMorphX() {
    return x/X;
  }
  float getMorphY() {
    return y/Y;
  }
  inline static float compute(float* wave00, float* wave01, float* wave10, float* wave11,
			      float phase, float xf, float yf){
    size_t phi0 = phase; // phase goes from 0.0 to SIZE
#ifdef DDS_INTERPOLATE
    // interpolating samples
    size_t phi1 = (phi0+1) % SIZE;
    float phf1 = phase-phi0; // fractional part
    float phf0 = 1-phf1;
    float s00 = wave00[phi0]*phf0 + wave00[phi1]*phf1;
    float s01 = wave01[phi0]*phf0 + wave01[phi1]*phf1;
    float s10 = wave10[phi0]*phf0 + wave10[phi1]*phf1;
    float s11 = wave11[phi0]*phf0 + wave11[phi1]*phf1;
#else
    float s00 = wave00[phi0];
    float s01 = wave01[phi0];
    float s10 = wave10[phi0];
    float s11 = wave11[phi0];
#endif
    s00 += (s10 - s00)*yf;
    s01 += (s11 - s01)*yf;
    s00 += (s01 - s00)*xf;
    return s00;
  }
  float generate(){
    size_t xi = x;
    float xf = x-xi;
    size_t yi = y;
    float yf = y-yi;
    size_t zi = z; // just use bottom bandwidth-limited wavetable
    float* wave00 = waves->getWave(xi, yi, zi);
    float* wave01 = waves->getWave(xi+1, yi, zi);
    float* wave10 = waves->getWave(xi, yi+1, zi);
    float* wave11 = waves->getWave(xi+1, yi+1, zi);
    float s00 = compute(wave00, wave01, wave10, wave11, phase, xf, yf);
    phase += incr;
    if(phase >= SIZE)
      phase -= SIZE;
    return s00;
  }
  float generate(float fm){
    float sample = generate();
    phase += fm*incr;
    if(phase >= SIZE)
      phase -= SIZE;
    return sample;
  }
#if 0
  using Oscillator::generate;
#else
  void generate(FloatArray output){
    float* out = output.getData();
    size_t len = output.getSize();
    size_t xi = x;
    float xf = x-xi;
    size_t yi = y;
    float yf = y-yi;
    size_t zi = z; // just use bottom bandwidth-limited wavetable
    float* wave00 = waves->getWave(xi, yi, zi);
    float* wave01 = waves->getWave(xi+1, yi, zi);
    float* wave10 = waves->getWave(xi, yi+1, zi);
    float* wave11 = waves->getWave(xi+1, yi+1, zi);
    while(len--){
      *out++ = compute(wave00, wave01, wave10, wave11, phase, xf, yf);
      phase += incr;
      if(phase >= SIZE)
	phase -= SIZE;
    }
  }
  void generate(FloatArray output, FloatArray input){
    float* fm = input.getData();
    float* out = output.getData();
    size_t len = output.getSize();
    size_t xi = x;
    float xf = x-xi;
    size_t yi = y;
    float yf = y-yi;
    size_t zi = z; // just use bottom bandwidth-limited wavetable
    float* wave00 = waves->getWave(xi, yi, zi);
    float* wave01 = waves->getWave(xi+1, yi, zi);
    float* wave10 = waves->getWave(xi, yi+1, zi);
    float* wave11 = waves->getWave(xi+1, yi+1, zi);
    while(len--){
      *out++ = compute(wave00, wave01, wave10, wave11, phase, xf, yf);
      phase += incr * (1 + *fm++);
      if(phase >= SIZE)
    	phase -= SIZE;
    }
  }
#endif
#if 0
  void generate(float newx, float newy, float newz, float* out, size_t len){
    // must be called with abs(newN-N)<1 for x, y, z
    // ie change less than one table in one block
    // and actually: int(N) == int(N+newN)
    // ie no transitions in one block
    // maybe set N = newN before calling if not true?
    
    // set increments based on new values
    float incx = (newx-x)/len;
    float incy = (newy-y)/len;
    // float incz = (newz-z)/len;
    float incz = (newz-z)/len;
    
    size_t xi = x;
    float xf = x-xi;
    size_t yi = y;
    float yf = y-yi;
    size_t zi = z; // just use bottom bandwidth-limited wavetable
    // we should probably be interpolating in z also

    // assuming we're not traversing tables in this block
    // float* wave00 = waves[xi][yi][zi];
    // float* wave01 = waves[xi][yi+1][zi];
    // float* wave10 = waves[xi+1][yi][zi];
    // float* wave11 = waves[xi+1][yi+1][zi];
    float* wave00 = waves->getWave(xi, yi, zi);
    float* wave01 = waves->getWave(xi, yi+1, zi);
    float* wave10 = waves->getWave(xi+1, yi, zi);
    float* wave11 = waves->getWave(xi+1, yi+1, zi);

    while(len--){
      size_t zi0 = phase; // z is phase and goes from 0 to Z
#ifdef DDS_INTERPOLATE
      // interpolating samples
      size_t zi1 = zi0+1;
      float zf1 = phase-zi0; // fractional part
      float zf0 = 1-zf1;
      float s00 = wave00[zi0]*zf0 + wave00[zi1]*zf1;
      float s01 = wave01[zi0]*zf0 + wave01[zi1]*zf1;
      float s10 = wave10[zi0]*zf0 + wave10[zi1]*zf1;
      float s11 = wave11[zi0]*zf0 + wave11[zi1]*zf1;
#else
      float s00 = wave00[zi0];
      float s01 = wave01[zi0];
      float s10 = wave10[zi0];
      float s11 = wave11[zi0];
#endif
      s00 += (s10 - s00)*yf;
      s01 += (s11 - s01)*yf;
      s00 += (s01 - s00)*xf;
      *out++ = s00;

      xf += incx;
      yf += incy;
      if(int(xf) != xi || int(yf) != yi){
	// we've traversed a table boundary
	xi = xf;
	xf = x-xi;
	yi = y;
	yf = y-yi;
	wave00 = waves->getWave(xi, yi, zi);
	wave01 = waves->getWave(xi, yi+1, zi);
	wave10 = waves->getWave(xi+1, yi, zi);
	wave11 = waves->getWave(xi+1, yi+1, zi);
      }
      phase += incr;
      incr += incz;
      if(phase >= SIZE)
	phase -= SIZE;
    }
    x = newx;
    y = newy;
    incr = newz;
  }
#endif
  
  static WaveBankOscillator<X, Y, Z, SIZE>* create(WaveBank<X, Y, Z, SIZE>* bank, float sr){
    return new WaveBankOscillator<X, Y, Z, SIZE>(bank, sr);
  }
  static void destroy(WaveBankOscillator<X, Y, Z, SIZE>* obj){
    delete obj;
  }
};

template<size_t X, size_t Y, size_t Z, size_t SIZE>
WaveBank<X, Y, Z, SIZE>* WaveBank<X, Y, Z, SIZE>::create(FloatArray wavetable){
  WaveBankFactory<X, Y, Z, SIZE>* factory = new WaveBankFactory<X, Y, Z, SIZE>(SIZE);
  WaveBank<X, Y, Z, SIZE>* bank = new WaveBank<X, Y, Z, SIZE>();
  factory->makeMatrix(bank, wavetable);
  delete factory;
  return bank;
}

template<size_t X, size_t Y, size_t Z, size_t SIZE>
void WaveBank<X, Y, Z, SIZE>::destroy(WaveBank<X, Y, Z, SIZE>* obj){
  // for(int x=0; x<X; ++x)
  //   for(int y=0; y<Y; ++y)
  //     for(int z=0; z<Z; ++z)
  // 	delete[] obj->getWave(x, y, z);
  delete obj;
}

#endif	/* __WaveBank_h */
