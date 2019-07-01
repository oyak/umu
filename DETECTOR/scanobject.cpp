#include <QDebug>
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

    if (offset < (unsigned int)_data.size())
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

void SCANOBJECT::getAllChannels(QVector<CID>& result)
{
QVector<CID> current;
    for(QVector<SignalsData>::iterator it = _data.begin(); it != _data.end(); ++it )
    {
        if(it == _data.begin())
        {
            it->getAllChannels(result);
        }
            else
            {
                it->getAllChannels(current);
//                исключаем в current элементы, имеющиеся в result
                for(QVector<CID>::iterator itCID = result.begin(); itCID != result.end(); ++itCID )
                {
                    for(QVector<CID>::iterator itCID2 = current.begin(); itCID2 != current.end(); ++itCID2)
                    {
                        if( *itCID == *itCID2)
                        {
                            current.erase(itCID2);
                            break;
                        }
                    }
                }
                result += current;
            }
    }
}
// формирует сигнал с параметром QString для каждого элемента в _data для отрисовки в виде таблицы
// формат строки таблицы
// смещение   задерка/амплитуда(канал x1) задерка/амплитуда(канал x2) ... задерка/амплитуда(канал xn)
// x1, x2 ...xn - номера каналов
void SCANOBJECT::view()
{
QVector<CID> channels;
QVector<CID>::iterator it;
SignalsData* pSignalsData;
tSignalObject *pSignals;
QString s;
    s = QString::asprintf("Cnannel: \t\t");
    getAllChannels(channels);
    for(it = channels.begin(); it < channels.end(); ++it)
    {
        s += QString::asprintf("%d \t", *it);
    }

    emit message(s);
    emit message("Offset \t Delay/Amplitude ...");
    emit message("");
    for(int offset=0; offset < _data.size(); ++offset)
    {
        pSignalsData = data(offset);
        s = QString::asprintf("%d ", offset);
        emit message(s);
        if (pSignalsData)
        {
        unsigned int maxOfSignals = 0;
// определить максимальное число сигналов для всех каналов
            for(it = channels.begin(); it < channels.end(); ++it)
            {
                pSignalsData->getSignals(*it, &pSignals);
                if (pSignals)
                {
                    maxOfSignals = (maxOfSignals < pSignals->Count) ? pSignals->Count:maxOfSignals;
                }
            }
            for(unsigned int ii=0; ii<maxOfSignals; ++ii)
            {
                s.clear();
                s += QString::asprintf("\t");
                for(it = channels.begin(); it < channels.end(); ++it)
                {
                    pSignalsData->getSignals(*it, &pSignals);
                    if (pSignals)
                    {
                        if (ii < pSignals->Count)
                        {
                            s += QString::asprintf("\t%d/%d", pSignals->Signals[ii].Delay, pSignals->Signals[ii].Ampl);
                        }
                            else s += QString::asprintf("\t ----");
                    }
                        else
                        {
                            s += QString::asprintf("\t ----");
                        }
                }
                emit message(s);
            }
        }
    }
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

