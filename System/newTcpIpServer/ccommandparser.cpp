#include "ccommandparser.h"
#include "ctcpipserver.h"
#include "Server/servercommands.h"
#include <QSettings>
#include "cdeviceinfo.h"

cCommandParser::cCommandParser(QObject *parent)
    : QObject(parent)
    , mInited(false)
{

}

void cCommandParser::initialize()
{
    cTcpIpServer::consoleWriteDebug("Command Parser initialization");
    prepareDeviceInfoList();
    // начинаем процесс проверки каждого элемента через отправку команд в executor и чтения ответов
}

void cCommandParser::parse(QObject *tcpIpProcess, QByteArray data)
{
    cTcpIpServer::consoleWriteDebug("cCommandParser::parse().");
    if (isRawData(data)){
        cTcpIpServer::consoleWriteDebug("Raw command packet");
        // смотрим адрес устройства
        if (static_cast<quint8>(data.at(0)) == SERVER_ADDRESS){
            cTcpIpServer::consoleWriteDebug(QString("SERVER: %1").arg(data.toHex().data()));
            // это запрос к серверу
            //TODO: serverRequest
            //serverRequest(tcpIpProcess, data);
        }
        else {
            cTcpIpServer::consoleWriteDebug(QString("DEVICE: %1").arg(data.toHex().data()));
            //ToDO: Device Request
            // а это запрос к Устройству
//            deviceInfo *info = currentDevice(data.second[0]);
//            // если такого устройства совсем нет, то и не будем обрабатывать это устройство
//            if (info != nullptr){
//                deviceRequest(info, ba);
//            }
        }
        return;
    }
    // Разбор SCPI команды
    QString tmpStr = QString(data).simplified();
    QStringList cmdList = tmpStr.split(" ");
    QString cmd = cmdList[0];
    QString params;
    if (cmdList.count()>1) {
        cmdList.removeFirst();
        params = cmdList.join(" ");
    }

    //удаляем все прописные символы из команды
    int i = 0;
    while (i<cmd.size()){
        if (cmd[i].isLower()) cmd.remove(i,1);
        else i++;
    }

    cTcpIpServer::consoleWriteDebug(QString("SCPI command: %1. Params: %2").arg(cmd).arg(params));

}

bool cCommandParser::isInited() const
{
    return mInited;
}

bool cCommandParser::isRawData(const QByteArray &ba)
{
    /// проверка на несоответствие протоколу SCPI
    /// основная тонкость заключается в том, что в SCPI используется только текстовая информация
    /// а в обычном пакете данных первый символ - не соответствует текстовому символу, на этом и производим сверку протоколов
    /// делаем расшифровку команд
    if ((ba.size() > 2) &&
        (ba.size() > ba[2] + 2))
            return true;
    return false;
}

void cCommandParser::prepareDeviceInfoList()
{
    cTcpIpServer::consoleWriteDebug("CommandParser: preparing DeviceInfoList");
    QSettings settings("Scontel", "RaspPi Server");
    int size = settings.beginReadArray("devices");
    mDeviceInfoList.clear();
    for (int i = 0; i < size; ++i){
        cDeviceInfo deviceInfo;
        settings.setArrayIndex(i);
        deviceInfo.setAddress(static_cast<quint8>(settings.value("devAddress", 255).toInt()));
        deviceInfo.setType(settings.value("devType","None").toString());
        deviceInfo.setDescription(settings.value("devDescription", "None").toString());
        deviceInfo.setUDID(cUDID(reinterpret_cast<quint8*>(settings.value("devUDID","0").toByteArray().data())));
        deviceInfo.setFirmwareVersion(settings.value("devFwVersion", "None").toString());
        deviceInfo.setHardwareVersion(settings.value("devHwVersion", "None").toString());
        deviceInfo.setModificationVersion(settings.value("devModVersion", "None").toString());
        mDeviceInfoList.append(deviceInfo);
    }
    settings.endArray();
    cTcpIpServer::consoleWriteDebug(QString("Loaded devices from SETTINGS: %1").arg(mDeviceInfoList.size()));
}
