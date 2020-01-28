#include "WinThread.h"

//cWinThread::cWinThread(TickClassPtr object, TickProcPtr Tick)
cWinThread::cWinThread(TickProcPtr Tick)
{
long unsigned int idx;
    _Proc = Tick;
    _toFinish = false;
    Finished = false;
    _ThreadID = CreateThread(NULL, 0, &Execute, (LPVOID)this, CREATE_SUSPENDED, &idx);
}

cWinThread::~cWinThread(void)
{
    _toFinish = true;
    while(_ThreadID);
}

bool cWinThread::Resume()
{
    return (bool)ResumeThread(_ThreadID);
    return false;
}
//parameter - указатель на объект класса cWinThread
long unsigned int WINAPI cWinThread::Execute(LPVOID parameter)
{
class cWinThread *objectPtr = (cWinThread*)parameter;

     while (true)
     {
          if (!objectPtr->_Proc(0) | objectPtr->_toFinish) break;
     };
     objectPtr->_ThreadID = 0;
     objectPtr->Finished = true;
     return 0;
}

