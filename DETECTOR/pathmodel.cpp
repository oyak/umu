#include "pathmodel.h"

void PATHMODEL::deleteObjects()
{
    for(int ii = 0; ii < NumOfOrders; ++ii)
    {
        (_pObjectsArray[ii])->deleteObjects();
    }
}
//
bool PATHMODEL::addObject(unsigned int id, int startCoord, unsigned int lenInSteps, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
    (_pObjectsArray[objectOrder])->addObject(id, startCoord, lenInSteps, objectOrder, pObject);
}
//
// coord - коодината в мм
// если найдены сигналы, соответствующие координате - SignalsData, возвращается указатель на них
// isDataObject == true, если найден объект, соответствующий координате
SignalsData* PATHMODEL::getObject(unsigned int coord, bool& isDataObject)
{
SignalsData* res;

    res = (_pObjectsArray[OverPlacing])->getObject(coord, isDataObject);
    if (isDataObject == false)
    {
        res = (_pObjectsArray[ExpandedOverPlacing])->getObject(coord, isDataObject);
    }
    if (isDataObject == false)
    {
        res = (_pObjectsArray[OverPlaced])->getObject(coord, isDataObject);
    }
    return res;
}

void PATHMODEL::setMovingDireciton(Test::eMovingDir movingDirection)
{
    for(int ii = 0; ii < NumOfOrders; ++ii)
    {
        (_pObjectsArray[ii])->setMovingDirection(movingDirection);
    }
}

void PATHMODEL::onMessageOverPlacing(QString s)
{
    emit message(s + " in OverPalacing list");
}

void PATHMODEL::onMessageExpandedOverPlacing(QString s)
{
    emit message(s + " in ExpandedOverPalacing list");
}

void PATHMODEL::onMessageOverPlaced(QString s)
{
    emit message(s + " in OverPalaced list");
}
