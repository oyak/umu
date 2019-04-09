#ifndef UNITLIN_H
#define UNITLIN_H

#include <QVector>
#include <QObject>
#include "CriticalSection_Lin.h"
#include "ThreadClassList_Lin.h"
#include "umudevice.h"

class UNITLIN: public QObject
{
    Q_OBJECT
public:

    UNITLIN();
    ~UNITLIN();
    void start();
    void stop();
    cCriticalSection_Lin *createCriticalSection();

    //unsigned int create_CriticalSection();

    void criticalSectionEnter(cCriticalSection_Lin *cs);
    void criticalSectionRelease(cCriticalSection_Lin *cs);

// для отладки
    void _onPathStep(int shift, unsigned int coordInSteps);
    void printConnectionStatus();

signals:
    void CDUconnected();

private slots:
    void on_CDU_connected();

private:
    class UMUDEVICE *_pDevice;
    QVector <cCriticalSection_Lin*> _cs;
    cCriticalSection_Lin vectorCs;
    cThreadClassList_Lin _thList;
};

#endif // UNITLIN_H
