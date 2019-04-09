#include <cstdlib>

#include "platforms.h"

#if defined(DEFCORE_CC_GNU)
#include <sys/time.h>
#endif

#include <iostream>
#include <cassert>

TDateTime CurrentDateTime()
{
#if defined(DEFCORE_CC_BOR)
    TDateTime dt;
    return dt.CurrentDateTime();
#elif defined(DEFCORE_CC_GNU)
    // TDateTime представляет собой число с плавающей точкой,
    // где целая часть — число дней с 30 декабря 1899 года до текущего дня, а дробная часть — число секунд от начала дня.
    // 25569.16666 — это 1 января 1970 года в формате TDateTime.
    // 86400 — количество секунд в одном дне.

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((static_cast<double>(tv.tv_sec) / 86400.) + 25569.16666);
#else
#error "function CurrentDateTime() doesn't implemented for this platform!";
#endif
}

void defcore_assert(const char* assertion, const char* file, int line)
{
    std::cerr << "ASSERT: \"" << assertion << "\" in file:" << file << ", line:" << line << std::endl;
    assert(false);
}
