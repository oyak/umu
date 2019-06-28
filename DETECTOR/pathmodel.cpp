#include "pathmodel.h"
#include "assert.h"

void PATHMODEL::deleteObjects()
{
    for(int ii = 0; ii < NumOfOrders; ++ii)
    {
        (_pObjectsArray[ii])->deleteObjects();
    }
    _pathStartCoordinate = 0;
    _pathEndCoordinate = 0;
}
//
// startCoord - начальная координата в мм
bool PATHMODEL::addObject(unsigned int id, int startCoord, unsigned int lenInSteps, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
    if ((_pObjectsArray[objectOrder])->addObject(id, startCoord, lenInSteps, objectOrder, pObject) == true)
    {
        if ((objectOrder == OverPlaced) || (objectOrder == ExpandedOverPlacing))
        {
            if (_pathStartCoordinate > startCoord)
            {
                _pathStartCoordinate = startCoord;
            }
            if (_pathEndCoordinate < startCoord + pObject->len() )
            {
                _pathEndCoordinate = startCoord + pObject->len();
            }
        }
        return true;
    }
    return false;
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
    _movingDirection = movingDirection;
}

Test::eMovingDir PATHMODEL::getMovingDireciton()
{
    return _movingDirection;
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

bool PATHMODEL::testPathMap(bool dbgOut=true)
{
bool res = true;
int coord;
int clearenceLen;
int clearenceStart;

    coord = _pathStartCoordinate;
    do
    {
        if (testCoordinate(coord, _pathEndCoordinate - coord, clearenceLen, clearenceStart) == true)
        {
            break;
        }
            else
            {
                res = false;
                emit message(QString::asprintf("testPathMap: clearence = %d on coordinate %d", clearenceLen, clearenceStart));
            }
    } while (coord <= _pathEndCoordinate);

    if ((res) && (dbgOut))
    {
        emit message("testPathMap: passed");
    }
    return res;
}

bool PATHMODEL::testCoordinate(int coordinate)
{
int shift;
int clearenceLen;
int coord;
bool res;
int clearenceStart;

    shift = _interObjectClearenceByMM + 1;
    if (getMovingDireciton() != Test::DirUpWard)
    {
        shift *= -1;
    }
    coord = coordinate;
    res = testCoordinate(coord, shift, clearenceLen, clearenceStart);
    if (!res)
    {
        emit message(QString::asprintf("testCoordinate: clearence at least %d mm on coordinate %d", clearenceLen, coordinate));
    }
    return res;
}

// ищет зазоры между объектами в списках ExpandedOverPlacing и OverPlaced в
// диапазоне кординат coordinate...coordinate + shift, мм
// если встретился недопустимый зазор, работа программы завершается на последней кординате зазора,
// возвращаем false, clearenceLen - его длина в мм, clearenceStartCoord - координата начала зазора,
// coordinate - последняя координата зазора
bool PATHMODEL::testCoordinate(int& coordinate, int shift, int& clearenceLen, int& clearenceStartCoord)
{
bool res = true;
bool isClearence = true;
bool isObject;
int currentCoord;

    assert(shift!= 0);

    clearenceLen = 0;
    currentCoord = coordinate;
    do
    {
        getObjectWnenTest(currentCoord, isObject);
        if (isObject)
        {
            if (isClearence)
            {
                isClearence = false;
                if (res == true) clearenceLen = 0;
                    else
                    {
                        coordinate = currentCoord; // встретился недопустимый зазор, clearenceLen - его длина
                        break;
                    }
            }
        }
            else
            {
                if (!isClearence)
                {
                    isClearence = true;
                    clearenceStartCoord = currentCoord;
                }
                    else
                    {
                        clearenceLen += 1;
                        if (clearenceLen > _interObjectClearenceByMM)
                        {
                            res = false;
                        }
                    }
            }
        if (shift > 0) currentCoord += 1;
            else currentCoord -= 1;
    } while ((shift > 0) && (currentCoord <= coordinate + shift) || (shift < 0) && (currentCoord >= coordinate + shift));
//
    if (res == true)
    {
        clearenceLen = 0;
        clearenceStartCoord = -1;
    }
    return res;
}

// подобна getObject(), но поиск осуществляется только в списках ExpandedOverPlacing и OverPlaced
SignalsData* PATHMODEL::getObjectWnenTest(unsigned int coord, bool& isDataObject)
{
SignalsData* res;

    res = (_pObjectsArray[ExpandedOverPlacing])->getObject(coord, isDataObject, false);
    if (isDataObject == false)
    {
        res = (_pObjectsArray[OverPlaced])->getObject(coord, isDataObject, false);
    }
    return res;
}

