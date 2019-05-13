#ifndef OBJECTSTOR_H
#define OBJECTSTOR_H
#include "test.h"

// ���� = ��� �������� Id ������� + "d" ��� "r" - ��� �������������
// ����������� ��������

class OBJECTSTOR
{
public:
    OBJECTSTOR(QString& pathToFiles);
    ~OBJECTSTOR();
    SCANOBJECT *extractObject(eOBJECT_ORDER& ObjectOrder, unsigned int& len, int& N0EMSShift, unsigned int objectId);

private:
    QMap<QString, tSCANOBJECT_EX*> _storage;

    void createStorage(QString &pathToFiles);
    bool addObject(unsigned int id, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject);
    void constructKey(QString& key, unsigned int objectId);

};


#endif // OBJECTSTOR_H
