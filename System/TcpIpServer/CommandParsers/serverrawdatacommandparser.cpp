#include "serverrawdatacommandparser.h"
#include "../ctcpipserver.h"
#include "../StarProtocol/servercommands.h"
#include "../StarProtocol/star_prc_commands.h"
#include "cuiodevice.h"

ServerRawDataCommandParser::ServerRawDataCommandParser(QObject *parent)
    : RawDataCommandParser(parent)
{

}

bool ServerRawDataCommandParser::doIt(const QByteArray &ba)
{
    if (static_cast<quint8>(ba.at(0)) != SERVER_ADDRESS) return false;
    // это запрос к серверу

    if (!executor()){
        cTcpIpServer::consoleWriteError("executor is empty");
    }

    cTcpIpServer::consoleWriteDebug("Raw command packet for SERVER");
    cTcpIpServer::consoleWriteDebug(QString("%1").arg(ba.toHex().data()));

    quint8 command = static_cast<quint8>(ba.at(1));
    const char* data = ba.data() + 3;

    switch (command) {
    // полный список всех доступных устройств
    case CMD_SERVER_GET_DEVICE_LIST:
    {
        QString tmpStr;
        tmpStr.clear();
        tmpStr.append(QString("DevCount: %1").arg(executor()->settings()->deviceCount()));
        for (int i = 0; i < executor()->settings()->deviceCount(); ++i){
            const cDeviceInfo &info= executor()->settings()->at(i);
            tmpStr.append(";<br>");
            tmpStr.append(QString("Dev%1: address=%2: type=%3")
                          .arg(i)
                          .arg(info.address())
                          .arg(info.type()));
        }
        executor()->prepareAnswer(SERVER_ADDRESS, command, static_cast<quint8>(tmpStr.count()), tmpStr.toLocal8Bit().data());
        break;
    }
    case CMD_SERVER_SEARCH_DEVICES:
    {
        for (quint8 i = 0; i < MAX_COUNT_DEVICES; ++i)
            executor()->addDevice(i);
        char tmpUint8 = static_cast<char>(executor()->settings()->deviceCount());
        executor()->prepareAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }
    case CMD_SERVER_ADD_DEVICE:
    {
        char tmpUint8 = static_cast<char>(executor()->addDevice(*reinterpret_cast<const quint8*>(data)));
        executor()->prepareAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }
    case CMD_SERVER_SAVE_DEVICE_LIST:
    {
        char tmpUint8 = 1;
        executor()->settings()->saveDeviceInformation();
        executor()->prepareAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }

    case CMD_SERVER_PIPE_MODE_ON:
    {
        executor()->setPipeMode(true);
        executor()->prepareAnswer(SERVER_ADDRESS, command, 0 , nullptr);
        break;
    }
    case CMD_SERVER_PIPE_MODE_OFF:{
        executor()->setPipeMode(false);
        executor()->prepareAnswer(SERVER_ADDRESS, command, 0 , nullptr);
        break;
    }

    default:
        return false;
    }
    return true;
}

void ServerRawDataCommandParser::msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor is empty");
        return;
    }

    if (!executor()->isPipeMode())
        //это вобще не к нам команда пришла. Откуда то еще
        return;

    static int i =0;

    if (command == cmd::BS_SweepData){
        QByteArray ba(reinterpret_cast<char*>(data), dataLength);
        cTcpIpServer::consoleWriteDebug(QString("Message received from Device. Address: %1. Command: %2. Data Length: %3. Data: %4")
                                        .arg(address)
                                        .arg(command)
                                        .arg(dataLength)
                                        .arg(ba.toHex().data()));

        executor()->prepareAnswer(address, command, dataLength, reinterpret_cast<char*>(data));
    }
}

void ServerRawDataCommandParser::initializeParser()
{
    if (executor())
        connect(executor()->interface(), &cuIOInterface::msgReceived,
                this, &ServerRawDataCommandParser::msgReceived);
}
