#include <assert.h>
#include <math.h>
#include <QDebug>
#include "emulator.h"
#include "ChannelsIds.h"
#include "variety.h"


EMULATOR::EMULATOR(cCriticalSection* cs1, cCriticalSection* cs2, float pathStep, QString pathToObjectsFiles)
{
    tStrokeConfig config;


    config.Stroke = 0;
    config.Line = ulRU1;
    _config.insert(F70E, config);
    //
    config.Line = ulRU2;
    _config.insert(B70E, config);
    //
    config.Stroke = 1;
    _config.insert(F42E, config);
    //
    config.Line = ulRU1;
    _config.insert(B42E, config);
    //
    config.Stroke = 2;
    _config.insert(F58ELU, config);
    //
    config.Line = ulRU2;
    _config.insert(F58ELW, config);
    //
    config.Stroke = 3;
    _config.insert(B58ELW, config);
    //
    config.Line = ulRU1;
    _config.insert(B58ELU, config);
    //
    config.Stroke = 4;
    _config.insert(N0EMS, config);

    _channelList = _config.keys();
    // таблица перевода кодирования дБ относительно порога в 32
    // отсчета  в амплитуду
    for (int ii = 0; ii < (int) sizeof(_ampl); ++ii) {
        float bell = (ii * 2 - 12.0) / 20.0;
        _ampl[ii] = (unsigned char) (pow(10.0, bell) * 32.0);
    }
    //
    //    QString pathToObjectsFiles = "files";
    _pFileParser = new Test(pathToObjectsFiles);

    _pPathModel[0] = new PATHMODEL(cs1, pathStep);
    _pPathModel[1] = new PATHMODEL(cs2, pathStep);

    _pStorage = new OBJECTSTOR(pathToObjectsFiles);

    connect(_pPathModel[usLeft], SIGNAL(message(QString)), this, SLOT(onLeftSideMessage(QString)));
    connect(_pPathModel[usRight], SIGNAL(message(QString)), this, SLOT(onRightSideMessage(QString)));
}

EMULATOR::~EMULATOR()
{
    disconnect(_pPathModel[usLeft], SIGNAL(messge(QString)), this, SLOT(onLeftSideMessage(QString)));
    disconnect(_pPathModel[usRight], SIGNAL(messge(QString)), this, SLOT(onRightSideMessage(QString)));
    _channelList.clear();
    _config.clear();
    deletePathObjects();
    delete _pPathModel[usLeft];
    delete _pPathModel[usRight];
}

void EMULATOR::getChannelList(QList<CID>& channelList)
{
    channelList = _channelList;
}

bool EMULATOR::onMessageId(unsigned short objectId, int startCoordInMM, eUMUSide side)
{
    bool res = false;

    assert((side == 0) || (side == 1));

    eOBJECT_ORDER order;
    unsigned int len;
    int N0EMSShift;
    switch(objectId)
    {
// превращаем код объекта (внутренний дефект) 201 в число 201..209, код 301 - в 301..306, код 551 - в 551...555, код 661 - в 661...665
        case 201:
        {
            objectId += getRandomNumber(0, 8);
            emit message(QString::asprintf("onMessageId: ObjectId 201 renumberred to %d", objectId));
            break;
        }
        case 301:
        {
            objectId += getRandomNumber(0, 5);
            emit message(QString::asprintf("onMessageId: ObjectId 301 renumberred to %d", objectId));
            break;
        }
        case 551:
        case 661:
        {
            objectId += getRandomNumber(0, 4);
            emit message(QString::asprintf("onMessageId: ObjectId (551 or 661) renumberred to %d", objectId));
            break;
        }
    default: break;
    }
    SCANOBJECT* pObject = _pStorage->extractObject(order, len, N0EMSShift, objectId);
    if (pObject) {
        if ((order == ExpandedOverPlacing) && (N0EMSShift > 0)) {
                startCoordInMM -= N0EMSShift;
        }
        res = _pPathModel[side]->addObject(objectId, startCoordInMM, len, order, pObject);
    }
    return res;
}

void EMULATOR::deletePathObjects()
{
    _pPathModel[0]->deleteObjects();
    _pPathModel[1]->deleteObjects();
}

SignalsData* EMULATOR::getScanObject(eUMUSide side, int coordInMM, bool& isDataObject)
{
    assert((side == 0) || (side == 1));
    return _pPathModel[side]->getObject(coordInMM, isDataObject);
}

void EMULATOR::setMovingDirection(Test::eMovingDir movingDirection)
{
    _pPathModel[0]->setMovingDireciton(movingDirection);
    _pPathModel[1]->setMovingDireciton(movingDirection);
}

void EMULATOR::onLeftSideMessage(QString s)
{
    emit message("left side: " + s);
}

void EMULATOR::onRightSideMessage(QString s)
{
    emit message("right side: " + s);
}

bool EMULATOR::testPathMap()
{
    return _pPathModel[0]->testPathMap(true) & _pPathModel[1]->testPathMap(true);
}

bool EMULATOR::testCoordinate(int coordinate)
{
    return _pPathModel[0]->testCoordinate(coordinate) & _pPathModel[1]->testCoordinate(coordinate);
}
