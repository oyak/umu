#include "trolleylin.h"

TROLLEY_LIN::TROLLEY_LIN()
{
    _pTrolley = new TROLLEY(&_cs);
}

TROLLEY_LIN::~TROLLEY_LIN()
{
    delete _pTrolley;
}
