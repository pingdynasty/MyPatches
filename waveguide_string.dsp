// waveguide_string.dsp
//
// waveguide model of a string
//
// - one-pole lowpass termination
//
// Henrik von Coler
// 2020-06-09
//
// ref: https://hvc.berlin/synthesis_algorithms/physical-modeling-faust-examples/
//
// Updated for the OWL by Martin Klang

import("all.lib");

// use '(pm.)l2s' to calculate number of samples
// from length in meters:

segment(maxLength,length) = waveguide(nMax,n)
with{
nMax = maxLength : l2s;
n = length : l2s/2;
};




// one lowpass terminator
fc = hslider("lowpass[OWL:C]",1000,10,10000,1) : si.smoo;
rt = rTermination(basicBlock,*(-1) : si.smooth(1.0-2*(fc/ma.SR)));

// one gain terminator with control
gain = hslider("gain[OWL:B]",0.99,0,1,0.01) : si.smoo;
lt = lTermination(*(-1)* gain,basicBlock);


idString(length,pos,excite) = endChain(wg)
with{

nUp   = length*pos;

nDown = length*(1-pos);

wg = chain(lt : segment(6,nUp) : in(excite) : out : segment(6,nDown) : rt); // waveguide chain
};

length = hslider("length[OWL:A]",1,0.1,12,0.01) : si.smoo;
// exc = button("pluck[OWL:B1]");
exc(b) = b : ba.impulsify : peakholder(hslider("sust[OWL:D]", 10, 1, 80, 1));
b1 = button("pluck1[OWL:B1]");
b2 = button("pluck2[OWL:B2]");
process = idString(length, 0.15, exc(b1)), idString(length, 0.30, exc(b2)) : *(0.5),*(0.5) : _,_;
