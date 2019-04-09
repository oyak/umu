#include "QDebug"
#include "pathmodel.h"


//условия включения нового элемента в _ObjectArray
//1. координаты нового элемента не пересекаются ни с одним элементом _ObjectArray
//2. или координаты нового элемента пересекаются только с одним элементом _ObjectArray
//3. если координаты нового элемента пересекаются с StartCoordinate  и EndCoordinate к.л. элемента _ObjectArray
//то новый элемент может быть добавлен в ComplexObject, если там единственный элемент, при этом
//новый элемент и имеющийся должны иметь разные значения поля ObjectOrder, т.е один должен быть
//перекрываемым, другой - перекрывающим

bool PATHMODEL::addObject(unsigned int id, unsigned int startCoordInMM, unsigned int lenInMM, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
bool res = true;
QVector<tCOMPLEXOBJECT>::iterator overCrossedElement;
QVector<tCOMPLEXOBJECT>::iterator temp;
unsigned int endCoordInMM = startCoordInMM + lenInMM;

    _cs->Enter();

    if(_objectArray.empty())
    {
        res = insertComplexObjectBefore(_objectArray.end(), id, startCoordInMM, endCoordInMM, objectOrder, pObject);
        _cs->Release();
        return res;
    }
//
    overCrossedElement = findOverCrossCoordinate(startCoordInMM, endCoordInMM, _objectArray.begin());
    if (overCrossedElement == _objectArray.end())
    {// нет пересекающихся элементов с новым
        temp = findBiggerCoordinate(endCoordInMM);
        res = insertComplexObjectBefore(temp, id, startCoordInMM, endCoordInMM, objectOrder, pObject);
    }
        else
        {
            temp =  overCrossedElement;
            ++temp;
            if ((temp == _objectArray.end()) || (findOverCrossCoordinate(startCoordInMM, endCoordInMM, temp) == _objectArray.end()))
            {// пересекающийся элемент только один
                res = addExObject(overCrossedElement, id, startCoordInMM, endCoordInMM, objectOrder, pObject);
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
void PATHMODEL::deleteObjects()
{
    _cs->Enter();
    _objectArray.clear();
    _currentObject = nullptr;
    _cs->Release();
}
//
// coord - коодината в мм
// если найдены сигналы, соответствующие координате - SignalsData, возвращается указатель на них и
SignalsData* PATHMODEL::getObject(unsigned int coord, bool& isDataObject)
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
            if ((coord >= _currentObject->StartCoordinate) && (coord <= _currentObject->EndCoordinate))
            {
                QVector<tSCANOBJECT_EX>::iterator it;
                isDataObject = true;
                unsigned int offsetInData;

                assert(!_currentObject->ExObjectsArray.isEmpty());
                it = _currentObject->ExObjectsArray.begin();

                if (_currentObject->ExObjectsArray.size() != 1)
                {
                    if ( (coord >= _currentObject->ExObjectsArray[1].FirstCoordinate) && (coord <= _currentObject->ExObjectsArray[1].LastCoordinate) )
                    {
                        ++it;
                    }
                }
                offsetInData = (coord - it->FirstCoordinate) * 100 / it->pScanObject->getPathStep();
                res = it->pScanObject->data(offsetInData);
                break;
            }
                else
                {
                    if (coord < _currentObject->StartCoordinate)
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
            qDebug() << "coord = " << coord  << "currentObject == null";
        }
            else
            {
                qDebug() << "coord = " << coord  << "currentObject Id == " << (*_currentObject).ExObjectsArray[0].Id;
            }
        _lastObject = _currentObject;
    }


    _cs->Release();
    return res;
}

// ищет элемент с пересекающимися координатами
QVector<tCOMPLEXOBJECT>::iterator PATHMODEL::findOverCrossCoordinate(unsigned int startCoord, unsigned int endCoord, QVector<tCOMPLEXOBJECT>::iterator startElement)
{
QVector<tCOMPLEXOBJECT>::iterator res;
    res = startElement;
    do
    {
        if ( !((endCoord < res->StartCoordinate) || (startCoord > res->EndCoordinate)) ) break;
        res++;
    }
    while(res != _objectArray.end());
    return res;
}

// ищет первый элемент с большими координатами, при условии, что нет элементов с пересекающимися координатами
QVector<tCOMPLEXOBJECT>::iterator PATHMODEL::findBiggerCoordinate(unsigned int endCoord)
{
QVector<tCOMPLEXOBJECT>::iterator res;
    res = _objectArray.begin();
    do
    {
        if  (endCoord < res->StartCoordinate) break;
        res++;
    }
    while(res != _objectArray.end());
    return res;
}

// вставляет object в элемент, на который указывает итератор
// при успехе возвращаем true
bool PATHMODEL::addExObject(QVector<tCOMPLEXOBJECT>::iterator whereToIt, unsigned int id, unsigned int firstCoord, unsigned int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
tSCANOBJECT_EX temp;
unsigned int initalSize = whereToIt->ExObjectsArray.size();

    assert(initalSize <= whereToIt->ExObjectsArrayMaxSize);
    if (initalSize == whereToIt->ExObjectsArrayMaxSize)
    {
        return false;
    }
    if (initalSize != 0)
    {// единственный элемент
        if (whereToIt->ExObjectsArray[0].ObjectOrder == objectOrder)
        {
            return false;
        }
    }
    temp.Id = id;
    temp.FirstCoordinate = firstCoord;
    temp.LastCoordinate = lastCoord;
    temp.ObjectOrder = objectOrder;
    temp.pScanObject = pObject;
    if ( (initalSize != 0) && (whereToIt->ExObjectsArray[0].ObjectOrder != whereToIt->OverPlacedElement) )
    {// единственный элемент является перекрывающим, новый элемент вставляем перед ним
        whereToIt->ExObjectsArray.push_front(temp);
    }
        else
        {
            whereToIt->ExObjectsArray.push_back(temp);
        }
    if (initalSize != 0)
    {
        if (firstCoord < whereToIt->StartCoordinate)
        {
            whereToIt->StartCoordinate = firstCoord;
        }
        if (lastCoord > whereToIt->EndCoordinate)
        {
            whereToIt->EndCoordinate = lastCoord;
        }
    }
        else
        {
            whereToIt->StartCoordinate = firstCoord;
            whereToIt->EndCoordinate = lastCoord;
        }
    return true;
}

bool PATHMODEL::insertComplexObjectBefore(QVector<tCOMPLEXOBJECT>::iterator whereToIt, unsigned int id, unsigned int firstCoord, unsigned int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
tCOMPLEXOBJECT temp;
    temp.StartCoordinate = 0;
    temp.EndCoordinate = 0;
    return addExObject(_objectArray.insert(whereToIt, temp), id, firstCoord, lastCoord, objectOrder, pObject);
}

unsigned int PATHMODEL::convertMMtoPathStep(unsigned int pathInMM)
{
    return static_cast<unsigned int>(pathInMM / _step);
}

