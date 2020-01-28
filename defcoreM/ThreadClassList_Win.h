//---------------------------------------------------------------------------

#ifndef ThreadClassList_WinH
#define ThreadClassList_WinH

#include <vector>

#include "threadClassList.h"
#include "WinThread.h"

class cThreadClassList_Win: public cThreadClassList
{
	std::vector < cWinThread* > ThList;

public:

	virtual int AddTick(TickClassPtr object, TickProcPtr function); // Создает и запускает поток, исполняющий функцию TickProcPtr, Возвращает номер потока
	virtual bool DelTick(int Index); // Удаляет ранее созданный поток
	virtual bool Finished(int Index); // Указывает что поток с номером Index закончил работу
    virtual bool Resume(int Index);

	#include "WinThread.h"

};

#endif
