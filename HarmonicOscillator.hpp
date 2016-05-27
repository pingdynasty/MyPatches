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

  double HALFd;
  int HalfSize;

  const double EnergyConstant = 0.5d; // increasing constant moves waveform up
  const double MAX_VALUE = 127.0d;

  int controlvalues[controls];
  int energyControl;

  double dt = 0.001 * 30;
  double t = 1.0d;

  double AverageEnerg;

  double aryCn[controls];
  double rawCn[controls];
  double alfan[controls];
  double nfact[controls];
  double PsiArray[samples+1][controls];

public:
  // HarmonicOscillator(int ssamples, int ccontrols){
  HarmonicOscillator(){
    // samples = ssamples;
    // controls = ccontrols;

    Nstate = controls;
    HeightConstant = controls;
    // aryCn = new double[HeightConstant];
    // rawCn = new double[HeightConstant];
    // alfan = new double[HeightConstant];
    // nfact = new double[HeightConstant];

    // increasing HALFd moves center of waveform to the right
    HALFd = controls / 2.0d; // 5.0d, samples / 10
    //     int HalfSize= samples / 10;
    // increasing HalfSize moves center of waveform to the right
    HalfSize = samples / controls; // 21, samples / 10

    // controlvalues = new int[controls];

    //   Fill the coefficient array used in functions PSI_n_form()
    for(int j=0;j<HeightConstant;j++)
      alfan[j] = fan(j);
        
    nfact[0] = 1.0d;
    for(int j=1;j<HeightConstant;j++)
      nfact[j] = nfact[j-1]*(double)j;

    // PsiArray = new double[samples+1][HeightConstant];
    initialisePSIArray();
  }


  void initialisePSIArray(){
    double exp, x;
    for(int lpoint=0; lpoint <= samples; ++lpoint){
      x = ((double)lpoint) / HalfSize - HALFd;
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
  double fan(int i){ 
    double d=1;
    for(int j=1; j<=i; j++)
      d=d*j;
    return( 1/ ( sqrtf(d*sqrtf(M_PI)*powf(2.0,i+0.0) ) ) );
  }

  //
  //    Hermite Polynomial
  double hermite(int n, double x){ 
    double h2 = 1.0d;
    double h1 = 2.0d * x;
    double hn = 1.0d;
    if(n==0) 
      return h2;
    if(n==1) 
      return h1;
    for(int i=2; i<=n; ++i){
      hn = 2.0d * x * h1 - 2.0d * ((double)i - 1.0d) * h2;
      h2 = h1; 
      h1 = hn;
    }
    return(hn);
  }

  //    n-th state of Harm.Osc. in terms of Hermite Polynomial
  //    and the constant alfan[n+1]
  double PSI_n_form(double x, int n, double exp){
    return(alfan[n] * hermite(n, x) * exp);
  }

  //
  //    Constructing the Wavefunction for a given  time
  //    and the constant alfan[n+1]
  double PSI(double t, double x, int xx){  
    double Cn;
    double Re = 0;
    double Im = 0;
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
    return (int)(HalfSize * 4.0d * MAX_VALUE / samples);
  }

  void setWavelength(int wavelength){
    // HalfSize = samples / controls
    HalfSize = (int)((wavelength * samples) / (4.0d * MAX_VALUE));
    if(HalfSize == 0)
      HalfSize = 1;
    initialisePSIArray();
  }

  int getEnergy(){
    return energyControl;
  }

  void setEnergy(int eenergyControl){
    energyControl = eenergyControl;
    setGlauberState(energyControl);
  }

  int getControls(){
    return controls; // controlvalues.length;
  }

  int getControl(int index){
    // assert index < controlvalues.length;
    return controlvalues[index];
  }

  void setControl(int index, int value){
    //         System.out.println("control "+index+": "+value);
    // assert index < controlvalues.length;
    // assert value <= MAX_VALUE;
    // assert value >= 0;
    controlvalues[index] = value;
    updateControlValues();
  }

  //  Amplitude controls start

  // Normalize amplitudes
  void normalizeAmplitudes(){
    double arySum = 0.0001d;
    for(int i=0;i<Nstate;i++)
      arySum += rawCn[i] * rawCn[i];
    double aryFact  = 1.0d / sqrtf(arySum);

    for(int i=0;i<Nstate;i++){
      aryCn[i] = rawCn[i] * aryFact;
      rawCn[i] = aryCn[i];
    }

    // Evaluate AverageEnerg
    AverageEnerg = EnergyConstant;
    for(int i=0; i<controls; i++)
      AverageEnerg += ((double)i) * aryCn[i] * aryCn[i];
    energyControl = (int)((MAX_VALUE / 10.0d) * (AverageEnerg - EnergyConstant));
  }

  // Setting of amplitudes by scrollers
  void updateControlValues(){
    // for(int i=0;i<controlvalues.length;++i)
    for(int i=0;i<controls;++i)
      rawCn[i] = ((double)controlvalues[i]) / MAX_VALUE;

    normalizeAmplitudes();
  }

  // Setting of amplitudes for a single state
  void setSingleState(int Nvalue){
    // assert Nvalue < controlvalues.length;
    //         System.out.println("single state "+Nvalue);
    for(int i=0; i<controls; ++i)
      rawCn[i] = 0.0;
    rawCn[Nvalue] = 1.0;

    normalizeAmplitudes();

    for(int i=0; i<controls; ++i){
      rawCn[i] = MAX_VALUE * aryCn[i];
      controlvalues[i] = (int)rawCn[i];
    }

  }

  //    Setting of amplitudes for Glauber State by energy selector scroller
  void setGlauberState(int Nvalue){
    //         System.out.println("glauber state "+Nvalue);
    energyControl = Nvalue;

    //         AverageEnerg=EnergyConstant+0.0001+(10.0-0.1*(double)Nvalue );  // SETENERGY
    AverageEnerg = EnergyConstant + 10.0d * Nvalue / MAX_VALUE;  // SETENERGY

    double Xx = sqrtf(AverageEnerg-EnergyConstant);
    double Xx2;

    for(int i=0; i<Nstate; ++i){
      //  Matlab values
      //  for N=2:10,fac(N)=fac(N-1)*(N-1);end
      //  for N=0:9,coef(N+1)=x^N/sqrt(fac(N+1))*exp(-0.5*x^2);end
      Xx2 = 0.5d * powf(Xx, 2);
      aryCn[i] = powf(Xx,i) / sqrtf(nfact[i]) * expf(-Xx2);
    }

    //         for(int i=0; i<controls; i++)
    //             rawCn[i] = aryCn[i];

    for(int i=0; i<controls; ++i){
      rawCn[i] = MAX_VALUE * aryCn[i];
      controlvalues[i] = (int)rawCn[i];
    }

  }

  void calculate(float* values, int size){
  // double* calculate() {  
    // calculating the wavefunction in sample points
    // values = new double[samples];
    // memset(values, 0, sizeof(values));
    // calculate first value
    values[0] = PSI(t,0,0);
    for(int i=1; i<size; ++i)
      values[i] = PSI(t, i / HalfSize - HALFd, i);
    // return values;
  }

  // double* calculateNormalized() {  
  //   // values = new double[samples];
  //   memset(values, 0, sizeof(values));
  //   values[0] = PSI(t,0,0);
  //   double max = values[0];
  //   double min = values[0];
  //   for(int i=1; i<samples; ++i){
  //     values[i] = PSI(t, i / HalfSize - HALFd, i);
  //     if(values[i] > max)
  // 	max = values[i];
  //     if(values[i] < min)
  // 	min = values[i];
  //   }
  //   for(int i=0; i<values.length; ++i)
  //     values[i] = (values[i] - min) / (max - min);
  //   return values;
  // }

  void increment(){
    t += dt;
  }

  // void dump(){
  //   for(int j=0;j<HeightConstant;j++)
  //     System.out.println("alfan["+j+"] "+alfan[j]);
  //   for(int j=0;j<HeightConstant;j++)
  //     System.out.println("nfact["+j+"] "+nfact[j]);

  //   for(int lpoint=0; lpoint < samples; ++lpoint)
  //     for(int jquant=0; jquant < Nstate; jquant++)
  // 	System.out.println("PsiArray["+lpoint+"]["+jquant+"]"+PsiArray[lpoint][jquant]);
  // }

  void setTimeStep(int value){
    dt = value * 0.001d;
  }

  int getTimeStep(){
    return (int)(dt / 0.001d);
  }

  // Set the time delta value. Should be small, 0.001 - 0.100.
  void setTimeStep(double ddt){
    //         System.out.println("dt: "+dt);
    dt = ddt;
  }
};
