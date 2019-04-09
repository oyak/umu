#include "CriticalSection_Lin.h"

cCriticalSection_Lin::cCriticalSection_Lin()
{
    pthread_mutex_init(&_mutex, NULL);
}

cCriticalSection_Lin::~cCriticalSection_Lin()
{
    pthread_mutex_destroy(&_mutex);
}

void cCriticalSection_Lin::Enter()
{
    pthread_mutex_lock(&_mutex);
}

void cCriticalSection_Lin::Release()
{
    pthread_mutex_unlock(&_mutex);
}
