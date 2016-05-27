/*
  Quantum Harmonic Oscillator Java applet:
  http://web.ift.uib.no/AMOS/MOV/HO/

  Quantum Harmonic Oscillator
  originally by Ondrej Psencik
  http://vega.fjfi.cvut.cz/docs/pvok/aplety/kv_osc/index.html

  Modified 2000 by L. Kocbach
  Modified and extended 2002 by L. Kocbach and Nazila Yavari
  Modified and reorganized 2003 by L. Kocbach

  Modified on the occasion of Glauber's Nobel Prize
  by L. Kocbach, October 2005

  This is a new documented version with many states
  Indicator of Glauber/Superposed added October 2005

*/

template<int samples, int controls>
class HarmonicOscillator {
private:

  // todo: find out discrepancies between Nstate, controls, and HeightConstant
  // int controls; // 10
  int Nstate; // 16
  int  HeightConstant; // 30

  float HALFd;
  float HalfSize;

  const float EnergyConstant = 0.5f; // increasing constant moves waveform up
  const float MAX_VALUE = 127.0f;

  // int controlvalues[controls];
  // int energyControl;

  float dt = 0.001 * 30;
  float t = 1.0f;

  float AverageEnerg;

  float aryCn[controls];
  float rawCn[controls];
  float alfan[controls];
  float nfact[controls];
  float PsiArray[samples+1][controls];

public:
  // HarmonicOscillator(int ssamples, int ccontrols){
  HarmonicOscillator(){

    Nstate = controls;
    HeightConstant = controls;

    // increasing HALFd moves center of waveform to the right
    HALFd = controls / 2.0f; // 5.0f, samples / 10
    //     int HalfSize= samples / 10;
    // increasing HalfSize moves center of waveform to the right
    HalfSize = samples / controls; // 21, samples / 10

    // controlvalues = new int[controls];

    //   Fill the coefficient array used in functions PSI_n_form()
    for(int j=0;j<HeightConstant;j++)
      alfan[j] = fan(j);
        
    nfact[0] = 1.0f;
    for(int j=1;j<HeightConstant;j++)
      nfact[j] = nfact[j-1]*(float)j;

    initialisePSIArray();
  }


  void initialisePSIArray(){
    float exp, x;
    for(int lpoint=0; lpoint <= samples; ++lpoint){
      x = ((float)lpoint) / HalfSize - HALFd;
      exp = expf(-x * x / 2.0);
      for(int jquant=0; jquant < Nstate; jquant++)
	PsiArray[lpoint][jquant] = PSI_n_form(x, jquant, exp);                    
    }
  }

  //
  //  Mathematical Functions 
  //
  //    Constant factors used in alfan[] filled above
  //    fan(i) = 1/ (  sqrt( n!  2^i sqrt( Pi ) )
  float fan(int i){ 
    float d=1;
    for(int j=1; j<=i; j++)
      d=d*j;
    return( 1/ ( sqrtf(d*sqrtf(M_PI)*powf(2.0,i+0.0) ) ) );
  }

  //
  //    Hermite Polynomial
  float hermite(int n, float x){ 
    float h2 = 1.0f;
    float h1 = 2.0f * x;
    float hn = 1.0f;
    if(n==0) 
      return h2;
    if(n==1) 
      return h1;
    for(int i=2; i<=n; ++i){
      hn = 2.0f * x * h1 - 2.0f * ((float)i - 1.0f) * h2;
      h2 = h1; 
      h1 = hn;
    }
    return(hn);
  }

  //    n-th state of Harm.Osc. in terms of Hermite Polynomial
  //    and the constant alfan[n+1]
  float PSI_n_form(float x, int n, float exp){
    return(alfan[n] * hermite(n, x) * exp);
  }

  //
  //    Constructing the Wavefunction for a given  time
  //    and the constant alfan[n+1]
  float PSI(float t, float x, int xx){  
    float Cn;
    float Re = 0;
    float Im = 0;
    for(int i=0;i<Nstate;i++){
      Cn = aryCn[i];
      if(Cn>0.0){
	Re += cosf((i-0.5)*t)*Cn*PsiArray[xx+1][i];
	Im += sinf((i-0.5)*t)*Cn*PsiArray[xx+1][i];
      }
    }
    return Re*Re + Im*Im;
  }

  int getDistance(){
    return (int)(HALFd * MAX_VALUE / controls);
  }

  // HALFd controls the horizontal position of the waveform
  // less means further to the left
  void setDistance(int distance){
    // HALFd = controls / 2.0;
    HALFd = (distance * controls) / (MAX_VALUE);
    initialisePSIArray();
  }

  // HalfSize controls the width of the waveform
  // less means narrower band, higher frequency sound
  int getWavelength(){
    return (int)(HalfSize * 4.0f * MAX_VALUE / samples);
  }

  void setWavelength(int wavelength){
    // HalfSize = samples / controls
    HalfSize = (int)((wavelength * samples) / (4.0f * MAX_VALUE));
    if(HalfSize == 0)
      HalfSize = 1;
    initialisePSIArray();
  }

  void setControl(int index, float value){
    rawCn[index] = value;
  }

  //  Amplitude controls start

  // Normalize amplitudes
  void normalizeAmplitudes(){
    float arySum = 0.0001f;
    for(int i=0;i<Nstate;i++)
      arySum += rawCn[i] * rawCn[i];
    float aryFact  = 1.0f / sqrtf(arySum);

    for(int i=0;i<Nstate;i++){
      aryCn[i] = rawCn[i] * aryFact;
      rawCn[i] = aryCn[i];
    }

    // Evaluate AverageEnerg
    AverageEnerg = EnergyConstant;
    for(int i=0; i<controls; i++)
      AverageEnerg += ((float)i) * aryCn[i] * aryCn[i];
    // energyControl = (int)((MAX_VALUE / 10.0f) * (AverageEnerg - EnergyConstant));
  }

  // Setting of amplitudes for a single state
  void setSingleState(int Nvalue){
    //         System.out.println("single state "+Nvalue);
    for(int i=0; i<controls; ++i)
      rawCn[i] = 0.0;
    rawCn[Nvalue] = 1.0;

    normalizeAmplitudes();

    for(int i=0; i<controls; ++i){
      rawCn[i] = MAX_VALUE * aryCn[i];
      // controlvalues[i] = (int)rawCn[i];
    }

  }

  //    Setting of amplitudes for Glauber State by energy selector scroller
  void setGlauberState(int Nvalue){
    //         AverageEnerg=EnergyConstant+0.0001+(10.0-0.1*(float)Nvalue );  // SETENERGY
    AverageEnerg = EnergyConstant + 10.0f * Nvalue / MAX_VALUE;  // SETENERGY

    float Xx = sqrtf(AverageEnerg-EnergyConstant);
    float Xx2;

    for(int i=0; i<Nstate; ++i){
      //  Matlab values
      //  for N=2:10,fac(N)=fac(N-1)*(N-1);end
      //  for N=0:9,coef(N+1)=x^N/sqrt(fac(N+1))*exp(-0.5*x^2);end
      Xx2 = 0.5f * powf(Xx, 2);
      aryCn[i] = powf(Xx,i) / sqrtf(nfact[i]) * expf(-Xx2);
    }

    //         for(int i=0; i<controls; i++)
    //             rawCn[i] = aryCn[i];

    for(int i=0; i<controls; ++i){
      rawCn[i] = MAX_VALUE * aryCn[i];
      // controlvalues[i] = (int)rawCn[i];
    }

  }

  void calculate(float* values, int size){
    // calculating the wavefunction in sample points
    values[0] = PSI(t,0,0);
    for(int i=1; i<size; ++i)
      values[i] = PSI(t, i / HalfSize - HALFd, i);
  }

  void calculateNormalized(float* values, int size){
    values[0] = PSI(t,0,0);
    float max = values[0];
    float min = values[0];
    for(int i=1; i<size; ++i){
      values[i] = PSI(t, i / HalfSize - HALFd, i);
      if(values[i] > max)
  	max = values[i];
      if(values[i] < min)
  	min = values[i];
    }
    for(int i=0; i<size; ++i)
      values[i] = (values[i] - min) / (max - min);
  }

  void increment(){
    t += dt;
  }

  void setTimeStep(float value){
    dt = value * 0.1;
  }

  float getTimeStep(){
    return dt *10;
  }
};
