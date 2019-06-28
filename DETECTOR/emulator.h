#ifndef EMULATOR_H
#define EMULATOR_H

#include <QMap>
#include <QObject>
#include <QList>
#include "pathmodel.h"
//#include "objectlib.h"
#include "objectstor.h"
#include "test.h"

typedef struct
{
eUMULine Line;
unsigned int Stroke;
} tStrokeConfig;


class EMULATOR: public QObject
{
    Q_OBJECT

public:
    EMULATOR(cCriticalSection *cs1, cCriticalSection *cs2, float pathStep, QString pathToObjectsFiles);
// cs1, cs2 критические секции для объектов *pPathodel[]
    ~EMULATOR();
    unsigned char codeToAmpl(unsigned int code)
    {
        return _ampl[code & 0xF];
    }
//
    tStrokeConfig CIDToLineAndStroke(CID channel)
    {
        assert(_config.contains(channel));
        return _config[channel];
    }

    SignalsData* getScanObject(eUMUSide side, int coord, bool &isDataObject);

    void getChannelList(QList<CID>& channelList);

    bool onMessageId(unsigned short objectId, int startCoord, eUMUSide side);
    void deletePathObjects();
    void setMovingDirection(Test::eMovingDir movingDirection);
    bool testCoordinate(int coordinate);
    bool testPathMap();

signals:
    void message(QString s);
//
public slots:
    void onLeftSideMessage(QString s);
    void onRightSideMessage(QString s);
//

private:
    PATHMODEL *_pPathModel[2]; // на две нити usLeft, usRight

    OBJECTSTOR *_pStorage;
    Test *_pFileParser;
    QMap<CID, tStrokeConfig> _config;
    QList <CID> _channelList;
    unsigned char _ampl[16];
};


#endif // EMULATOR_H
