#ifndef DEVICE_EMULATION
#include "values.h"
#else
#include "umudevice.h"
#endif

// gets ammount of ticks passed till timestartick
int get_tickdur(int timestartick)
{
register int ii;

   ii = UMU::xTaskGetTickCount() - timestartick;
   return  (ii >= 0) ? ii:(0-ii);
}
//------------------------------------------------------------------------
// caluculates  ammount of ticks passed till timestartick untill timeendtick
int calc_tickdur(int timestartick, int timeendtick)
{
register int ii;

   ii = timeendtick - timestartick;
   return  (ii >= 0) ? ii:(0-ii);
}
