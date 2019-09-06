#include "objectstor.h"
#include "sobfmaker.h"
//#include "objectlib.h"


OBJECTSTOR::OBJECTSTOR(QString& pathToFiles)
{
    createStorage(pathToFiles);
}

OBJECTSTOR::~OBJECTSTOR()
{
QMap<QString, tSCANOBJECT_EX*>::iterator it;

    for(it = _storage.begin(); it != _storage.end(); ++it)
    {
        if (it.value()->pScanObject != 0)
            delete (it.value()->pScanObject);
    }
    _storage.clear();
}

// возвращает: objectOrder - порядок объекта, len - длина в мм
SCANOBJECT *OBJECTSTOR::extractObject(eOBJECT_ORDER& objectOrder, unsigned int& len, int& N0EMSShift, unsigned int objectId)
{
SCANOBJECT *res = nullptr;
QString key;
    constructKey(key, objectId);
    if (_storage.contains(key))
    {
        res = _storage[key]->pScanObject;
        objectOrder = _storage[key]->ObjectOrder;
        len = _storage[key]->Size;
        N0EMSShift = _storage[key]->N0EMSShift;
    }
    return res;
}

void OBJECTSTOR::createStorage(QString& pathToFiles)
{
    Q_UNUSED(pathToFiles);
QVector<unsigned int> objectIdsArray;
OBJECTLIB lib;
SOBFMAKER maker(":/files/files"); // файлы объектов включены в проект как ресурсы
tSCANOBJECT_EX* pObjectEx;

    lib.getAllIds(objectIdsArray);
    if (!objectIdsArray.empty())
    {
    QVector<unsigned int>::iterator it;
        for(it = objectIdsArray.begin(); it != objectIdsArray.end(); ++it)
        {
            QString key;
            pObjectEx = maker.restoreObjectFromFile(*it);
            if(pObjectEx)
            {
                constructKey(key, *it);
                _storage.insert(key, pObjectEx);
            }
        }
    }

    /*

    pObjectEx = maker.restoreObjectFromFile(21);
    if(pObjectEx)
    {
        _storage.insert("21d", pObjectEx);
    }
    pObjectEx = maker.restoreObjectFromFile(81);
    if(pObjectEx)
    {
        _storage.insert("81d", pObjectEx);
    }
    pObjectEx = maker.restoreObjectFromFile(91);
    if(pObjectEx)
    {
        _storage.insert("91d", pObjectEx);
    }
*/
}

bool OBJECTSTOR::addObject(unsigned int id, eOBJECT_ORDER objectOrder, SCANOBJECT *pObject)
{

}

void OBJECTSTOR::constructKey(QString& key, unsigned int objectId)
{
    key.setNum(objectId);
    key += "d";
}
