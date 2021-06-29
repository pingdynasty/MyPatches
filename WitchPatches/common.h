#define PATCH_PERFORMANCE

#ifdef PATCH_PERFORMANCE
#define USE_MPE
#warning Building Expression patch
#else
#warning Building Performance patch
#endif

