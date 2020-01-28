#include "CriticalSection_Win.h"

cCriticalSection_Win::cCriticalSection_Win(void)
{
    InitializeCriticalSection(&cs);
}

cCriticalSection_Win::~cCriticalSection_Win(void)
{
    DeleteCriticalSection(&cs);
}

void cCriticalSection_Win::Enter(void)
{
    EnterCriticalSection(&cs);
}

void cCriticalSection_Win::Release(void)
{
    LeaveCriticalSection(&cs);
}
