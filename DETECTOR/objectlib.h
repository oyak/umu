#ifndef OBJECTLIB_H
#define OBJECTLIB_H

#include <QMap>
#include "Definitions.h"
#include "scanobject.h"

enum eOBJECT_ORDER
{
    OverPlaced = 0, // � ������� ���������� ����������
    ExpandedOverPlacing = 1,
    OverPlacing = 2,
//
    NumOfOrders = 3
};

struct tSCANOBJECT_EX
{
   unsigned int Id;
   unsigned int Size; // ����� ������� - LastCoordinate = FirstCoordinate + Size
   unsigned int FirstCoordinate; // ���� ���������� ������������ ������ ������� SCANOBJECT
   unsigned int LastCoordinate; // ���� ���������� ������������ ��������� ������� SCANOBJECT
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
    int N0EMSShift; // �������� ������ � ������ 0 ��. ������������ ������ ������� - LengthMM/2
// ��� �������� ����� ������� ����� ������� ������������� �� abs(N0ENSShift), ���� ��������
// ������������� (�.� ����� ������ � ������ 0 ��������� ����� �� ������ �������), �� ����
// ��� �������� ���������� �����, ����� - ������. ���� N0EMSShift ������� �� ����,
// ���� Order ����� ����� �������� ������ ExpandedOverPlaced

    unsigned int LngCutting; // ���������� �����
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
