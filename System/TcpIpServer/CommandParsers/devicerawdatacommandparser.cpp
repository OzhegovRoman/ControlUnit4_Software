#include "devicerawdatacommandparser.h"
#include "../ctcpipserver.h"
#include "Interfaces/cuiointerface.h"
#include "star_prc_commands.h"
#include <QElapsedTimer>
#include <QCoreApplication>

DeviceRawDataCommandParser::DeviceRawDataCommandParser(QObject *parent)
    : RawDataCommandParser(parent)
    , mStopFlag(true)
{
}


bool DeviceRawDataCommandParser::doIt(const QByteArray &ba)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor is empty");
        return false;
    }

    cTcpIpServer::consoleWriteDebug("Raw command packet for DEVICE:");
    cTcpIpServer::consoleWriteDebug(QString("Data: %1").arg(ba.toHex().data()));

    // проверяем, а есть ли это устройство среди подключенных
    quint8 address = 0xFF;
    cDeviceInfo deviceInfo;
    for (int i = 0; i < executor()->settings()->deviceCount(); i++){
        if (executor()->settings()->at(i).address() == ba.at(0)){
            address = static_cast<quint8>(ba.at(0));
            deviceInfo = executor()->settings()->at(i);
        }
    }

    if (address == 0xFF) {
        cTcpIpServer::consoleWriteDebug(QString("Getting Device raw command packet with unexist device address: %1").arg(ba.at(0)));
        return false;
    }

    quint8 command = static_cast<quint8>(ba.at(1));
    quint8 dataLength = static_cast<quint8>(ba.at(2));

    switch (command) {
    case cmd::G_GetDeviceId:
        executor()->prepareAnswer(address, command,
                                 sizeof(cUDID),
                                 reinterpret_cast<char*>(deviceInfo.UDID().UDID()));
        break;
    case cmd::G_GetDeviceType:
        executor()->prepareAnswer(address, command,
                                 static_cast<quint8>(deviceInfo.type().length()),
                                 deviceInfo.type().toLocal8Bit().data());
        break;
    case cmd::G_GetModVersion:
        executor()->prepareAnswer(address, command,
                                 static_cast<quint8>(deviceInfo.modificationVersion().length()),
                                 deviceInfo.modificationVersion().toLocal8Bit().data());
        break;
    case cmd::G_GetHwVersion:
        executor()->prepareAnswer(address, command,
                                 static_cast<quint8>(deviceInfo.hardwareVersion().length()),
                                 deviceInfo.hardwareVersion().toLocal8Bit().data());
        break;
    case cmd::G_GetFwVersion:
        executor()->prepareAnswer(address, command,
                                 static_cast<quint8>(deviceInfo.firmwareVersion().length()),
                                 deviceInfo.firmwareVersion().toLocal8Bit().data());
        break;
    case cmd::G_GetDeviceDescription:
        executor()->prepareAnswer(address, command,
                                 static_cast<quint8>(deviceInfo.description().length()),
                                 deviceInfo.description().toLocal8Bit().data());
        break;

    default:
    {
        // А вот тут отправляем данные по внутреннему интерфейсу
        char* data = const_cast<char*>(ba.data() + 3);

        mStopFlag = false;
        executor()->interface()->sendMsg(address, command, dataLength, reinterpret_cast<quint8*>(data));

        QElapsedTimer timer;
        timer.start();
        while (!mStopFlag && (timer.elapsed()<1000)) //TODO: сделать правильную задержку
            qApp->processEvents();

        return true;
    }
    }
    return true;
}

void DeviceRawDataCommandParser::msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    if (mStopFlag)
        //это вобще не к нам команда пришла. Откуда то еще
        return;

    if (!executor()){
        cTcpIpServer::consoleWriteError("executor is empty");
        return;
    }

    QByteArray ba(reinterpret_cast<char*>(data), dataLength);
    cTcpIpServer::consoleWriteDebug(QString("Message received from Device. Address: %1. Command: %2. Data Length: %3. Data: %4")
                                    .arg(address)
                                    .arg(command)
                                    .arg(dataLength)
                                    .arg(ba.toHex().data()));
    mStopFlag = true;
    executor()->prepareAnswer(address, command, dataLength, reinterpret_cast<char*>(data));
}

void DeviceRawDataCommandParser::initializeParser()
{
    if (executor())
        connect(executor()->interface(), &cuIOInterface::msgReceived,
                this, &DeviceRawDataCommandParser::msgReceived);
}

