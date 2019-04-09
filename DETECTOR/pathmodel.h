#ifndef PATHMODEL_H
#define PATHMODEL_H

#include <QVector>
#include "scanobject.h"
#include "objectlib.h"
#include "platforms.h"
#include "CriticalSection.h"

class cCriticalSection;


typedef struct _COMPLEX_OBJECT
{
    unsigned int StartCoordinate; // �������� ����������� ����������� FirstCoordinate �� ���� ��������� ������� complexObj
    unsigned int EndCoordinate;   // �������� ������������ ����������� LastCoordinate �� ���� ��������� ������� complexObj
    QVector<tSCANOBJECT_EX> ExObjectsArray;
    unsigned int ExObjectsArrayMaxSize;
// ������ ������� � ExObjectsArray ����������� ������, �.� ���� ������� ���������� ��������� � � ������� �
// �� ������� ��������, �� ����� ������ �� �������
// � ������� � ������� ��������� ������ ���� ������ �������� ���� ObjectOrder
    unsigned int OverPlacedElement;
    _COMPLEX_OBJECT():ExObjectsArrayMaxSize(2),
                     OverPlacedElement(0)
    {
    }
    ~_COMPLEX_OBJECT()
    {
        ExObjectsArray.clear();
    }
}tCOMPLEXOBJECT;


class PATHMODEL{
//
public:
//
    PATHMODEL(cCriticalSection* cs, float pathStep): _cs(cs),
                                     _currentObject(nullptr),
                                     _step(pathStep),
                                     _lastObject(nullptr)
    {

    }

    void deleteObjects();
    bool addObject(unsigned int id, unsigned int startCoord, unsigned int lenInSteps, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
//

    SignalsData *getObject(unsigned int coord, bool &isDataObject);
private:
    float _step; // ��� ��
    QVector<tCOMPLEXOBJECT> _objectArray;
// ��������� ��������� StartCoordinate-EndCoordinate ���������
// �� ������ ������������. �������� ������ ���� ����������� �� ����������� StartCoordinate

    QVector<tCOMPLEXOBJECT>::iterator _currentObject;
    QVector<tCOMPLEXOBJECT>::iterator _lastObject;

    cCriticalSection* _cs;

    QVector<tCOMPLEXOBJECT>::iterator findOverCrossCoordinate(unsigned int startCoord, unsigned int endCoord, QVector<tCOMPLEXOBJECT>::iterator startElement);
    QVector<tCOMPLEXOBJECT>::iterator findBiggerCoordinate(unsigned int endCoord);

    bool addExObject(QVector<tCOMPLEXOBJECT>::iterator whereToIt, unsigned int id, unsigned int firstCoord, unsigned int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
    bool insertComplexObjectBefore(QVector<tCOMPLEXOBJECT>::iterator whereToIt, unsigned int id, unsigned int firstCoord, unsigned int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
    unsigned int convertMMtoPathStep(unsigned int pathInMM);

};

#endif // PATHMODEL_H
