#ifndef DATATRANSFER_LAN_H
#define DATATRANSFER_LAN_H

#include "idatatransfer.h"

class cDataTransferLan : public cIDataTransfer
{
public:
    struct cLanConnectionParams : public cSocketLan::cSocketLanParams
    {
        enum
        {
            _ip_size = 16
        };
        char _local_ip[_ip_size];
        char _remote_ip[_ip_size];
        int _port_in;
        int _port_out;
        bool _socket_1_tcp;   //  = true, if tcp connection
        bool _socket_2_tcp;  //
        cSocketLan::eSocketTransferDirection _socket_1_transfer_direction;
// ���� ������ _port_in _port_out ���������, ��������� ���� ����� - �����������
// �������� ��������������� DirectionBoth
// ��������������, ��� ����������� �������� ������ � _socket_1 � _socket_2
// ���������������
//
        bool _socket_1_server;   //  = true, ���� ���� ����������
        bool _socket_2_server;  //

        cLanConnectionParams():_socket_1_transfer_direction(cSocketLan::DirectionUnknown),
                               _socket_1_server(true),
                               _socket_2_server(true)
        {
        };
    };

    cDataTransferLan() {}
    ~cDataTransferLan() {}

    virtual int addConnection(const cISocket::cConnectionParams* const connection_params);
    virtual int openConnection(const int connection_id);
    virtual bool closeConnection(const int connection_id);
    virtual bool write(const int connection_id, const unsigned char* const data_ptr, const unsigned int data_count);
    virtual int read(const int connection_id, unsigned char* data_ptr, const unsigned int data_count);
};

#endif  // DATATRANSFER_LAN_H
