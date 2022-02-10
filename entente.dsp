declare name "L'Entente Cordiale";
declare message "L'Entente Cordiale";
declare description "French and English church bells in unison.";

import("stdfaust.lib");

englishBell(strikePosition,strikeCutoff,strikeSharpness,gain,trigger) =
  pm.strikeModel(10,strikeCutoff,strikeSharpness,gain,trigger) :
  pm.englishBellModel(45,strikePosition,30,1,3); // modes >= 46 doesn't work on Genius

// process = pm.frenchBell(pos : ba.sAndH(ba.impulsify(lgate)),cutoff,sharp,gain,lgate),
// 	  englishBell(pos : ba.sAndH(ba.impulsify(rgate)),cutoff,sharp,gain,rgate)
process = pm.frenchBell(pos : ba.latch(lgate),cutoff,sharp,gain,lgate),
	  englishBell(pos : ba.latch(rgate),cutoff,sharp,gain,rgate)
with{
  pos = nentry("Position[OWL:A]",0,0,6,1);
  sharp = hslider("Sharpness[OWL:B]",0.5,0.0001,1,0.01), 2 : pow;
  cutoff = hslider("Cutoff[OWL:C]",6500,20,20000,1);
  gain = hslider("Gain[OWL:D]",0.5,0,1,0.01);
  lgate = button("France[OWL:B1]");
  rgate = button("England[OWL:B2]");
};
