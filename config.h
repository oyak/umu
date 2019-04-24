#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSettings>


class CONFIG
{
public:
    CONFIG(QString configFilePath);
    ~CONFIG();

    QString& getConfigFilePath();

    QString& getCDULocalIPAddress();
    QString& getCDURemoteIPAddress();
    bool setCDULocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);
    bool setCDURemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);
    QString& getPCLocalIPAddress();
    QString& getPCRemoteIPAddress();
    bool setPCLocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);
    bool setPCRemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);

    bool setCDULocalPort(QString portString);
    bool setCDURemotePort(QString portString);
    bool setPCLocalPort(QString portString);
    bool setPCRemotePort(QString portString);

    void setCDULocalPort(unsigned short port);
    void setCDURemotePort(unsigned short port);
    void setPCLocalPort(unsigned short port);
    void setPCRemotePort(unsigned short port);

    unsigned short getCDULocalPort();
    unsigned short getCDURemotePort();
    unsigned short getPCLocalPort();
    unsigned short getPCRemotePort();

    bool getRestorePCConnectionFlagState();
    void setRestorePCConnectionFlag(bool state);

    QString getPathToObjectsFiles();
    void setPathToObjectsFiles(QString path);

    bool testPassword(QString& password);
    void save();

private:

    QString _configFilePath;

    QString _CDULocalIpAddress;
    QString _CDURemoteIpAddress;

    QString _PCLocalIpAddress;
    QString _PCRemoteIpAddress;

    unsigned short _CDULocalPort;
    unsigned short _CDURemotePort;
    unsigned short _PCLocalPort;
    unsigned short _PCRemotePort;


    bool _restorePCConnection;
    QString _pathToObjectsFiles;

    QString _CDULocalIpAddressParameterName;
    QString _CDURemoteIpAddressParameterName;

    QString _PCLocalIpAddressParameterName;
    QString _PCRemoteIpAddressParameterName;

    QString _restorePCConnectionParameterName;
    QString _pathToObjectsFilesParameterName;

    QString _CDULocalPortParameterName;
    QString _CDURemotePortParameterName;
    QString _PCLocalPortParameterName;
    QString _PCRemotePortParameterName;


    void setCDULocalIPAddress(QString IPAddressString);
    void setCDURemoteIPAddress(QString IPAddressString);
    void setPCLocalIPAddress(QString IPAddressString);
    void setPCRemoteIPAddress(QString IPAddressString);


    bool compileIPAddressString(QString& destinationString, QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);
    bool _permission;
    unsigned int _qAttempts;
    QSettings *pSettings;
};

#endif // CONFIG_H
