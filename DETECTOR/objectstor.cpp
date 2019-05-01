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
SCANOBJECT *OBJECTSTOR::extractObject(eOBJECT_ORDER& objectOrder, unsigned int& len, unsigned int objectId, Test::eMovingDir movingDirection)
{
SCANOBJECT *res = nullptr;
QString key;
    constructKey(key, objectId, movingDirection);
    if (_storage.contains(key))
    {
        res = _storage[key]->pScanObject;
        objectOrder = _storage[key]->ObjectOrder;
        len = _storage[key]->Size;
    }
    return res;
}

void OBJECTSTOR::createStorage(QString& pathToFiles)
{
QVector<unsigned int> objectIdsArray;
OBJECTLIB lib;
SOBFMAKER maker(pathToFiles);
tSCANOBJECT_EX* pObjectEx;

    lib.getAllIds(objectIdsArray);
    if (!objectIdsArray.empty())
    {
    QVector<unsigned int>::iterator it;
        for(it = objectIdsArray.begin(); it != objectIdsArray.end(); ++it)
        {
            QString key;
            pObjectEx = maker.restoreObjectFromFile(*it, Test::DirUpWard);
            if(pObjectEx)
            {
                constructKey(key, *it, Test::DirUpWard);
                _storage.insert(key, pObjectEx);
            }
            pObjectEx = maker.restoreObjectFromFile(*it, Test::DirDownWard);
            if(pObjectEx)
            {
                constructKey(key, *it, Test::DirDownWard);
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

void OBJECTSTOR::constructKey(QString& key, unsigned int objectId, Test::eMovingDir movingDirection)
{
    assert(movingDirection != Test::DirNotDefined);
    key.setNum(objectId);
    if (movingDirection == Test::DirUpWard) key += "d";
        else key += "r";
}
