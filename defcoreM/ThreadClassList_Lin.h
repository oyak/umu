#ifndef THREADCLASSLIST_LIN_H
#define THREADCLASSLIST_LIN_H

#include <vector>

#include "ThreadClassList.h"

class LinThread;

class cThreadClassList_Lin : public cThreadClassList
{
public:
    cThreadClassList_Lin();
    ~cThreadClassList_Lin();

    int AddTick(TickClassPtr object, TickProcPtr function);
    bool DelTick(int index);
    bool Finished(int index);
    bool Resume(int index);

    std::vector <LinThread *> _threadList;
};

#endif // THREADCLASSLIST_LIN_H
