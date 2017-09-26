import("pm.lib");

// UI elements
freq = hslider("freq[OWL:A]",440,50,1000,0.01);
gate = button("gate[OWL:Push]");
exPos = hslider("exPos[OWL:B]",0.5,0,1,0.01);
p = hslider("pole[OWL:C]",0.9,0,1,0.01)*0.1+0.9;

l = 320/freq; // coverts a length in meters into a frequency
impulsify = _ <: _,mem : - : >(0); // creates an impulse from a button

process = gate : impulsify : idealString(l,p,exPos) <: _,_;