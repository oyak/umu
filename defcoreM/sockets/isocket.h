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
#define ioctl ioctlsocket
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
    virtual ~cISocket()
    {
        disconnect();
    }
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
#endif
};

#endif  // ISOCKET_H
