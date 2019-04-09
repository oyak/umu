#include "TickCount.h"
#include "platforms.h"

#if defined(DEFCORE_CC_BOR)
#include "windows.h";
#elif defined(DEFCORE_CC_GNU)
#include <sys/time.h>
#include <cstddef>
#endif

unsigned long GetTickCount_()
{
#ifdef DEFCORE_CC_BOR
    return GetTickCount();
#elif defined(DEFCORE_CC_GNU)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#else
#error "Unsupported platform"
#endif
}

#if defined(DEFCORE_CC_BOR)
#pragma package(smart_init)
#endif
