//---------------------------------------------------------------------------

#ifndef cCriticalSection_WinH
#define cCriticalSection_WinH

#include <windows.h>
#include "CriticalSection.h"

//---------------------------------------------------------------------------

class cCriticalSection_Win: public cCriticalSection
{

private:

    CRITICAL_SECTION cs;

public:

    cCriticalSection_Win(void);
    ~cCriticalSection_Win(void);
    virtual void Enter(void);
    virtual void Release(void);

};

#endif
