#include <iostream>
#include <cstdlib>
#include "platforms.h"

#include "ThreadClassList_Lin.h"
#include "LinThread.h"

cThreadClassList_Lin::cThreadClassList_Lin() {}

cThreadClassList_Lin::~cThreadClassList_Lin()
{
    for (std::vector<LinThread*>::iterator it = _threadList.begin(); it != _threadList.end(); ++it) {
        if (*it != NULL) {
            delete *it;
        }
    }
    _threadList.clear();
}

int cThreadClassList_Lin::AddTick(TickClassPtr object, TickProcPtr function)
{
    LinThread* linThread = new LinThread(object, function);
    _threadList.push_back(linThread);

    return _threadList.size() - 1;
}

bool cThreadClassList_Lin::DelTick(int index)
{
    DEFCORE_ASSERT(index >= 0);
    DEFCORE_ASSERT(index < (int) _threadList.size());

    if (_threadList[index] != NULL) {
        delete _threadList[index];
        _threadList[index] = NULL;
    }

    return true;
}

bool cThreadClassList_Lin::Finished(int index)
{
    DEFCORE_ASSERT(index >= 0);
    DEFCORE_ASSERT(index < (int) _threadList.size());

    if (_threadList[index] != NULL) {
        return _threadList[index]->isFinished();
    }

    return true;
}

bool cThreadClassList_Lin::Resume(int index)
{
    int result = _threadList[index]->start();
    if (result == 0) {
        return true;
    }

    return false;
}
