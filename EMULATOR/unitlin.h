#ifndef UNITLIN_H
#define UNITLIN_H

#include <QVector>
#include <QObject>
#include "CriticalSection_Lin.h"
#include "ThreadClassList_Lin.h"
#include "umudevice.h"
#include "config.h"

class UNITLIN: public QObject
{
    Q_OBJECT
public:

    static class UMUDEVICE *DevicePtr;
    static class cCriticalSection_Lin classCs;
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);

    UNITLIN(CONFIG *pConfig);
    ~UNITLIN();
    void start();
    void stop();
    cCriticalSection_Lin *createCriticalSection();

    //unsigned int create_CriticalSection();

    void criticalSectionEnter(cCriticalSection_Lin *cs);
    void criticalSectionRelease(cCriticalSection_Lin *cs);

// работа с настройками
    QString& getCDULocalIPAddress();
    QString& getCDURemoteIPAddress();
    bool setCDULocalIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0);
    bool setCDURemoteIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0);
    QString& getPCLocalIPAddress();
    QString& getPCRemoteIPAddress();
    bool setPCLocalIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0);
    bool setPCRemoteIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0);

    bool setCDULocalPort(QString port);
    bool setCDURemotePort(QString port);
    bool setPCLocalPort(QString port);
    bool setPCRemotePort(QString port);

    unsigned short getCDULocalPort();
    unsigned short getCDURemotePort();
    unsigned short getPCLocalPort();
    unsigned short getPCRemotePort();

    bool getRestorePCConnectionFlagState();
    void setRestorePCConnectionFlag(bool state);

    QString getPathToObjectsFiles();
    void setPathToObjectsFiles(QString path);

    bool testPassword(const QString& password);
    void save();
    void scatterIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0, QString sourceString);


// для отладки
    void _onPathStep(int shift, unsigned int coordInSteps);
    void printConnectionStatus();

//
signals:
    void CDUconnected();
    void message(QString s);

private slots:
    void on_CDU_connected();
    void onMessage(QString s); // слот на сигналы с текстовыми сообщениями от используемых классов

private:
    class UMUDEVICE *_pDevice;
    QVector <cCriticalSection_Lin*> _cs;
    cCriticalSection_Lin vectorCs;
    cThreadClassList_Lin _thList;
};

#endif // UNITLIN_H
