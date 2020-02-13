
#include "unitlin.h"
#include "QDebug"
#include "assert.h"

class UMUDEVICE *UNITLIN::DevicePtr;
class cCriticalSection_Lin UNITLIN::classCs;

UNITLIN::UNITLIN(CONFIG *pConfig)
{
    _pDevice = new UMUDEVICE(&_thList, (void*)this, pConfig);
    DevicePtr = _pDevice;
     connect(_pDevice, SIGNAL(CDUConnected()), this, SLOT(on_CDU_connected()));
//    connect(_pDevice, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
     connect(_pDevice, SIGNAL(messageHandlerSignal(QString)), this, SLOT(on_MessageHandler(QString)));
}

UNITLIN::~UNITLIN()
{
QVector <cCriticalSection_Lin*>::iterator it;
    disconnect(_pDevice, SIGNAL(CDUConnected()), this, SLOT(on_CDU_connected()));
//  disconnect(_pDevice, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
    classCs.Enter();
    delete _pDevice;
    DevicePtr = 0;
    classCs.Release();
    for (it = _cs.begin(); it < _cs.end(); ++it) delete *it;
    _cs.clear();
}

void UNITLIN::start()
{
    _pDevice->start();
}

void UNITLIN::stop()
{
    _pDevice->stop();
}

cCriticalSection_Lin *UNITLIN::createCriticalSection()
{
cCriticalSection_Lin *cs;
    vectorCs.Enter();
    cs = new cCriticalSection_Lin;
    _cs.push_back(cs);
    vectorCs.Release();
    return cs;
}

void UNITLIN::criticalSectionEnter(cCriticalSection_Lin* cs)
{
    cs->Enter();
}

void UNITLIN::criticalSectionRelease(cCriticalSection_Lin *cs)
{
    cs->Release();
}

void UNITLIN::on_CDU_connected()
{
    emit CDUconnected();
}

//
QString& UNITLIN::getCDULocalIPAddress()
{
    return _pDevice->getCDULocalIPAddress();
}

QString& UNITLIN::getCDURemoteIPAddress()
{
    return _pDevice->getCDURemoteIPAddress();
}

bool UNITLIN::setCDULocalIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setCDULocalIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UNITLIN::setCDURemoteIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setCDURemoteIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

QString& UNITLIN::getPCLocalIPAddress()
{
    return _pDevice->getPCLocalIPAddress();
}

QString& UNITLIN::getPCRemoteIPAddress()
{
    return _pDevice->getPCRemoteIPAddress();
}

bool UNITLIN::setPCLocalIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setPCLocalIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UNITLIN::setPCRemoteIPAddress(QString IPAddressPart3, QString IPAddressPart2, QString IPAddressPart1, QString IPAddressPart0)
{
    return _pDevice->setPCRemoteIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UNITLIN::setCDULocalPort(QString port)
{
    return _pDevice->setCDULocalPort(port);
}

bool UNITLIN::setCDURemotePort(QString port)
{
    return _pDevice->setCDURemotePort(port);
}

bool UNITLIN::setPCLocalPort(QString  port)
{
    return _pDevice->setPCLocalPort(port);
}

bool UNITLIN::setPCRemotePort(QString  port)
{
    return _pDevice->setPCRemotePort(port);
}

unsigned short UNITLIN::getCDULocalPort()
{
    return _pDevice->getCDULocalPort();
}

unsigned short UNITLIN::getCDURemotePort()
{
    return _pDevice->getCDURemotePort();
}

unsigned short UNITLIN::getPCLocalPort()
{
    return _pDevice->getPCLocalPort();
}

unsigned short UNITLIN::getPCRemotePort()
{
    return _pDevice->getPCRemotePort();
}

bool UNITLIN::getRestorePCConnectionFlagState()
{
    return _pDevice->getRestorePCConnectionFlagState();
}

void UNITLIN::setRestorePCConnectionFlag(bool state)
{
    _pDevice->setRestorePCConnectionFlag(state);
}

QString UNITLIN::getPathToObjectsFiles()
{
    return _pDevice->getPathToObjectsFiles();
}

void UNITLIN::setPathToObjectsFiles(QString path)
{
    _pDevice->setPathToObjectsFiles(path);
}

bool UNITLIN::testPassword(const QString& password)
{
    return _pDevice->testPassword((QString&)password);
}

void UNITLIN::save()
{
    _pDevice->save();
}

void UNITLIN::scatterIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0, QString sourceString)
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

void UNITLIN::restartCDUConection()
{
    _pDevice->restartCDUConection();
}

// для отладки
void UNITLIN::_onPathStep(int shift, unsigned int coordInMM)
{
    _pDevice->_onPathStep(shift, coordInMM, coordInMM);
}

void UNITLIN::printConnectionStatus()
{
    _pDevice->printConnectionStatus();
}

void UNITLIN::onMessage(QString s)
{
    qWarning() << s;
}

void UNITLIN::on_MessageHandler(QString s)
{
    emit messageHandlerSignal(s);
}

// в случае использования в проект вставить DEFINES += QT_MESSAGELOGCONTEXT
void UNITLIN::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
    classCs.Enter();
    if (DevicePtr != 0) DevicePtr->messageHandler(type, context, msg);
    classCs.Release();
}
