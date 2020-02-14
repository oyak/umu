#include "isocket.h"
#include "stdio.h"
#include "assert.h"
#include <iostream>


#if defined (DEFCORE_OS_WIN) && defined (DEFCORE_CC_MINGW)
const char WINSOCKET_LIBRARY_PATH[] = {"C:\\WINDOWS\\system32\\wsock32.dll"}; // path for windows XP
#endif

cISocket::cISocket() : _socket (-1),
                       _serverSocket (-1)
{
#ifdef DEFCORE_OS_WIN
    _state = false;

#ifdef DEFCORE_CC_MINGW
//int strLen = strlen(WINSOCKET_LIBRARY_PATH);
//wchar_t libPath[strLen + 1];
//    mbstowcs(libPath, WINSOCKET_LIBRARY_PATH, strLen);
    _libraryH = LoadLibraryA(WINSOCKET_LIBRARY_PATH);

    if (_libraryH)
    {
        _WSAStartupProc = (WSAStartupPtr)GetProcAddress(_libraryH, "WSAStartup");
        _WSACleanupProc = (WSACleanupPtr)GetProcAddress(_libraryH, "WSACleanup");
        _acceptProc = (acceptPtr)GetProcAddress(_libraryH, "accept");
        _bindProc = (bindPtr)GetProcAddress(_libraryH, "bind");
        _connectProc = (connectPtr)GetProcAddress(_libraryH, "connect");
        _getsockoptProc = (getsockoptPtr)GetProcAddress(_libraryH, "getsockopt");
        _htonlProc = (htonlPtr)GetProcAddress(_libraryH, "htonl");
        _htonsProc = (htonsPtr)GetProcAddress(_libraryH, "htons");
        _inet_addrProc = (inet_addrPtr)GetProcAddress(_libraryH, "inet_addr");
        _ioctlsocketProc = (ioctlsocketPtr)GetProcAddress(_libraryH, "ioctlsocket");
        _listenProc = (listenPtr)GetProcAddress(_libraryH, "listen");
        _recvProc = (recvPtr)GetProcAddress(_libraryH, "recv");
        _sendtoProc = (sendtoPtr)GetProcAddress(_libraryH, "sendto");
        _sendProc = (sendPtr)GetProcAddress(_libraryH, "send");
        _setsockoptProc = (setsockoptPtr)GetProcAddress(_libraryH, "setsockopt");
        _socketProc = (socketPtr)GetProcAddress(_libraryH, "socket");
        _closesocketProc = (closesocketPtr)GetProcAddress(_libraryH, "closesocket");

        if (!_WSAStartupProc || \
            !_WSACleanupProc || \
            !_acceptProc || \
            !_bindProc || \
            !_connectProc || \
            !_getsockoptProc || \
            !_htonlProc || \
            !_htonsProc || \
            !_inet_addrProc || \
            !_ioctlsocketProc || \
            !_listenProc || \
            !_recvProc || \
            !_sendtoProc || \
            !_sendProc || \
            !_setsockoptProc || \
            !_socketProc || \
            !_closesocketProc){
            std::cerr << "cISocket(): wsock32.dll functions not found"  << std::endl;
            FreeLibrary(_libraryH);
            _libraryH = 0;
            exit(1);
        }
    }
        else
        {
            std::cerr << "cISocket(): wsock32.dll not found"  << std::endl;
            exit(1);
        }
#endif
#endif
}

cISocket::~cISocket()
{
    disconnect();
#if defined (DEFCORE_OS_WIN) && defined (DEFCORE_CC_MINGW)
    if (_libraryH) FreeLibrary(_libraryH);
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

#if defined(DEFCORE_OS_WIN)
    WSADATA wsadata;

    _state = false;

#ifdef DEFCORE_CC_MINGW
    int error = _WSAStartupProc(0x0202, &wsadata);
#else
    int error = WSAStartup(0x0202, &wsadata);
#endif

    if (error)
        return false;

    if (wsadata.wVersion != 0x0202)
    {
#ifdef DEFCORE_CC_MINGW
        _WSACleanupProc();
#else
        WSACleanup();
#endif
        return false;
    }
#endif
    return true;
}

void cISocket::disconnect()
{
    if (_serverSocket >= 0)
    {
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
       _closesocketProc(_serverSocket);
#else
        closesocket(_serverSocket);
#endif
        _serverSocket = -1;
    }
//
    if (_socket >= 0)
    {
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
        _closesocketProc(_socket);
#else
        closesocket(_socket);
#endif
        _socket = -1;
    }

#ifdef DEFCORE_OS_WIN
#ifdef DEFCORE_CC_MINGW
    _WSACleanupProc();
#else
    WSACleanup();
#endif
    _state = false;
#endif

}

int cISocket::reciveData(unsigned char *msg, const int length)
{
    if (_socket < 0)
    {
        return -1;
    }

    int read_res = -1;

#ifdef DEFCORE_OS_WIN
#if defined (DEFCORE_CC_MINGW)
    read_res = _recvProc(_socket, reinterpret_cast<char*>(msg), length, 0);
#else
    read_res = recv(_socket, reinterpret_cast<char*>(msg), length, 0);
#endif
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
