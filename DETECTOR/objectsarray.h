#ifndef OBJECTSARRAY_H
#define OBJECTSARRAY_H


#include <QVector>
#include "scanobject.h"
#include "objectlib.h"
#include "platforms.h"
#include "CriticalSection.h"

class cCriticalSection;

/*
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
*/

class OBJECTSARRAY{
//
public:
//
    OBJECTSARRAY(cCriticalSection* cs, float pathStep): _cs(cs),
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
    QVector<tSCANOBJECT_EX> _objectArray;
// ��������� ��������� StartCoordinate-EndCoordinate ���������
// �� ������ ������������. �������� ������ ���� ����������� �� ����������� StartCoordinate

    QVector<tSCANOBJECT_EX>::iterator _currentObject;
    QVector<tSCANOBJECT_EX>::iterator _lastObject;

    cCriticalSection* _cs;

    QVector<tSCANOBJECT_EX>::iterator findOverCrossCoordinate(unsigned int startCoord, unsigned int endCoord, QVector<tSCANOBJECT_EX>::iterator startElement);
    QVector<tSCANOBJECT_EX>::iterator findBiggerCoordinate(unsigned int endCoord);

    bool insertSCANOBJECT_EXBefore(QVector<tSCANOBJECT_EX>::iterator whereToIt, unsigned int id, unsigned int firstCoord, unsigned int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
    unsigned int convertMMtoPathStep(unsigned int pathInMM);

};




#endif // OBJECTSARRAY_H