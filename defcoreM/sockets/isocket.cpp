#include "isocket.h"
//#include "errno.h"

cISocket::cISocket() : _socket (-1),
                       _serverSocket (-1)
{
#ifdef DEFCORE_OS_WIN
    _state = false;
#endif
}

bool cISocket::connect(const cConnectionParams *socket_params)
{
    if (socket_params == 0)
    {
        return false;
    }
    // Если сокет уже открыт и не был закрыт
    if ((_socket >= 0) || (_serverSocket >= 0))
    {
        return false;
    }

/*
#if defined(DEFCORE_OS_WIN)
    WSADATA wsadata;

    _state = false;

    int error = WSAStartup(0x0202, &wsadata);

    if (error)
        return false;

    if (wsadata.wVersion != 0x0202)
    {
        WSACleanup();
        return false;
    }
#endif
*/

    return true;
}

void cISocket::disconnect()
{
    if (_serverSocket >= 0)
    {
        close(_serverSocket);
        _serverSocket = -1;
    }
//
    if (_socket >= 0)
    {
        close(_socket);
        _socket = -1;
    }

    /*
#ifdef DEFCORE_OS_WIN
    WSACleanup();
    _state = false;
#endif
*/
}

int cISocket::reciveData(unsigned char *msg, const int length)
{
    if (_socket < 0)
    {
        return -1;
    }

    int read_res = -1;

#ifdef DEFCORE_OS_WIN
    read_res = recv(_socket, reinterpret_cast<char*>(msg), length, 0);
    if (read_res == SOCKET_ERROR)  read_res = 0; //FDV

#else
    read_res = recv(_socket, msg, length, MSG_DONTWAIT); // MSG_DONTWAIT подтверждена работоспособность с CAN под Linux, в случае нарушения работы иных протоколов заменить на 0
#endif

    if (read_res < 0)
    {
        read_res = -1;
        if (errno == EAGAIN)
        {
            read_res = 0;
        }
    }

    return read_res;
}
