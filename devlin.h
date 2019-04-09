#ifndef DEVLIN_H
#define DEVLIN_H
#include "unitlin.h"

class DEVLin: public QObject
{
    Q_OBJECT

public:
    DEVLin();
    ~DEVLin();
    void start();

public slots:
    void onFinishing();

private:
    UNITLIN *pDevice;
};

#endif // DEVLIN_H
