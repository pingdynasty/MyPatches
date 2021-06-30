#define PATCH_EXPRESSION

#ifdef PATCH_EXPRESSION
#define USE_MPE
#warning Building Expression patch
#else
#warning Building Performance patch
#endif

