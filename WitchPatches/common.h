#define PATCH_EXPRESSION

#ifdef PATCH_EXPRESSION
#define USE_MPE
#warning Building Expression patch
#else
#warning Building Performance patch
#endif

#if VOICES == 1
#define GAINFACTOR 1
#elif VOICES == 2
#define GAINFACTOR 0.70
#elif VOICES <= 4
#define GAINFACTOR 0.5
#elif VOICES <= 6
#define GAINFACTOR 0.4
#elif VOICES <= 8
#define GAINFACTOR 0.35
#else
#define GAINFACTOR 0.25
#endif

#define BUTTON_VELOCITY 100

#define PARAMETER_FX_AMOUNT               PARAMETER_E
#define PARAMETER_FX_SELECT               PARAMETER_H
#define PARAMETER_ATTACK                  PARAMETER_AA
#define PARAMETER_DECAY                   PARAMETER_AB
#define PARAMETER_SUSTAIN                 PARAMETER_AC
#define PARAMETER_RELEASE                 PARAMETER_AD
#define PARAMETER_EXTL_AMOUNT             PARAMETER_AE
#define PARAMETER_EXTR_AMOUNT             PARAMETER_AF

#define PATCH_PARAMETER_ATTENUATE_A PATCH_PARAMETER_BA
#define PATCH_PARAMETER_ATTENUATE_B PATCH_PARAMETER_BB
#define PATCH_PARAMETER_ATTENUATE_C PATCH_PARAMETER_BC
#define PATCH_PARAMETER_ATTENUATE_D PATCH_PARAMETER_BD

#define PATCH_PARAMETER_LFO1_SHAPE  PATCH_PARAMETER_AG
#define PATCH_PARAMETER_LFO2_SHAPE  PATCH_PARAMETER_AH

#define PARAMETER_WAVESHAPE               PARAMETER_AG
#define PARAMETER_STEREO_MIX              PARAMETER_AG

#define PATCH_PARAMETER_MODULATION_AMOUNT 
#define PATCH_PARAMETER_PRESSURE_AMOUNT
