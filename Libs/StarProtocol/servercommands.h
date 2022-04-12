#ifndef SERVERCOMMANDS_H
#define SERVERCOMMANDS_H

#define SERVER_TCPIP_PORT                  9876

#define SERVER_ADDRESS              0xFF
#define MAX_DEVICE_TRY_COUNT        5

#define CMD_SERVER_GET_DEVICE_LIST  0
#define CMD_SERVER_SEARCH_DEVICES   1
#define CMD_SERVER_ADD_DEVICE       2
#define CMD_SERVER_SAVE_DEVICE_LIST 3

#define CMD_SERVER_PIPE_MODE_ON     4
#define CMD_SERVER_PIPE_MODE_OFF    5

#include <QObject>
#include "../CuPlugins/cudid.h"

struct deviceInfo {
    quint8  devAddress;
    QString devType;
    cUDID   devUDID;
    QString devModVersion;
    QString devHwVersion;
    QString devFwVersion;
    QString devDescription;
};

#endif // SERVERCOMMANDS_H
