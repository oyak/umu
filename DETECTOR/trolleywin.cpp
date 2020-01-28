#include "trolleywin.h"

TROLLEY_WIN::TROLLEY_WIN()
{
    _pTrolley = new TROLLEY(&_cs);
}

TROLLEY_WIN::~TROLLEY_WIN()
{
    delete _pTrolley;
}
