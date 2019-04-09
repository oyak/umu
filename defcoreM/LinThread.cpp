#include "LinThread.h"

#include <iostream>

LinThread::LinThread(TickClassPtr object, TickProcPtr function)
    : _thread(-1)
    , _object(object)
    , _function(function)
    , _isFinished(true)
{
}

LinThread::~LinThread()
{
    void* status;
    if (_thread != -1) {
        int rc = pthread_join(_thread, &status);
        std::cerr << "LinThread: " << _thread << " exited with code: " << rc << std::endl;
    }
}

int LinThread::start()
{
    if (pthread_create(&_thread, NULL, execute, this) != 0) {
        return -1;
    }

    return 0;
}

bool LinThread::isFinished()
{
    return _isFinished;
}

void* LinThread::execute(void* param)
{
    LinThread* linThread = static_cast<LinThread*>(param);
    linThread->_isFinished = false;
    while (true) {
        if (!linThread->_function(linThread->_object)) {
            break;
        }
    };
    linThread->_isFinished = true;

    return NULL;
}
