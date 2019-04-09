#ifndef SOCKET_CAN_H
#define SOCKET_CAN_H

#include "isocket.h"
#include "TickCount.h"
#include "CriticalSection.h"
//#include "CriticalSection_Lin.h"

class cSocketCanRaw : public cISocket
{
    void waitForCan();
    void resetWait();
public:
    struct cSocketCanRawParams : public cConnectionParams
    {
        int _can_interface_num;
    };

    virtual bool connect(const cConnectionParams *socket_params);
    virtual void disconnect();
    virtual bool sendData(const unsigned char *msg, const int length);
    virtual int  reciveData(unsigned char     *msg, const int length);
};

#endif // SOCKET_CAN_H
