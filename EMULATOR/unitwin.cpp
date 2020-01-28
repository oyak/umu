
#include "unitwin.h"
#include "QDebug"
#include "assert.h"

class UMUDEVICE *UNITWIN::DevicePtr;
class cCriticalSection_Win UNITWIN::classCs;

UNITWIN::UNITWIN(CONFIG *pConfig)
{
    _pDevice = new UMUDEVICE(&_thList, (void*)this, pConfig);
    DevicePtr = _pDevice;
     connect(_pDevice, SIGNAL(CDUconnected()), this, SLOT(on_CDU_connected()));
//    connect(_pDevice, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
     connect(_pDevice, SIGNAL(messageHandlerSignal(QString)), this, SLOT(on_MessageHandler(QString)));
}

UNITWIN::~UNITWIN()
{
QVector <cCriticalSection_Win*>::iterator it;
    disconnect(_pDevice, SIGNAL(CDUconnected()), this, SLOT(on_CDU_connected()));
//  disconnect(_pDevice, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
    classCs.Enter();
    delete _pDevice;
    DevicePtr = 0;
    classCs.Release();
    for (it = _cs.begin(); it < _cs.end(); ++it) delete *it;
    _cs.clear();
}

void UNITWIN::start()
{
    _pDevice->start();
}

void UNITWIN::stop()
{
    _pDevice->stop();
}

cCriticalSection_Win *UNITWIN::createCriticalSection()
{
cCriticalSection_Win *cs;
    vectorCs.Enter();
    cs = new cCriticalSection_Win;
    _cs.push_back(cs);
    vectorCs.Release();
    return cs;
}

void UNITWIN::criticalSectionEnter(cCriticalSection_Win* cs)
{
    cs->Enter();
}

void UNITWIN::criticalSectionRelease(cCriticalSection_Win *cs)
{
    cs->Release();
}

void UNITWIN::on_CDU_connected()
{
    emit CDUconnected();
}

//
QString& UNITWIN::getCDULocalIPAddress()
{
    return _pDevice->getCDULocalIPAddress();
}

QString& UNITWIN::getCDURemoteIPAddress()
{
    return _pDevice->getCDURemoteIPAddress();
}

bool UNITWIN::setCDULocalIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setCDULocalIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UNITWIN::setCDURemoteIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setCDURemoteIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

QString& UNITWIN::getPCLocalIPAddress()
{
    return _pDevice->getPCLocalIPAddress();
}

QString& UNITWIN::getPCRemoteIPAddress()
{
    return _pDevice->getPCRemoteIPAddress();
}

bool UNITWIN::setPCLocalIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setPCLocalIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UNITWIN::setPCRemoteIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setPCRemoteIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UNITWIN::setCDULocalPort(QString port)
{
    return _pDevice->setCDULocalPort(port);
}

bool UNITWIN::setCDURemotePort(QString port)
{
    return _pDevice->setCDURemotePort(port);
}

bool UNITWIN::setPCLocalPort(QString  port)
{
    return _pDevice->setPCLocalPort(port);
}

bool UNITWIN::setPCRemotePort(QString  port)
{
    return _pDevice->setPCRemotePort(port);
}

unsigned short UNITWIN::getCDULocalPort()
{
    return _pDevice->getCDULocalPort();
}

unsigned short UNITWIN::getCDURemotePort()
{
    return _pDevice->getCDURemotePort();
}

unsigned short UNITWIN::getPCLocalPort()
{
    return _pDevice->getPCLocalPort();
}

unsigned short UNITWIN::getPCRemotePort()
{
    return _pDevice->getPCRemotePort();
}

bool UNITWIN::getRestorePCConnectionFlagState()
{
    return _pDevice->getRestorePCConnectionFlagState();
}

void UNITWIN::setRestorePCConnectionFlag(bool state)
{
    _pDevice->setRestorePCConnectionFlag(state);
}

QString UNITWIN::getPathToObjectsFiles()
{
    return _pDevice->getPathToObjectsFiles();
}

void UNITWIN::setPathToObjectsFiles(QString path)
{
    _pDevice->setPathToObjectsFiles(path);
}

bool UNITWIN::testPassword(const QString& password)
{
    return _pDevice->testPassword((QString&)password);
}

void UNITWIN::save()
{
    _pDevice->save();
}

void UNITWIN::scatterIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0, QString sourceString)
{
QStringList list;
    list = sourceString.split(".");
    for(int ii=0; ii<4; ++ii)
    {
        switch(ii)
        {
            case 0:
                IPAddressPart3 = list.value(ii);
                break;
            case 1:
                IPAddressPart2 = list.value(ii);
                break;
            case 2:
                IPAddressPart1 = list.value(ii);
                break;
            default:
                IPAddressPart0 = list.value(ii);
                break;
        }
    }
}

void UNITWIN::restartCDUConection()
{
    _pDevice->restartCDUConection();
}

// для отладки
void UNITWIN::_onPathStep(int shift, unsigned int coordInMM)
{
    _pDevice->_onPathStep(shift, coordInMM, coordInMM);
}

void UNITWIN::printConnectionStatus()
{
    _pDevice->printConnectionStatus();
}

void UNITWIN::onMessage(QString s)
{
    qWarning() << s;
}

void UNITWIN::on_MessageHandler(QString s)
{
    emit messageHandlerSignal(s);
}

// в случае использования в проект вставить DEFINES += QT_MESSAGELOGCONTEXT
void UNITWIN::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
    classCs.Enter();
    if (DevicePtr != 0) DevicePtr->messageHandler(type, context, msg);
    classCs.Release();
}
