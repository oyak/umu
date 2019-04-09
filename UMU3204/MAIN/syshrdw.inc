#ifndef syshrdwinc__
#define syshrdwinc__


#define SYSFREQ 72
#define OSCFREQ 12  // 6
//
#define SDRAMCS0BASE 0xA0000000
//
//
#define FCCO 288
//
#define CCLKCFG_Val (FCCO/SYSFREQ - 1)
#if ((CCLKCFG_Val & 1) == 0)
#error syshrdw.inc: CCLKCFG_Val must be odd value
#endif
//
#define PLLCFG_Val (FCCO/2/OSCFREQ - 1)

#endif
