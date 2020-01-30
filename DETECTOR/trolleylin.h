#ifndef TROLLEYLIN_H
#define TROLLEYLIN_H

#include "trolley.h"
#include "CriticalSection_Lin.h"

class TROLLEY_LIN
{
public:
    TROLLEY_LIN();
    ~TROLLEY_LIN();

private:
    cCriticalSection_Lin _cs;
    TROLLEY *_pTrolley;

};

#endif // TROLLEYLIN_H
