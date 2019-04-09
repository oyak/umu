#include "scanobject.h"

SCANOBJECT::SCANOBJECT(): _step(183)
{}

SCANOBJECT::~SCANOBJECT()
{
    clear();
}

void SCANOBJECT::add(SignalsData* pData)
{
    _data.push_back(*pData);
}

SignalsData* SCANOBJECT::data(unsigned int offset)
{
SignalsData* dataPtr = nullptr;

    if (offset < _data.size())
        dataPtr = &_data[offset];
    return dataPtr;
}

unsigned int SCANOBJECT::len()
{
    return _data.size() * _step / 100;
}

unsigned int SCANOBJECT::size()
{
    return _data.size();
}


bool SCANOBJECT::isEmpty()
{
    return _data.isEmpty();
}


void SCANOBJECT::clear()
{
    for(QVector<SignalsData>::iterator it = _data.begin(); it != _data.end(); ++it )
    {
        it->clear();
    }
}

void SCANOBJECT::setPathStep(unsigned int step)
{
    _step = step;
}
unsigned int SCANOBJECT::getPathStep()
{
    return _step;
}
