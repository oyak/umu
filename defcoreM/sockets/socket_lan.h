#ifndef SOCKET_LAN_H
#define SOCKET_LAN_H

#include "isocket.h"

class cSocketLan : public cISocket
{
public:

    enum eSocketTransferDirection
    {
        DirectionUnknown = 0,
        DirectionToLocal = 1,
        DirectionToRemoute = 2, // в этом случае bind() не вызываетс€ и порт всевыдел€етс€ динамически с 49152
        DirectionBoth = 3
    };

    struct cSocketLanParams : public cConnectionParams
    {
        int   _local_port_num;
        char *_local_ip_address;
        int   _remoute_port_num;
        char *_remoute_ip_address;
        bool _server; // дл€ сокета cSocketLanTcp, если ждем соединени€
        eSocketTransferDirection _transferDirection;
        cSocketLanParams(): _transferDirection(eSocketTransferDirection::DirectionUnknown)
        {};
    };
};

class cSocketLanUdp : public cSocketLan
{
public:
    virtual bool connect(const cConnectionParams *socket_params);
    virtual bool sendData(const unsigned char *msg, const int length);
};

class cSocketLanTcp : public cSocketLan
{
public:
    virtual bool connect(const cConnectionParams *socket_params);
    virtual bool sendData(const unsigned char *msg, const int length);
    virtual int  reciveData(unsigned char     *msg, const int length);

    int getDataCountInTcpBuffer();
};

#endif // SOCKET_LAN_H

