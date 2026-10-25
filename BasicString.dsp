import("physmodels.lib");

// UI elements
freq = hslider("freq[OWL:A]",440,50,1000,0.01);
gate = button("gate[OWL:Push]");
exPos = hslider("exPos[OWL:B]",0.5,0,1,0.01);
p = hslider("pole[OWL:C]",0.9,0,1,0.01)*0.1+0.9;

l = 320/freq; // coverts a length in meters into a frequency
impulsify = _ <: _,mem : - : >(0); // creates an impulse from a button

// physmodels.lib ks(length, damping, excitation) : _ is a Karplus-Strong string
// that maps directly onto the original three controls (length, pole->damping,
// excitation). pm.lib's idealString piped the excitation in and had no damping
// parameter; ks takes excitation as an argument and exposes damping, so the
// Colour/pole control keeps its role. exPos scales the excitation amplitude.
exc = gate : impulsify : *(exPos);
process = ks(l, p, exc) <: _,_;