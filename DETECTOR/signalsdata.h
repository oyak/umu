#ifndef SIGNALDATA_H
#define SIGNALDATA_H
#include <QMap>
#include "dc_definitions.h"

typedef struct
{
    unsigned int Count;
    tDaCo_BScanSignalList Signals;
} tSignalObject;

typedef QMap <CID, tSignalObject> tSignalsMap;

class SignalsData
{
public:
//
    SignalsData(){};

    ~SignalsData(){ clear();}

    void clear();
    bool addSignals(CID channel, unsigned int signalCount, tDaCo_BScanSignalList *pSignalList);
    void getSignals(CID channel, tSignalObject **pSignals);
    void getNumOfSignals(CID channel, unsigned int *pSignalCount);

    void getAllChannels(QVector<CID>& channels);
    bool getSignalParamters(CID channel, unsigned char& amplitude, unsigned char& delay, unsigned int signalIndex);
    bool addSignalToList(tDaCo_BScanSignalList *signalList, unsigned char amplitude, unsigned char delay, unsigned int signalIndex);

    //    SignalsData& operator=(SignalsData& s);
private:
    tSignalsMap _signals;
};


#endif // SIGNALDATA_H
