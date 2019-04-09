#ifndef CriticalSectionH
#define CriticalSectionH

class cCriticalSection
{
public:
    virtual ~cCriticalSection() {}
    virtual void Enter(void) = 0;
    virtual void Release(void) = 0;
};

#endif
