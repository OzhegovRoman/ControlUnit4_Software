#ifndef CDEVMNGR_H
#define CDEVMNGR_H

#include <QString>
#include <QList>
#include "Server/servercommands.h"
#include "consolereader.h"

class cDevMngr
{
public:
    enum Commands{
        cmd_Unknown,
        cmd_GetInterface,
        cmd_SetInterface,
        cmd_SetComPort,
        cmd_GetComPort,
        cmd_SetTcpIpAddress,
        cmd_GetTcpIpAddress,
        cmd_GetDeviceList,
        cmd_AddDevice,
        cmd_DeleteDevice,
        cmd_ClearAll,
        cmd_SaveData,
        cmd_Help,
        cmd_Exit,
        Error
    };
    cDevMngr();

    QString portName() const;
    void setPortName(const QString &portName);

    QString tcpIpAddress() const;
    void setTcpIpAddress(const QString &tcpIpAddress);

    bool isTcpIpProtocol() const;
    void setTcpIpProtocolEnabled(bool value);
    void setRs485ProtocolEnabled(bool value){setTcpIpProtocolEnabled(!value);}

    bool addDevice(int address);
    bool deleteDevice(int address);

    void initializeDeviceList();
    void updateDeviceListViaTcpIp();
    void clearDeviceList();
    void saveDeviceList();

    QList<deviceInfo> devList() const;

    void run();

private:
    bool mTcpIpProtocolEnabled {};
    QString mPortName;
    QString mTcpIpAddress;
    QList<deviceInfo> mDevList;
    ConsoleReader *reader;

    void sortDevList();
};

#endif // CDEVMNGR_H
