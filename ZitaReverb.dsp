// re = library("reverbs.lib");
// ef = library("misceffects.lib");
import("effect.lib");
// import("reverbs.lib");

// zita_light = (_,_ <: zita_rev1_stereo(rdel,f1,f2,t60dc,t60m,fsmax),_,_ : 
// 	out_eq,_,_ : dry_wet : out_level)
// with{
//   fsmax = 48000.0;
//   t60dc = vslider("[1] Low RT60 [unit:s] [style:knob] [OWL:A]
//     [tooltip: T60 = time (in seconds) to decay 60dB in low-frequency band]",
//     3, 1, 8, 0.1);
//   f1 = vslider("[2] LF X [unit:Hz] [style:knob] [scale:log] [OWL:B]
//     [tooltip: Crossover frequency (Hz) separating low and middle frequencies]",
//     200, 50, 1000, 1);
//   t60m = vslider("[3] Mid RT60 [unit:s] [style:knob] [scale:log] [OWL:C]
//     [tooltip: T60 = time (in seconds) to decay 60dB in middle band]",
//     2, 1, 8, 0.1);
//   f2 = vslider("[4] HF Damping [unit:Hz] [style:knob] [scale:log] [OWL:E]
//     [tooltip: Frequency (Hz) at which the high-frequency T60 is half the middle-band's T60]",
//     6000, 1500, 0.49*fsmax, 1);
// 	// fsmax = 48000.0;  // highest sampling rate that will be used
// 	rdel = 60;
// 	// f1 = 200;
// 	// t60dc = 3;
// 	// t60m = 2;
// 	// f2 = 6000;
// 	out_eq = pareq_stereo(eq1f,eq1l,eq1q) : pareq_stereo(eq2f,eq2l,eq2q);
// 	pareq_stereo(eqf,eql,Q) = fi.peak_eq_rm(eql,eqf,tpbt), fi.peak_eq_rm(eql,eqf,tpbt)
// 	with {
// 		tpbt = wcT/sqrt(max(0,g)); // tan(PI*B/SR), B bw in Hz (Q^2 ~ g/4)
// 		wcT = 2*ma.PI*eqf/ma.SR;  // peak frequency in rad/sample
// 		g = ba.db2linear(eql); // peak gain
// 	};
// 	eq1f = 315;
// 	eq1l = 0;
// 	eq1q = 3;
// 	eq2f = 1500;
// 	eq2l = 0;
// 	eq2q = 3;
// 	dry_wet(x,y) = *(wet) + dry*x, *(wet) + dry*y 
// 	with {
// 		wet = 0.5*(drywet+1.0);
// 		dry = 1.0-wet;
// 	};
// 	drywet = vslider("[1] Dry/Wet Mix [OWL:D]",
// 		0,-1.0,1.0,0.01) : si.smoo;
// 	gain = vslider("[2] Level [unit:dB] [style:knob] [tooltip: Output scale
// 		factor]", -6, -70, 40, 0.1) : ba.db2linear : si.smoo;
// 	out_level = *(gain),*(gain);
// };

// process = zita_light; // 108%

zita_rev_fdn_custom(f1,f2,t60dc,t60m,fsmax) =
  ((bus(2*N) :> allpass_combs(N) : feedbackmatrix(N)) ~
   (delayfilters(N,freqs,durs) : fbdelaylines(N)))
with {
  N = 8; // 4x4: 67% CPU, 8x8: 136% CPU on OWL Modular / Pedal, 85% on Magus
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

// zita_rev1_stereo_custom(rdel,f1,f2,t60dc,t60m,fsmax) =
//    zita_in_delay(rdel)
//  : zita_distrib2(N)
//  : zita_rev_fdn_custom(f1,f2,t60dc,t60m,fsmax)
//  : output2(N)
// with {
//  N = 8;
//  output2(N) = outmix(N) : *(t1),*(t1);
//  t1 = 0.37; // zita-rev1 linearly ramps from 0 to t1 over one buffer
//  outmix(4) = !,butterfly(2),!; // probably the result of some experimenting!
//  outmix(N) = outmix(N/2),par(i,N/2,!);
// };

// zita_reverb = zita_rev1_stereo(rdel, f1,f2,t60dc,t60m,fsmax)
zita_reverb = zita_rev_fdn_custom(f1,f2,t60dc,t60m,fsmax)
with {
  fsmax = 48000.0;
  t60dc = vslider("[1] Low RT60 [unit:s] [style:knob] [OWL:A]
    [tooltip: T60 = time (in seconds) to decay 60dB in low-frequency band]",
    3, 1, 8, 0.1);
  f1 = vslider("[2] LF X [unit:Hz] [style:knob] [scale:log] [OWL:B]
    [tooltip: Crossover frequency (Hz) separating low and middle frequencies]",
    200, 50, 1000, 1);
  t60m = vslider("[3] Mid RT60 [unit:s] [style:knob] [scale:log] [OWL:C]
    [tooltip: T60 = time (in seconds) to decay 60dB in middle band]",
    2, 1, 8, 0.1);
  f2 = vslider("[4] HF Damping [unit:Hz] [style:knob] [scale:log] [OWL:E]
    [tooltip: Frequency (Hz) at which the high-frequency T60 is half the middle-band's T60]",
    6000, 1500, 0.49*fsmax, 1);
  rdel = vslider("[1] In Delay [unit:ms] [style:knob] [tooltip: Delay in ms
  		before reverberation begins]",60,20,100,1);
};

// mix = vslider("[Mix[OWL:D]", 0.5, 0, 1, 0.01);


// process(l,r) = l,r <: *(1-mix), *(1-mix),
	       
process = bus(8) : zita_reverb : bus(8);
// // : dry_wet
// // with {
// //   wet = vslider("[Mix[OWL:D]", 0.5, 0, 1, 0.01);
// //   dry_wet(x,y) = *(wet) + (1 - wet)*x, *(wet) + (1 - wet)*y;
// // };
