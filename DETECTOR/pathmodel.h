#ifndef PATHMODEL_H
#define PATHMODEL_H


//#include "platforms.h"
#include "CriticalSection.h"
#include "objectsarray.h"

class cCriticalSection;

class PATHMODEL{
//
public:
//
    PATHMODEL(cCriticalSection* cs, float pathStep)
    {
        for(int ii = 0; ii < NumOfOrders; ++ii)
        {
            _pObjectsArray[ii] = new OBJECTSARRAY(cs, pathStep);
        }
    }
//
    ~PATHMODEL()
    {
        for(int ii = 0; ii < NumOfOrders; ++ii)
        {
            delete _pObjectsArray[ii];
        }
    }
//
    void deleteObjects();
    bool addObject(unsigned int id, unsigned int startCoord, unsigned int lenInSteps, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
    SignalsData *getObject(unsigned int coord, bool &isDataObject);
//
private:
    OBJECTSARRAY *_pObjectsArray[NumOfOrders];
};

#endif // PATHMODEL_H
