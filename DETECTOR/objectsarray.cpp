#include <QDebug>
#include "objectsarray.h"

//условия включения нового элемента в _ObjectArray
//1. координаты нового элемента не пересекаются ни с одним элементом _ObjectArray
//2. или начальная координата нового элемента пересекается с конечной координатой последнего элементa _ObjectArray -
//производим усечение ранее включенного элемента

bool OBJECTSARRAY::addObject(unsigned int id, int startCoordInMM, unsigned int lenInMM, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
bool res = true;
QVector<tSCANOBJECT_EX>::iterator overCrossedElement;
QVector<tSCANOBJECT_EX>::iterator temp;
int endCoordInMM = startCoordInMM + (int)lenInMM;

    _cs->Enter();

    if(_objectArray.empty())
    {
        res = insertSCANOBJECT_EXBefore(_objectArray.end(), id, startCoordInMM, endCoordInMM, objectOrder, pObject);
        _cs->Release();
        return res;
    }
//
    overCrossedElement = findOverCrossCoordinate(startCoordInMM, endCoordInMM, _objectArray.begin());
    if (overCrossedElement == _objectArray.end())
    {// нет пересекающихся элементов с новым
        temp = findBiggerCoordinate(endCoordInMM);
        res = insertSCANOBJECT_EXBefore(temp, id, startCoordInMM, endCoordInMM, objectOrder, pObject);
    }
        else
        {
            temp =  overCrossedElement;
            ++temp;
            if (temp == _objectArray.end())
            {// пересекающийся элемент последний в списке
                if (((overCrossedElement->FirstCoordinate + 1) < startCoordInMM))
                { // производим усечение ранее всключенного объекта, если его начало находится раньше, чем startCoordInMM
                    overCrossedElement->LastCoordinate = startCoordInMM - 1;
                    res = insertSCANOBJECT_EXBefore(_objectArray.end(), id, startCoordInMM, endCoordInMM, objectOrder, pObject);
                }
                    else res = false;
            }
                else
                {// новый элемент добавить невозможно
                    res = false;
                }
        }
    _cs->Release();
    return res;
}
//
void OBJECTSARRAY::deleteObjects()
{
    _cs->Enter();
    _objectArray.clear();
    _currentObject = nullptr;
    _cs->Release();
}
//
// coord - коодината в мм
// если найдены сигналы, соответствующие координате - SignalsData, возвращается указатель на них и
SignalsData* OBJECTSARRAY::getObject(int coord, bool& isDataObject)
{
SignalsData* res = 0;
int seekDir = 0; // 0- не определено, -1 - к началу массива, 1 - к концу массива

    _cs->Enter();
    if (_objectArray.isEmpty())
    {
        isDataObject = false;
        _cs->Release();
        return 0;
    }
    do
    {
        if (_currentObject != nullptr)
        {
            if ((coord >= _currentObject->FirstCoordinate) && (coord <= _currentObject->LastCoordinate))
            {
                QVector<tSCANOBJECT_EX>::iterator it;
                isDataObject = true;
                unsigned int offsetInData;
                assert(_movingDirection != Test::DirNotDefined);
                if (_movingDirection == Test::DirUpWard)
                {
                    offsetInData = (coord - _currentObject->FirstCoordinate) * 100 / _currentObject->pScanObject->getPathStep();
                }
                    else
                    {// движение в сторону уменьшения координаты
                        offsetInData = (_currentObject->LastCoordinate - coord) * 100 / _currentObject->pScanObject->getPathStep();
                    }
                res = _currentObject->pScanObject->data(offsetInData);
                break;
            }
                else
                {
                    if (coord < _currentObject->FirstCoordinate)
                    {
                        if (_currentObject != _objectArray.begin() && (seekDir != 1))
                        {
                            seekDir = -1;
                            _currentObject--;
                        }
                            else
                            {
                                isDataObject = false;
                                _currentObject = nullptr;
                                break;
                            }
                    }
                        else if (_currentObject != _objectArray.end() && (seekDir != -1))
                             {
                                 seekDir = 1;
                                 _currentObject++;
                                 if (_currentObject == _objectArray.end())
                                 {
                                     isDataObject = false;
                                     _currentObject = nullptr;
                                     break;
                                 }
                             }
                                 else
                                 {
                                     isDataObject = false;
                                     _currentObject = nullptr;
                                     break;
                                 }
                }
        }
            else
            {
                seekDir = 1;
                _currentObject = _objectArray.begin();
            }

    } while(true);


    if (_currentObject != _lastObject)
    {
        if (_currentObject == nullptr)
        {
            qWarning() << "coord = " << coord  << "currentObject == null";
        }
            else
            {
                qWarning() << "coord = " << coord  << "currentObject Id == " << _currentObject->Id;
            }
        _lastObject = _currentObject;
    }


    _cs->Release();
    return res;
}

// ищет элемент с пересекающимися координатами
QVector<tSCANOBJECT_EX>::iterator OBJECTSARRAY::findOverCrossCoordinate(int startCoord, int endCoord, QVector<tSCANOBJECT_EX>::iterator startElement)
{
QVector<tSCANOBJECT_EX>::iterator res;
    res = startElement;
    do
    {
        if ( !((endCoord < res->FirstCoordinate) || (startCoord > res->LastCoordinate)) ) break;
        res++;
    }
    while(res != _objectArray.end());
    return res;
}

// ищет первый элемент с большими координатами, при условии, что нет элементов с пересекающимися координатами
QVector<tSCANOBJECT_EX>::iterator OBJECTSARRAY::findBiggerCoordinate(int endCoord)
{
QVector<tSCANOBJECT_EX>::iterator res;
    res = _objectArray.begin();
    do
    {
        if  (endCoord < res->FirstCoordinate) break;
        res++;
    }
    while(res != _objectArray.end());
    return res;
}


bool OBJECTSARRAY::insertSCANOBJECT_EXBefore(QVector<tSCANOBJECT_EX>::iterator whereToIt, unsigned int id, int firstCoord, int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
tSCANOBJECT_EX temp;
    temp.FirstCoordinate = firstCoord;
    temp.LastCoordinate = lastCoord;
    temp.ObjectOrder = objectOrder;
    temp.pScanObject = pObject;
    temp.Id = id;
    temp.Size = temp.LastCoordinate - temp.FirstCoordinate;
    _objectArray.insert(whereToIt, temp);
    return true;
}

unsigned int OBJECTSARRAY::convertMMtoPathStep(unsigned int pathInMM)
{
    return static_cast<unsigned int>(pathInMM / _step);
}

void OBJECTSARRAY::setMovingDirection(Test::eMovingDir movingDirection)
{
    _movingDirection = movingDirection;
}

