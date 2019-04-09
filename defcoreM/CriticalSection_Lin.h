#ifndef CRITICALSECTION_LIN_H
#define CRITICALSECTION_LIN_H

#include <pthread.h>

#include "CriticalSection.h"

class cCriticalSection_Lin : public cCriticalSection
{
public:
    cCriticalSection_Lin();
    ~cCriticalSection_Lin();

    void Enter();
    void Release();

private:
    pthread_mutex_t _mutex;
};

#endif  // CRITICALSECTION_LIN_H
