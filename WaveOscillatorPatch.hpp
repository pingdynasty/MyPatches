
/*
 * In the same vein: a family of smoothed sawtooth waves
 * f(x) = x - x^a
 * changing this to :
 * f(x) = x - sgn(x)*abs(x)^a
 * allows 'a' to be continuously variable, not just an odd integer.
 * sgn(x) is the signum function.
 * f(x) = x - sgn(x)*abs(x)^a
 */
float f(float x, float a){
  return x - copysignf(powf(abs(x), a), x);
}
// in the range -1<=x<1, 2<=a
// for a=2 this produces amplitude 0.25, a=4.5 amplitude 0.5, a=100 amplitude 0.945

float f(float x, float a){
  return x * (1 - powf(abs(x), a));
}

