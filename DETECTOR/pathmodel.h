#ifndef PATHMODEL_H
#define PATHMODEL_H


//#include "platforms.h"
#include "CriticalSection.h"
#include "objectsarray.h"

class cCriticalSection;

class PATHMODEL:public QObject
{
    Q_OBJECT
public:
//
    PATHMODEL(cCriticalSection* cs, float pathStep)
    {
        for(int ii = 0; ii < NumOfOrders; ++ii)
        {
            _pObjectsArray[ii] = new OBJECTSARRAY(cs, pathStep);
            switch(ii)
            {
                case OverPlacing:
                    connect(_pObjectsArray[ii], SIGNAL(message(QString)), this, SLOT(onMessageOverPlacing(QString)));
                    break;
                case ExpandedOverPlacing:
                    connect(_pObjectsArray[ii], SIGNAL(message(QString)), this, SLOT(onMessageExpandedOverPlacing(QString)));
                    break;
                default:
                    connect(_pObjectsArray[ii], SIGNAL(message(QString)), this, SLOT(onMessageOverPlaced(QString)));
                    break;
            }
        }
        _pathStartCoordinate = 0;
        _pathEndCoordinate = 0;
        _interObjectClearenceByMM = 4;
    }
//
    ~PATHMODEL()
    {
        for(int ii = 0; ii < NumOfOrders; ++ii)
        {
//            disconnect(_pObjectsArray[ii], SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
            disconnect(_pObjectsArray[ii], SIGNAL(message(QString)));
            delete _pObjectsArray[ii];
        }
    }
//
    void deleteObjects();
    bool addObject(unsigned int id, int startCoord, unsigned int lenInSteps, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
    SignalsData *getObject(unsigned int coord, bool &isDataObject);
    void setMovingDireciton(Test::eMovingDir movingDirection);
    Test::eMovingDir getMovingDireciton();
    bool testCoordinate(int coordinate);
    bool testPathMap(bool dbgOut);
//
signals:
    void message(QString s);
//
public slots:
    void onMessageOverPlacing(QString s);
    void onMessageExpandedOverPlacing(QString s);
    void onMessageOverPlaced(QString s);
//
private:
    OBJECTSARRAY *_pObjectsArray[NumOfOrders];
    Test::eMovingDir _movingDirection;
    int _pathStartCoordinate; // начальная координата самого первого объекта пути в массивах ExpandedOverPlacing и OverPlaced
    int _pathEndCoordinate; // конечная координата самого последнего объекта пути в массивах ExpandedOverPlacing и OverPlaced
    int _interObjectClearenceByMM; // допустимый "зазор" - дырка между объектами в пути, мм

    SignalsData* getObjectWnenTest(unsigned int coord, bool& isDataObject);
    bool testCoordinate(int& coordinate, int shift, int& clearenceLen, int& clearenceStartCoord);
};

#endif // PATHMODEL_H
