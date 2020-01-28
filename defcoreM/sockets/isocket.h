#ifndef ISOCKET_H
#define ISOCKET_H

/*
 * Данный заголовочный файл является кроссплатформенным для систем Windows
 * и Linux, определяет общий интерфейс cISocket для всех POSIX совместимых
 * сокетов и определяет базовоую логику их работы
 */

// STD headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include "platforms.h"
#ifdef DEFCORE_OS_WIN
#include <winsock2.h>
//#include "C:/Program Files/Embarcadero/RAD Studio/7.0/include/winsock2.h"
#include <tchar.h>

#ifdef DEFCORE_CC_MINGW
#define ioctl _ioctlsocketProc
#else
#define ioctl ioctlsocket
#endif

#endif

// POSIX and GNU C headers
// Linux headers
#ifndef WIN32
#include <linux/sockios.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/sockios.h>
#endif

#if (!defined ANDROID) && (!defined WIN32)
#include <linux/can.h>
#endif

#include <cerrno>

#if defined (DEFCORE_OS_WIN) && defined (DEFCORE_CC_MINGW)
typedef int WSAAPI (*WSAStartupPtr)(WORD wVersionRequested,LPWSADATA lpWSAData);
typedef int WSAAPI (*WSACleanupPtr)(void);
typedef SOCKET WSAAPI (*acceptPtr)(SOCKET s,struct sockaddr *addr,int *addrlen);
typedef int WSAAPI (*bindPtr)(SOCKET s,const struct sockaddr *name,int namelen);
typedef int WSAAPI (*connectPtr)(SOCKET s,const struct sockaddr *name,int namelen);
typedef int WSAAPI (*getsockoptPtr)(SOCKET s,int level,int optname,char *optval,int *optlen);
typedef u_long WSAAPI (*htonlPtr)(u_long hostlong);
typedef u_short WSAAPI (*htonsPtr)(u_short hostshort);
typedef unsigned __LONG32 WSAAPI (*inet_addrPtr)(const char *cp);
typedef int WSAAPI (*ioctlsocketPtr)(SOCKET s,__LONG32 cmd,u_long *argp);
typedef int WSAAPI (*listenPtr)(SOCKET s,int backlog);
typedef int WSAAPI (*recvPtr)(SOCKET s,char *buf,int len,int flags);
typedef int WSAAPI (*sendPtr)(SOCKET s,const char *buf,int len,int flags);
typedef int WSAAPI (*sendtoPtr)(SOCKET s,const char *buf,int len,int flags,const struct sockaddr *to,int tolen);
typedef int WSAAPI (*setsockoptPtr)(SOCKET s,int level,int optname,const char *optval,int optlen);
typedef SOCKET WSAAPI (*socketPtr)(int af,int type,int protocol);
#endif

class cISocket
{
protected:
    int _socket;
    int _serverSocket;

#ifdef DEFCORE_OS_WIN
    bool _state;
#endif

    // Запрещаем копировнаие
    explicit cISocket(const cISocket&);
    void operator=(const cISocket&);

public:
    struct cConnectionParams
    {
        // В зависимости от того, с чем соединяется сокет, нам требуется
        // передавать абсолютно разные параметры соединения для метода connect()
        // поэтому с целью унифицирования интерфейса мы заводим дополнительную
        // абстрактную структуру для хранения параметров соединения
        cConnectionParams() {}
        virtual ~cConnectionParams() {}
    };

    // Общие методы для всех реализаций сокетов
    cISocket();
    virtual ~cISocket();

    int getSocket() const
    {
        return _socket;
    }
    // Методы, которые необходимо определить/дополнить для конкретной реализации
    virtual bool connect(const cConnectionParams* socket_params);
    virtual void disconnect();
    virtual bool sendData(const unsigned char* msg, const int length) = 0;
    virtual int reciveData(unsigned char* msg, const int length);

#ifdef DEFCORE_OS_WIN
    bool getState() const
    {
        return _state;
    }
    void setState(const bool state)
    {
        _state = state;
    }

#ifdef DEFCORE_CC_MINGW
protected:
    WSAStartupPtr _WSAStartupProc;
    WSACleanupPtr _WSACleanupProc;
    acceptPtr _acceptProc;
    bindPtr _bindProc;
    connectPtr _connectProc;
    getsockoptPtr _getsockoptProc;
    htonlPtr _htonlProc;
    htonsPtr _htonsProc;
    inet_addrPtr _inet_addrProc;
    ioctlsocketPtr _ioctlsocketProc;
    listenPtr _listenProc;
    recvPtr _recvProc;
    sendPtr _sendProc;
    sendtoPtr _sendtoProc;
    setsockoptPtr _setsockoptProc;
    socketPtr _socketProc;

private:
    HMODULE _libraryH;
#endif

#endif
};

#endif  // ISOCKET_H
