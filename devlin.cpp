#include "devlin.h"

DEVLin::DEVLin()
{
    pDevice = new UNITLIN;
}
DEVLin::~DEVLin(){}

void DEVLin::start()
{
    pDevice->start();
}

void DEVLin::onFinishing()
{
    pDevice->stop();
    delete pDevice;
}
