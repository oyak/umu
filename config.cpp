#include "config.h"

CONFIG::CONFIG(QString configFilePath)
{
QString iniFilePath;
QString fileName = "umu.ini";
//
    _permission = false;
    _qAttempts = 0;
//
    _CDULocalIpAddressParameterName = "CDU_LOCAL_IP_ADDRESS";
    _CDURemoteIpAddressParameterName = "CDU_REMOTE_IP_ADDRESS";

    _PCLocalIpAddressParameterName = "PC_LOCAL_IP_ADDRESS";
    _PCRemoteIpAddressParameterName = "PC_REMOTE_IP_ADDRESS";

    _restorePCConnectionParameterName = "RESTORE_PC_CONNECTION";
    _pathToObjectsFilesParameterName = "PATH_TO_OBJECTS_FILES";

    _CDULocalPortParameterName = "CDU_LOCAL_PORT";
    _CDURemotePortParameterName = "CDU_REMOTE_PORT";
    _PCLocalPortParameterName = "PC_LOCAL_PORT";
    _PCRemotePortParameterName = "PC_REMOTE_PORT";

//
    _configFilePath = configFilePath;
    if (configFilePath.isEmpty())
    {
        iniFilePath = fileName;
    }
        else
        {
            iniFilePath = _configFilePath + "/" + fileName;
        }
    pSettings = new QSettings(iniFilePath, QSettings::IniFormat);
//
    if (pSettings->contains(_CDULocalIpAddressParameterName) == false)
    {
        pSettings->setValue(_CDULocalIpAddressParameterName, "127.0.0.1");
        pSettings->sync();
    }
    setCDULocalIPAddress(pSettings->value(_CDULocalIpAddressParameterName).toString());
//
    if (pSettings->contains(_CDURemoteIpAddressParameterName) == false)
    {
        pSettings->setValue(_CDURemoteIpAddressParameterName, "127.0.0.1");
        pSettings->sync();
    }
    setCDURemoteIPAddress(pSettings->value(_CDURemoteIpAddressParameterName).toString());
//
    if (pSettings->contains(_CDULocalPortParameterName) == false)
    {
        pSettings->setValue(_CDULocalPortParameterName, "43000");
        pSettings->sync();
    }
    setCDULocalPort(pSettings->value(_CDULocalPortParameterName).toInt());
//
    if (pSettings->contains(_CDURemotePortParameterName) == false)
    {
        pSettings->setValue(_CDURemotePortParameterName, "43001");
        pSettings->sync();
    }
    setCDURemotePort(pSettings->value(_CDURemotePortParameterName).toInt());
//
//
    if (pSettings->contains(_PCLocalIpAddressParameterName) == false)
    {
        pSettings->setValue(_PCLocalIpAddressParameterName, "192.168.100.1");
        pSettings->sync();
    }
    setPCLocalIPAddress(pSettings->value(_PCLocalIpAddressParameterName).toString());
//
    if (pSettings->contains(_PCRemoteIpAddressParameterName) == false)
    {
        pSettings->setValue(_PCRemoteIpAddressParameterName, "192.168.100.3");
        pSettings->sync();
    }
    setPCRemoteIPAddress(pSettings->value(_PCRemoteIpAddressParameterName).toString());
    //
    if (pSettings->contains(_PCLocalPortParameterName) == false)
    {
        pSettings->setValue(_PCLocalPortParameterName, "50002");
        pSettings->sync();
    }
    setPCLocalPort(pSettings->value(_PCLocalPortParameterName).toInt());
//
    if (pSettings->contains(_PCRemotePortParameterName) == false)
    {
        pSettings->setValue(_PCRemotePortParameterName, "50002");
        pSettings->sync();
    }
    setPCRemotePort(pSettings->value(_PCRemotePortParameterName).toInt());
//
    if (pSettings->contains(_restorePCConnectionParameterName) == false)
    {
        pSettings->setValue(_restorePCConnectionParameterName, true);
        pSettings->sync();
    }
    _restorePCConnection = (pSettings->value(_restorePCConnectionParameterName) == true) ? true:false;
//
    if (pSettings->contains(_pathToObjectsFilesParameterName) == false)
    {
        pSettings->setValue(_pathToObjectsFilesParameterName, getConfigFilePath() + "/");
        pSettings->sync();
    }
    _pathToObjectsFiles = pSettings->value(_pathToObjectsFilesParameterName).toString();
}

CONFIG::~CONFIG()
{
    pSettings->sync();
    delete pSettings;
}

QString& CONFIG::getConfigFilePath()
{
    return _configFilePath;
}

QString& CONFIG::getCDULocalIPAddress()
{
    return _CDULocalIpAddress;
}

QString& CONFIG::getCDURemoteIPAddress()
{
    return _CDURemoteIpAddress;
}

QString& CONFIG::getPCLocalIPAddress()
{
    return _PCLocalIpAddress;
}

QString& CONFIG::getPCRemoteIPAddress()
{
    return _PCRemoteIpAddress;
}

bool CONFIG::setCDULocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    if (_permission)
    {
        return compileIPAddressString(_CDULocalIpAddress, IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
    }
        else return false;
}

bool CONFIG::setCDURemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    if (_permission)
    {
    return compileIPAddressString(_CDURemoteIpAddress, IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
    }
        else return false;
}

bool CONFIG::setPCLocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    if (_permission)
    {
        return compileIPAddressString(_PCLocalIpAddress, IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
    }
        else return false;
}

bool CONFIG::setPCRemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    if (_permission)
    {
        return compileIPAddressString(_PCRemoteIpAddress, IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
    }
        else return false;
}

void CONFIG::save()
{
    pSettings->setValue(_CDULocalIpAddressParameterName, getCDULocalIPAddress());
    pSettings->setValue(_CDURemoteIpAddressParameterName, getCDURemoteIPAddress());
    pSettings->setValue(_CDULocalPortParameterName, getCDULocalPort());
    pSettings->setValue(_CDURemotePortParameterName, getCDURemotePort());
    pSettings->setValue(_PCLocalIpAddressParameterName, getPCLocalIPAddress());
    pSettings->setValue(_PCRemoteIpAddressParameterName, getPCRemoteIPAddress());
    pSettings->setValue(_PCLocalPortParameterName, getPCLocalPort());
    pSettings->setValue(_PCRemotePortParameterName, getPCRemotePort());
    pSettings->setValue(_restorePCConnectionParameterName, getRestorePCConnectionFlagState());
    pSettings->setValue(_pathToObjectsFilesParameterName, getPathToObjectsFiles());
    pSettings->sync();
}

bool CONFIG::getRestorePCConnectionFlagState()
{
    return _restorePCConnection;
}

void CONFIG::setRestorePCConnectionFlag(bool state)
{
    _restorePCConnection = state;
}

QString CONFIG::getPathToObjectsFiles()
{
    return _pathToObjectsFiles;
}

void CONFIG::setPathToObjectsFiles(QString path)
{
    _pathToObjectsFiles = path;
}

bool CONFIG::testPassword(QString& password)
{
bool res;
    if (_qAttempts < 10) res = true;
        else res = false;
    if ((!_permission) && (_qAttempts < 10))
    {
        if (1) // (password == "")
        {
            _permission = true;
        }
            else
            {
                _qAttempts++;
                res = false;
            }
    }
    return res;
}

void CONFIG::setCDULocalPort(unsigned short port)
{
    _CDULocalPort = port;
}

void CONFIG::setCDURemotePort(unsigned short port)
{
    _CDURemotePort = port;
}

void CONFIG::setPCLocalPort(unsigned short port)
{
    _PCLocalPort = port;
}

void CONFIG::setPCRemotePort(unsigned short port)
{
    _PCRemotePort = port;
}

bool CONFIG::setCDULocalPort(QString portString)
{
bool res = true;
unsigned short port;
    if (!portString.isEmpty())
    {
         port = portString.toUShort(&res);
    }
        else port = 0;
    if (res) _CDULocalPort = port;
    return res;
}

bool CONFIG::setCDURemotePort(QString portString)
{
bool res = true;
unsigned short port;
    if (!portString.isEmpty())
    {
         port = portString.toUShort(&res);
    }
        else port = 0;
    if (res) _CDURemotePort = port;
    return res;
}

bool CONFIG::setPCLocalPort(QString portString)
{
bool res = true;
unsigned short port;
    if (!portString.isEmpty())
    {
        port = portString.toUShort(&res);
    }
        else port = 0;

    if (res) _PCLocalPort = port;
    return res;
}

bool CONFIG::setPCRemotePort(QString portString)
{
bool res = true;
unsigned short port;
    if (!portString.isEmpty())
    {
        port = portString.toUShort(&res);
    }
        else port = 0;
    if (res) _PCRemotePort = port;
    return res;
}

unsigned short CONFIG::getCDULocalPort()
{
    return _CDULocalPort;
}

unsigned short CONFIG::getCDURemotePort()
{
    return _CDURemotePort;
}

unsigned short CONFIG::getPCLocalPort()
{
    return _PCLocalPort;
}

unsigned short CONFIG::getPCRemotePort()
{
    return _PCRemotePort;
}



bool CONFIG::compileIPAddressString(QString& destinationString, QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
bool res = true;
unsigned int num;
    if (!IPAddressPart3.isEmpty())
    {
        num = IPAddressPart3.toUInt(&res);
    }
        else
        {
            num = 0;
            IPAddressPart3.setNum(0);
        }
    if ((res) && (num < 255))
    {
        if (!IPAddressPart2.isEmpty())
        {
            num = IPAddressPart2.toUInt(&res);
        }
           else
           {
               num = 0;
               IPAddressPart2.setNum(0);
           }
        if ((res) && (num < 255))
        {
            if (!IPAddressPart1.isEmpty())
            {
                num = IPAddressPart1.toUInt(&res);
            }
                else
                {
                    num = 0;
                    IPAddressPart1.setNum(0);
                }
            if ((res) && (num < 255))
            {
                if (!IPAddressPart0.isEmpty())
                {
                    num = IPAddressPart0.toUInt(&res);
                }
                    else
                    {
                        num = 0;
                        IPAddressPart0.setNum(0);
                    }
                if ((res) && (num < 255))
                {
                    destinationString = IPAddressPart3 + "."
                        + IPAddressPart2 + "." \
                        + IPAddressPart1 + "." \
                        + IPAddressPart0;
                }
            }
        }

    }
    return res;
}




void CONFIG::setCDULocalIPAddress(QString IPAddressString)
{
    _CDULocalIpAddress = IPAddressString;
}

void CONFIG::setCDURemoteIPAddress(QString IPAddressString)
{
    _CDURemoteIpAddress = IPAddressString;
}

void CONFIG::setPCLocalIPAddress(QString IPAddressString)
{
    _PCLocalIpAddress = IPAddressString;
}

void CONFIG::setPCRemoteIPAddress(QString IPAddressString)
{
    _PCRemoteIpAddress = IPAddressString;
}




void setCDULocalPort(unsigned short port);
void setCDURemotePort(unsigned short port);
void setPCLocalPort(unsigned short port);
void setPCRemotePort(unsigned short port);

