#ifndef TROLLEYWIN_H
#define TROLLEYWIN_H

#include "trolley.h"
#include "CriticalSection_Win.h"

class TROLLEY_WIN
{
public:
    TROLLEY_WIN();
    ~TROLLEY_WIN();

private:
    cCriticalSection_Win _cs;
    TROLLEY *_pTrolley;

};

#endif // TROLLEYWIN_H
