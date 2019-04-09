#ifndef LINTHREAD_H
#define LINTHREAD_H

#include <pthread.h>
#include "ThreadClassList.h"

class LinThread
{
public:
    LinThread(TickClassPtr object, TickProcPtr function);
    ~LinThread();

    int start();
    bool isFinished();

private:
    static void* execute(void* param);

private:
    pthread_t _thread;
    TickClassPtr _object;
    TickProcPtr _function;
    bool _isFinished;
};

#endif  // LINTHREAD_H
