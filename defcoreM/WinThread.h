//---------------------------------------------------------------------------
//2
#ifndef WinThreadH
#define WinThreadH

#include "Windows.h"
#include "ThreadClassList.h"

class cWinThread
{
public:

static long unsigned int WINAPI Execute(LPVOID parameter);
  cWinThread(TickProcPtr Tick);
  ~cWinThread(void);
  bool Resume();
  HANDLE _ThreadID;
  TickProcPtr _Proc;
  bool _toFinish; // ����� ���������� ���� � �����������
//
  unsigned int Tag;
  bool Finished; // �������
};
#endif

