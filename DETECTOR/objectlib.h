#ifndef OBJECTLIB_H
#define OBJECTLIB_H

#include <QMap>
#include "Definitions.h"
#include "scanobject.h"

enum eOBJECT_ORDER
{
    OverPlaced = 0, // в порядке увеличения приоритета
    ExpandedOverPlacing = 1,
    OverPlacing = 2,
//
    NumOfOrders = 3
};

struct tSCANOBJECT_EX
{
   unsigned int Id;
   unsigned int Size; // длина объекта - LastCoordinate = FirstCoordinate + Size
   unsigned int FirstCoordinate; // этой координате соответствет первый элемент SCANOBJECT
   unsigned int LastCoordinate; // этой координате соответствет последний элемент SCANOBJECT
   int N0EMSShift;
   eOBJECT_ORDER ObjectOrder;
   SCANOBJECT *pScanObject;
   tSCANOBJECT_EX()
   {
       pScanObject = new SCANOBJECT;
   }
   tSCANOBJECT_EX(SCANOBJECT *pObject)
   {
       pScanObject = pObject;
   }
} ;

typedef struct _OBJECT_SOURCE_DESCRIPTOR
{
    QString SourceFileName;
    eUMUSide Side;
    int StartKm;
    int StartPk;
    int StartM;
    int StartmM;
    unsigned int LengthMM;
    int N0EMSShift; // смещение центра в канале 0 гр. относительно центра объекта - LengthMM/2
// при создании файла объекта длина объекта увеличивается на abs(N0ENSShift), если смещение
// отрицательное (т.е центр записи в канале 0 находится слева от центра объекта), то файл
// при создании удлиняется слева, иначе - справа. Если N0EMSShift отлично от нуля,
// поле Order может иметь значение только ExpandedOverPlaced

    unsigned int LngCutting; // укорочение длины
    eOBJECT_ORDER Order; //
    QString ObjectName;
    _OBJECT_SOURCE_DESCRIPTOR()
    {
        Order = OverPlaced;
        N0EMSShift = 0;
        LngCutting = 0;
    }

} tObjectSourceDescr;


class OBJECTLIB
{
public:
    OBJECTLIB();
    ~OBJECTLIB();

    bool getRecordData(unsigned int Id, tObjectSourceDescr& descriptor);
    void getAllIds(QVector<unsigned int>& IdsArray);
    void getAllSorceFiles(QVector<QString>& FileNamesArray);

private:
    QMap<unsigned int, tObjectSourceDescr> table;
};
#endif // OBJECTLIB_H
