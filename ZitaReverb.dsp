import("effect.lib");

zita_rev_fdn_custom(f1,f2,t60dc,t60m,fsmax) =
  ((bus(2*N) :> allpass_combs(N) : feedbackmatrix(N)) ~
   (delayfilters(N,freqs,durs) : fbdelaylines(N)))
with {
  N = 4; // 4x4: 67% CPU, 8x8: 136% CPU on OWL Modular / Pedal
  // Delay-line lengths in seconds:
  apdelays = (0.020346, 0.024421, 0.031604, 0.027333, 0.022904,
              0.029291, 0.013458, 0.019123); // feedforward delays in seconds
  tdelays = ( 0.153129, 0.210389, 0.127837, 0.256891, 0.174713,
              0.192303, 0.125000, 0.219991); // total delays in seconds
  tdelay(i) = floor(0.5 + SR*take(i+1,tdelays)); // samples
  apdelay(i) = floor(0.5 + SR*take(i+1,apdelays));
  fbdelay(i) = tdelay(i) - apdelay(i);
  // NOTE: Since SR is not bounded at compile time, we can't use it to
  // allocate delay lines; hence, the fsmax parameter:
  tdelaymaxfs(i) = floor(0.5 + fsmax*take(i+1,tdelays));
  apdelaymaxfs(i) = floor(0.5 + fsmax*take(i+1,apdelays));
  fbdelaymaxfs(i) = tdelaymaxfs(i) - apdelaymaxfs(i);
  nextpow2(x) = ceil(log(x)/log(2.0));
  maxapdelay(i) = int(2.0^max(1.0,nextpow2(apdelaymaxfs(i))));
  maxfbdelay(i) = int(2.0^max(1.0,nextpow2(fbdelaymaxfs(i))));

  apcoeff(i) = select2(i&1,0.6,-0.6);  // allpass comb-filter coefficient
  allpass_combs(N) =
    par(i,N,(allpass_comb(maxapdelay(i),apdelay(i),apcoeff(i)))); // filter.lib
  fbdelaylines(N) = par(i,N,(delay(maxfbdelay(i),(fbdelay(i)))));
  freqs = (f1,f2); durs = (t60dc,t60m);
  delayfilters(N,freqs,durs) = par(i,N,filter(i,freqs,durs));
  feedbackmatrix(N) = hadamard(N); // math.lib

  staynormal = 10.0^(-20); // let signals decay well below LSB, but not to zero

  special_lowpass(g,f) = smooth(p) with {
    // unity-dc-gain lowpass needs gain g at frequency f => quadratic formula:
    p = mbo2 - sqrt(max(0,mbo2*mbo2 - 1.0)); // other solution is unstable
    mbo2 = (1.0 - gs*c)/(1.0 - gs); // NOTE: must ensure |g|<1 (t60m finite)
    gs = g*g;
    c = cos(2.0*PI*f/float(SR));
  };

  filter(i,freqs,durs) = lowshelf_lowpass(i)/sqrt(float(N))+staynormal
  with {
    lowshelf_lowpass(i) = gM*low_shelf1_l(g0/gM,f(1)):special_lowpass(gM,f(2));
    low_shelf1_l(G0,fx,x) = x + (G0-1)*lowpass(1,fx,x); // filter.lib
    g0 = g(0,i);
    gM = g(1,i);
    f(k) = take(k,freqs);
    dur(j) = take(j+1,durs);
    n60(j) = dur(j)*SR; // decay time in samples
    g(j,i) = exp(-3.0*log(10.0)*tdelay(i)/n60(j));
  };
};

zita_reverb = zita_rev_fdn_custom(f1,f2,t60dc,t60m,fsmax)
with {
  fsmax = 48000.0;
  fdn_group(x) = hgroup(
    "Zita_Rev Internal FDN Reverb [tooltip: ~ Zita_Rev's internal 8x8 Feedback Delay Network (FDN) & Schroeder allpass-comb reverberator.  See Faust's effect.lib for documentation and references]",x);
  t60dc = fdn_group(vslider("[1] Low RT60 [unit:s] [style:knob] [OWL:A]
    [tooltip: T60 = time (in seconds) to decay 60dB in low-frequency band]",
    3, 1, 8, 0.1));
  f1 = fdn_group(vslider("[2] LF X [unit:Hz] [style:knob] [scale:log] [OWL:B]
    [tooltip: Crossover frequency (Hz) separating low and middle frequencies]",
    200, 50, 1000, 1));
  t60m = fdn_group(vslider("[3] Mid RT60 [unit:s] [style:knob] [scale:log] [OWL:C]
    [tooltip: T60 = time (in seconds) to decay 60dB in middle band]",
    2, 1, 8, 0.1));
  f2 = fdn_group(vslider("[4] HF Damping [unit:Hz] [style:knob] [scale:log] [OWL:D]
    [tooltip: Frequency (Hz) at which the high-frequency T60 is half the middle-band's T60]",
    6000, 1500, 0.49*fsmax, 1));
};

process = zita_reverb ;