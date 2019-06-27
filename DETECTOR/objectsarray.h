#ifndef OBJECTSARRAY_H
#define OBJECTSARRAY_H


#include <QVector>
#include "scanobject.h"
#include "objectlib.h"
#include "platforms.h"
#include "CriticalSection.h"
#include "test.h"

class cCriticalSection;

/*
typedef struct _COMPLEX_OBJECT
{
    unsigned int StartCoordinate; // является минимальной координатой FirstCoordinate из всех элементов массива complexObj
    unsigned int EndCoordinate;   // является максимальной координатой LastCoordinate из всех элементов массива complexObj
    QVector<tSCANOBJECT_EX> ExObjectsArray;
    unsigned int ExObjectsArrayMaxSize;
// второй элемент в ExObjectsArray перекрывает первый, т.е если текущая координата относится и к первому и
// ко второму элементу, то берем данные из второго
// у первого и второго элементов должны быть разные значения поля ObjectOrder
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

class OBJECTSARRAY:public QObject
{
    Q_OBJECT
public:
//
    OBJECTSARRAY(cCriticalSection* cs, float pathStep): _cs(cs),
                                     _currentObject(nullptr),
                                     _step(pathStep),
                                     _lastObject(nullptr),
                                     _movingDirection(Test::DirNotDefined)
    {

    }

    void deleteObjects();
    bool addObject(unsigned int id, int startCoord, unsigned int lenInSteps, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
//

    SignalsData *getObject(int coord, bool &isDataObject);
    void setMovingDirection(Test::eMovingDir movingDirection);
private:
    float _step; // шаг ДП
    QVector<tSCANOBJECT_EX> _objectArray;
// диапазоны координат StartCoordinate-EndCoordinate элементов
// не должны пересекаться. Элементы должны быть упорядочены по возрастанию StartCoordinate

    QVector<tSCANOBJECT_EX>::iterator _currentObject;
    QVector<tSCANOBJECT_EX>::iterator _lastObject;

    cCriticalSection* _cs;
    Test::eMovingDir _movingDirection;

    QVector<tSCANOBJECT_EX>::iterator findOverCrossCoordinate(int startCoord, int endCoord, QVector<tSCANOBJECT_EX>::iterator startElement);
    QVector<tSCANOBJECT_EX>::iterator findBiggerCoordinate(int endCoord);

    bool insertSCANOBJECT_EXBefore(QVector<tSCANOBJECT_EX>::iterator whereToIt, unsigned int id, int firstCoord, int lastCoord, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
    unsigned int convertMMtoPathStep(unsigned int pathInMM);

signals:
    void message(QString);
};




#endif // OBJECTSARRAY_H
