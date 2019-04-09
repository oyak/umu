
#include "unitlin.h"


UNITLIN::UNITLIN()
{
    _pDevice = new UMUDEVICE(&_thList, (void*)this);
     connect(_pDevice, SIGNAL(CDUconnected()), this, SLOT(on_CDU_connected()));
}

UNITLIN::~UNITLIN()
{
QVector <cCriticalSection_Lin*>::iterator it;
    delete _pDevice;
    for (it = _cs.begin(); it < _cs.end(); ++it) delete *it;
    _cs.clear();
}

void UNITLIN::start()
{
    _pDevice->start();
}

void UNITLIN::stop()
{
    _pDevice->stop();
}

cCriticalSection_Lin *UNITLIN::createCriticalSection()
{
cCriticalSection_Lin *cs;
    vectorCs.Enter();
    cs = new cCriticalSection_Lin;
    _cs.push_back(cs);
    vectorCs.Release();
    return cs;
}

void UNITLIN::criticalSectionEnter(cCriticalSection_Lin* cs)
{
    cs->Enter();
}

void UNITLIN::criticalSectionRelease(cCriticalSection_Lin *cs)
{
    cs->Release();
}

void UNITLIN::on_CDU_connected()
{
    emit CDUconnected();
}


// для отладки
void UNITLIN::_onPathStep(int shift, unsigned int coordInSteps)
{
    _pDevice->_onPathStep(shift, coordInSteps);
}

void UNITLIN::printConnectionStatus()
{
    _pDevice->printConnectionStatus();
}
