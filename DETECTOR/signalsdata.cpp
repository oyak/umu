#include <assert.h>
#include <QDebug>
#include "signalsdata.h"

void SignalsData::clear()
{
    _signals.clear();
}
//
// channel - в терминах каналов дефектоскопа
// возвращает true если сигналы были добавлены для канала, т.е при
// добавлении их количество не могло превысить MaxSignalAtBlock
bool SignalsData::addSignals(CID channel, unsigned int signalCount, tDaCo_BScanSignalList *pSignalList)
{
unsigned int cnt;
    if (!_signals.contains(channel))
    {
    tSignalObject sO;
        sO.Count = 0;
        _signals.insert(channel, sO);
    }
    cnt = _signals[channel].Count;
    if ((cnt + signalCount) > MaxSignalAtBlock)
    {
        qDebug() << "addSignals could not add signals for channel" << channel;
        return false;
    }
    for (unsigned int ii=0; ii < signalCount; ++ii)
    {
     _signals[channel].Signals[cnt + ii].Delay = (*pSignalList)[ii].Delay;
     _signals[channel].Signals[cnt + ii].Ampl = (*pSignalList)[ii].Ampl;
/*
     qDebug() << "addSignals: addition - channel = " << channel << ", signal index = " << cnt+ii << \
                 ", Delay = " << (*pSignalList)[ii].Delay << ", Amplitude = " << (*pSignalList)[ii].Ampl;
*/
    }
    _signals[channel].Count += signalCount;
    return true;
}


void SignalsData::getSignals(CID channel, tSignalObject **pSignals)
{
   *pSignals = 0;
   if (_signals.contains(channel))
   {
       if (_signals[channel].Count != 0)
       {
           *pSignals = &_signals[channel];
       }
   }
}

void SignalsData::getNumOfSignals(CID channel, unsigned int *pSignalCount)
{
tSignalObject *pSignals;

    DEFCORE_ASSERT(pSignalCount != nullptr);
    *pSignalCount = 0;
    getSignals(channel, &pSignals);
    if (pSignals)
    {
        if (pSignals->Count != 0)
        {
            *pSignalCount = pSignals->Count;
        }
    }
}

// возвращает true, если данные, соответствующие signalIndex, извлечены
bool SignalsData::getSignalParamters(CID channel, unsigned char& amplitude, unsigned char& delay, unsigned int signalIndex)
{
tSignalObject *pSignals;
bool res = false;
    getSignals(channel, &pSignals);
    if (signalIndex < pSignals->Count)
    {
        delay = pSignals->Signals[signalIndex].Delay;
        amplitude = pSignals->Signals[signalIndex].Ampl;
        res = true;
    }
    return res;
}

bool SignalsData::addSignalToList(tDaCo_BScanSignalList *signalList, unsigned char amplitude, unsigned char delay, unsigned int signalIndex)
{
bool res = false;
    if (signalIndex < MaxSignalAtBlock)
    {
        (*signalList)[signalIndex].Delay = delay;
        (*signalList)[signalIndex].Ampl = amplitude;
        res = true;
    }
    return res;
}


void SignalsData::getAllChannels(QVector<CID>& channels)
{
QList<CID> keys;
        channels.clear();
        keys = _signals.keys();
        channels = keys.toVector();
}

