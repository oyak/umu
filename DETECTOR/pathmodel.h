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

};

#endif // PATHMODEL_H
