//---------------------------------------------------------------------------

#pragma hdrstop

#include "ThreadClassList_Win.h"

//---------------------------------------------------------------------------

int cThreadClassList_Win::AddTick(TickClassPtr object, TickProcPtr function)
{
	cWinThread* tmp;
	tmp = new cWinThread(function);

//    tmp->Priority = tpHigher;
    // tpIdle, tpLowest, tpLower, tpNormal, tpHigher, tpHighest, tpTimeCritical

    tmp->Tag = ThList.size();
	ThList.push_back(tmp);
//    tmp->Suspend();
	return ThList.size() - 1;
}

bool cThreadClassList_Win::Resume(int Index)
{
    ThList[Index]->Resume();
    return true;
}

bool cThreadClassList_Win::DelTick(int Index)
{
     if (Finished(Index))
     {
          delete ThList[Index];
     }
     return true;
}

bool cThreadClassList_Win::Finished(int Index)
{
	return ThList[Index]->Finished;
}

