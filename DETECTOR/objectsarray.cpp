#include <QDebug>
#include "objectsarray.h"

//������� ��������� ������ �������� � _ObjectArray
//1. ���������� ������ �������� �� ������������ �� � ����� ��������� _ObjectArray
//2. ��� ���������� ������ �������� ������������ ������ � ����� ��������� _ObjectArray

bool OBJECTSARRAY::addObject(unsigned int id, unsigned int startCoordInMM, unsigned int lenInMM, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{
bool res = true;
QVector<tSCANOBJECT_EX>::iterator overCrossedElement;
QVector<tSCANOBJECT_EX>::iterator temp;
unsigned int endCoordInMM = startCoordInMM + lenInMM;

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
    {// ��� �������������� ��������� � �����
        temp = findBiggerCoordinate(endCoordInMM);
        res = insertSCANOBJECT_EXBefore(temp, id, startCoordInMM, endCoordInMM, objectOrder, pObject);
    }
        else
        {
            temp =  overCrossedElement;
            ++temp;
            if ((temp == _objectArray.end()) || (findOverCrossCoordinate(startCoordInMM, endCoordInMM, temp) == _objectArray.end()))
            {// �������������� ������� ������ ����
                // �������� ����������
                    res = false;
            }
                else
                {// ����� ������� �������� ����������
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
// coord - ��������� � ��
// ���� ������� �������, ��������������� ���������� - SignalsData, ������������ ��������� �� ��� �
SignalsData* OBJECTSARRAY::getObject(unsigned int coord, bool& isDataObject)
{
SignalsData* res = 0;
int seekDir = 0; // 0- �� ����������, -1 - � ������ �������, 1 - � ����� �������

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
                offsetInData = (coord - _currentObject->FirstCoordinate) * 100 / _currentObject->pScanObject->getPathStep();
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
            qDebug() << "coord = " << coord  << "currentObject == null";
        }
            else
            {
                qDebug() << "coord = " << coord  << "currentObject Id == " << _currentObject->Id;
            }
        _lastObject = _currentObject;
    }


    _cs->Release();
    return res;
}

// ���� ������� � ��������������� ������������
QVector<tSCANOBJECT_EX>::iterator OBJECTSARRAY::findOverCrossCoordinate(unsigned int startCoord, unsigned int endCoord, QVector<tSCANOBJECT_EX>::iterator startElement)
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

// ���� ������ ������� � �������� ������������, ��� �������, ��� ��� ��������� � ��������������� ������������
QVector<tSCANOBJECT_EX>::iterator OBJECTSARRAY::findBiggerCoordinate(unsigned int endCoord)
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


bool OBJECTSARRAY::insertSCANOBJECT_EXBefore(QVector<tSCANOBJECT_EX>::iterator whereToIt, unsigned int id, unsigned int firstCoord, unsigned int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
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



