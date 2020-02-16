#include "socket_lan.h"


void cSocketLan::disconnect()
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
    cISocket::disconnect();
}
// ------------------------------- cSocketLanTcp -------------------------------
// +
int cSocketLanTcp::getDataCountInTcpBuffer()
{
    unsigned long count = 0;
    int res_val = ioctl(_socket, FIONREAD, &count);

    if (res_val < 0) {
        return -1;
    }

    return count;
}

// +
bool cSocketLanTcp::connect(const cConnectionParams* socket_params)
{
    int* pSocket;
    const cSocketLanParams* params = static_cast<const cSocketLanParams*>(socket_params);

    if (!cISocket::connect(socket_params)) {
        return false;
    }
    if (params->_server) {
        pSocket = &_serverSocket;
    }
    else {
        pSocket = &_socket;
    }
    int local_port_num;
    local_port_num = params->_local_port_num;
    char* local_ip_address = 0;
    if (params->_local_ip_address[0] != '\0') {
        local_ip_address = params->_local_ip_address;
    }
    int remoute_port_num;
    remoute_port_num = params->_remoute_port_num;
    char* remoute_ip_address = params->_remoute_ip_address;

#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
    *pSocket = _socketProc(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
    *pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
    if (*pSocket < 0) {
        return false;
    }

    int i = 1;  // ?
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
    int sockopt_res = _setsockoptProc(*pSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));
#else
    int sockopt_res = setsockopt(*pSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));
#endif
    if (sockopt_res < 0) {
        disconnect();
        return false;
    }

    sockaddr_in target;
    memset(&target, 0x00, sizeof(sockaddr_in));

    target.sin_family = AF_INET;

#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
    target.sin_port = _htonsProc(local_port_num);
#else
    target.sin_port = htons(local_port_num);
#endif
    if (local_ip_address) {
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
        target.sin_addr.s_addr = _inet_addrProc(local_ip_address);
#else
        target.sin_addr.s_addr = inet_addr(local_ip_address);
#endif
    }
    else {
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
        target.sin_addr.s_addr = _htonlProc(INADDR_ANY);
#else
        target.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    }

    if (params->_transferDirection != cSocketLan::DirectionToRemoute) {
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
        if (_bindProc(*pSocket, reinterpret_cast<sockaddr*>(&target), sizeof(target)) < 0) {
#else
        if (bind(*pSocket, reinterpret_cast<sockaddr*>(&target), sizeof(target)) < 0) {
#endif
            disconnect();
            return false;
        }
    }
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
    target.sin_addr.s_addr = _inet_addrProc(remoute_ip_address);
#else
    target.sin_addr.s_addr = inet_addr(remoute_ip_address);
#endif

    if (!params->_server) {
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
        target.sin_port = _htonsProc(remoute_port_num);
        int resl = _connectProc(_socket, reinterpret_cast<sockaddr*>(&target), sizeof(target));
        if (resl < 0) {
            disconnect();
            return false;
        }
#else
        target.sin_port = htons(remoute_port_num);
        int resl = ::connect(_socket, reinterpret_cast<sockaddr*>(&target), sizeof(target));
        if (resl < 0) {
            disconnect();
            return false;
        }
#endif

    }
    else {
        int socketLen = sizeof(target);
#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
        int resl = _listenProc(_serverSocket, SOMAXCONN);
#else
        int resl = listen(_serverSocket, SOMAXCONN);
#endif
        if (resl < 0) {
            disconnect();
            return false;
        }
#ifdef DEFCORE_OS_WIN

#if defined DEFCORE_CC_MINGW
         _socket = _acceptProc(_serverSocket, reinterpret_cast<sockaddr*>(&target), &socketLen);
#else
        _socket = accept(_serverSocket, reinterpret_cast<sockaddr*>(&target), &socketLen);
#endif
#else
        _socket = accept(_serverSocket, reinterpret_cast<sockaddr*>(&target), reinterpret_cast<socklen_t*>(&socketLen));
#endif
        if (_socket < 0) {
            disconnect();
            return false;
        }
    }
    return true;
}
//
// +/- Разный код для Win и Lin, нужны тесты для объединения, пока legacy стиль
bool cSocketLanTcp::sendData(const unsigned char* msg, const int length)
{
    if (_socket < 0) {
        return false;
    }

#ifdef DEFCORE_OS_WIN
    sockaddr_in target;

    target.sin_family = AF_INET;

#ifdef DEFCORE_CC_MINGW
    target.sin_port = _htonsProc(0);
    target.sin_addr.s_addr = _htonlProc(INADDR_ANY);
    int i = _sendtoProc(_socket, reinterpret_cast<const char*>(msg), length, 0, (sockaddr*) &target, sizeof(target));
#else
    target.sin_port = htons(0);
    target.sin_addr.s_addr = htonl(INADDR_ANY);
    int i = sendto(_socket, reinterpret_cast<const char*>(msg), length, 0, (sockaddr*) &target, sizeof(target));
#endif

#ifdef Dbg
    std::cout << "sended - " << msg << endl;
#endif

    if (i != -1) {
        return true;
    }
    else {
#ifdef Dbg
        std::cout << "connect Error! - " << WSAGetLastError() << endl;
#endif
        return false;
    }
#else
    int errorCode = 0;
    socklen_t len = sizeof(errorCode);
    int retval = getsockopt(_socket, SOL_SOCKET, SO_ERROR, &errorCode, &len);
    if (retval == 0 && errorCode == 0) {
        if (send(_socket, msg, length, MSG_NOSIGNAL) != -1) {
            return true;
        }
    }
    return false;
#endif
}

// +
int cSocketLanTcp::reciveData(unsigned char* msg, const int length)
{
    int res = getDataCountInTcpBuffer();
    if (res == -1) {
        return -1;
    }
    if (res == 0) {
        return 0;
    }

    return cISocket::reciveData(msg, length);
}

// ------------------------------- cSocketLanUdp -------------------------------
bool cSocketLanUdp::connect(const cConnectionParams* socket_params)
{
    // Begin of wrapper block
    if (!cISocket::connect(socket_params)) {
        return false;
    }

    const cSocketLanParams* params = static_cast<const cSocketLanParams*>(socket_params);

    char* local_ip_address = 0;
    if (params->_local_ip_address[0] != '\0') {
        local_ip_address = params->_local_ip_address;
    }
    int remoute_port_num = params->_remoute_port_num;
    char* remoute_ip_address = params->_remoute_ip_address;
    // End of wrapper block

    sockaddr_in target;
    memset(&target, 0, sizeof(sockaddr_in));

    target.sin_family = AF_INET;

#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
    target.sin_port = _htonsProc(params->_local_port_num);
    if (local_ip_address) {
        target.sin_addr.s_addr = _inet_addrProc(local_ip_address);
    }
    else {

        target.sin_addr.s_addr = _htonlProc(INADDR_ANY);
    }
    _socket = _socketProc(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket < 0) {
        return false;
    }
    {
        int i = 1;
        int sockopt_res_1 = _setsockoptProc(_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));
        if (sockopt_res_1 < 0) {
            disconnect();
            return false;
        }
    }
#else
    target.sin_port = htons(params->_local_port_num);
    if (local_ip_address) {
        target.sin_addr.s_addr = inet_addr(local_ip_address);
    }
    else {

        target.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket < 0) {
        return false;
    }
    {
        int i = 1;
        int sockopt_res_1 = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));
        if (sockopt_res_1 < 0) {
            disconnect();
            return false;
        }
    }
#endif

#ifdef DEFCORE_OS_WIN
    {
        int bufsize = 262144;  //Увеличиваем размер буффера под вход. сообщения
#ifdef DEFCORE_CC_MINGW
        int sockopt_res_2 = _setsockoptProc(_socket, SOL_SOCKET, SO_RCVBUF, (char*) &bufsize, sizeof(bufsize));
#else
        int sockopt_res_2 = setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char*) &bufsize, sizeof(bufsize));
#endif
        if (sockopt_res_2 < 0) {
            disconnect();
            return false;
        }
    }
#endif

    // делаем сокет неблокирующим
    u_long p = 1;
    if (ioctl(_socket, FIONBIO, &p) < 0) {
        disconnect();
        return false;
    }

#if defined(DEFCORE_OS_WIN) && defined(DEFCORE_CC_MINGW)
    if (params->_transferDirection != cSocketLan::DirectionToRemoute) {
        int result = _bindProc(_socket, reinterpret_cast<sockaddr*>(&target), sizeof(target));
        if (result < 0) {
            disconnect();
            return false;
        }
    }
    target.sin_port = _htonsProc(remoute_port_num);
    target.sin_addr.s_addr = _inet_addrProc(remoute_ip_address);

    if (params->_transferDirection == cSocketLan::DirectionToRemoute) {
        if (_connectProc(_socket, reinterpret_cast<sockaddr*>(&target), sizeof(target)) < 0) {
            disconnect();
            return false;
        }
    }
#else
    if (params->_transferDirection != cSocketLan::DirectionToRemoute) {
        int result = bind(_socket, reinterpret_cast<sockaddr*>(&target), sizeof(target));
        if (result < 0) {
            disconnect();
            return false;
        }
    }
    target.sin_port = htons(remoute_port_num);
    target.sin_addr.s_addr = inet_addr(remoute_ip_address);

    if (params->_transferDirection == cSocketLan::DirectionToRemoute) {
        if (::connect(_socket, reinterpret_cast<sockaddr*>(&target), sizeof(target)) < 0) {
            disconnect();
            return false;
        }
    }
#endif
    return true;
}
// +
bool cSocketLanUdp::sendData(const unsigned char* msg, const int length)
{
int errorCode = 0;

#ifdef DEFCORE_OS_WIN
    int len = sizeof(errorCode);
#ifdef DEFCORE_CC_MINGW
    int retval = _getsockoptProc(_socket, SOL_SOCKET, SO_ERROR, (char*)&errorCode, &len);
    if (retval == 0 && errorCode == 0) {
    if (_sendProc(_socket, (const char*)msg, length, 0) != SOCKET_ERROR) {
        return true;
    }
#else
    int retval = getsockopt(_socket, SOL_SOCKET, SO_ERROR, &errorCode, &len);
    if (retval == 0 && errorCode == 0) {
    if (send(_socket, (const char*)msg, length, 0) != SOCKET_ERROR) {
        return true;
    }
#endif
}
#else
    socklen_t len = sizeof(errorCode);
    int retval = getsockopt(_socket, SOL_SOCKET, SO_ERROR, &errorCode, &len);
    if (retval == 0 && errorCode == 0) {
        if (send(_socket, msg, length, MSG_NOSIGNAL) != -1) {
            return true;
        }
    }
#endif
    return false;
}

