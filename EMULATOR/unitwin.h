#ifndef UNITWIN_H
#define UNITWIN_H

#include <QVector>
#include <QObject>
#include "CriticalSection_Win.h"
#include "ThreadClassList_Win.h"
#include "umudevice.h"
#include "config.h"

class UNITWIN: public QObject
{
    Q_OBJECT
public:

    static class UMUDEVICE *DevicePtr;
    static class cCriticalSection_Win classCs;
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);

    UNITWIN(CONFIG *pConfig);
    ~UNITWIN();
    void start();
    void stop();
    cCriticalSection_Win *createCriticalSection();

    //unsigned int create_CriticalSection();

    void criticalSectionEnter(cCriticalSection_Win *cs);
    void criticalSectionRelease(cCriticalSection_Win *cs);

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
    void restartCDUConection(void);

// для отладки
    void _onPathStep(int shift, unsigned int coordInSteps);
    void printConnectionStatus();

//
signals:
    void CDUconnected();
    void message(QString s);
    void messageHandlerSignal(QString s);

private slots:
    void on_CDU_connected();
    void onMessage(QString s); // слот на сигналы с текстовыми сообщениями от используемых классов
    void on_MessageHandler(QString s); // слот на сигнал обработчика UMUDEVICE::messageHandler(), порождает messageHandlerSignal

private:
    class UMUDEVICE *_pDevice;
    QVector <cCriticalSection_Win*> _cs;
    cCriticalSection_Win vectorCs;
    cThreadClassList_Win _thList;
};

#endif // UNITWIN_H
