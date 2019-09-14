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
    _config[usLeft].insert(F70E, config);
    _config[usRight].insert(F70E, config);
    //
    config.Line = ulRU2;
    _config[usLeft].insert(B70E, config);
    _config[usRight].insert(B70E, config);
    //
    config.Stroke = 1;
    _config[usLeft].insert(F42E, config);
    _config[usRight].insert(F42E, config);
    //
    config.Line = ulRU1;
    _config[usLeft].insert(B42E, config);
    _config[usRight].insert(B42E, config);
    //
    config.Stroke = 2;
    _config[usLeft].insert(F58ELU, config);
    config.Line = ulRU2;
    _config[usRight].insert(F58ELU, config);
    //
    _config[usLeft].insert(F58ELW, config);
    config.Line = ulRU1;
    _config[usRight].insert(F58ELW, config);
    //
    config.Stroke = 3;
    _config[usLeft].insert(B58ELU, config);
    _config[usRight].insert(B58ELW, config);
    config.Line = ulRU2;
    _config[usLeft].insert(B58ELW, config);
    _config[usRight].insert(B58ELU, config);
    //
    config.Stroke = 4;
    config.Line = ulRU1;
    _config[usLeft].insert(N0EMS, config);
    _config[usRight].insert(N0EMS, config);

    _channelList = _config[usLeft].keys();
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

    _pStorage = new OBJECTSTOR();

    connect(_pPathModel[usLeft], SIGNAL(message(QString)), this, SLOT(onLeftSideMessage(QString)));
    connect(_pPathModel[usRight], SIGNAL(message(QString)), this, SLOT(onRightSideMessage(QString)));
}

EMULATOR::~EMULATOR()
{
    disconnect(_pPathModel[usLeft], SIGNAL(messge(QString)), this, SLOT(onLeftSideMessage(QString)));
    disconnect(_pPathModel[usRight], SIGNAL(messge(QString)), this, SLOT(onRightSideMessage(QString)));
    _channelList.clear();
    _config[usLeft].clear();
    _config[usRight].clear();
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

// превращаем код объекта (внутренний дефект) 201..209 в случайное число из этого же диапазона
// и тоже самое для дефектов 301..306, 531...533, 551...555, 661...665
        if ((objectId >= 201) && (objectId <= 209)) {
            objectId =  201 + getRandomNumber(0, 8);
            emit message(QString::asprintf("onMessageId: ObjectId 201 renumberred to %d", objectId));
        }
            else {
                if ((objectId >= 301) && (objectId <= 306)) {
                objectId = 301 + getRandomNumber(0, 5);
                emit message(QString::asprintf("onMessageId: ObjectId 301 renumberred to %d", objectId));
                }
                    else {
                        if ((objectId >= 531) && (objectId <= 533)) {
                        objectId = 531 + getRandomNumber(0, 2);
                        emit message(QString::asprintf("onMessageId: ObjectId 531 renumberred to %d", objectId));
                        }
                            else {
                                if ((objectId >= 551) && (objectId <= 555)) {
                                    objectId = 551 + getRandomNumber(0, 4);
                                    emit message(QString::asprintf("onMessageId: ObjectId 551 renumberred to %d", objectId));
                                }
                                    else {
                                        if ((objectId >= 661) && (objectId <= 665)) {
                                            objectId = 661 + getRandomNumber(0, 4);
                                            emit message(QString::asprintf("onMessageId: ObjectId 661 renumberred to %d", objectId));
                                        }
                                    }
                            }
                  }
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
