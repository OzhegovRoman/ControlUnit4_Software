#include "ccommandexecutor.h"
#include <QThread>
#include <QCoreApplication>
#include <QSettings>
#include "ctcpipserver.h"
#include "Server/servercommands.h"
#include "Drivers/adriver.h"
#include "settingsprovider.h"

cCommandExecutor::cCommandExecutor(QObject *parent)
    : QObject(parent)
    , mStopFlag(false)
    , mInterface(nullptr)
    , processTimer(new QTimer(this))
    , settings(new SettingsProvider(this))
{
    processTimer->setInterval(10);
    processTimer->setSingleShot(true);
    connect(processTimer, &QTimer::timeout, this, &cCommandExecutor::process);
}

void cCommandExecutor::doWork()
{
    cTcpIpServer::consoleWriteDebug("Command executor started");

    initialize();
    processTimer->start();
}

void cCommandExecutor::stop()
{
    cTcpIpServer::consoleWriteDebug("Command executor get STOP signal");
    mStopFlag = true;
    emit finished();
}

void cCommandExecutor::executeCommand(QObject *tcpIpProcess, QByteArray data)
{
    cTcpIpServer::consoleWrite(QString("New command execute"));
    cmdList.append(Command(tcpIpProcess, data));
}

void cCommandExecutor::process()
{
    // выполняем команду, после чего пускаем все заново
    if (!cmdList.isEmpty()){
        // пришла команда
        cTcpIpServer::consoleWriteDebug(QString("CmdList buffer size: %1. Start to execute command.").arg(cmdList.count()));

        // проверяем есть ли еще в стеке команды, если есть, то удаляем и переходим у следующей команде
        bool sameProcessFounded = false;
        for (auto iter = cmdList.begin() + 1; iter < cmdList.end(); ++iter){
            if (iter->process() == cmdList.begin()->process()){
                sameProcessFounded = true;
                break;
            }
        }

        if (!sameProcessFounded)
            parse(cmdList.begin()->data());

        // в конце удаляем команду из начала списка команд
        cmdList.removeAt(0);
    }
    processTimer->start();
}

cuIOInterface *cCommandExecutor::interface() const
{
    return mInterface;
}

void cCommandExecutor::setInterface(cuIOInterface *interface)
{
    mInterface = interface;
}

void cCommandExecutor::initialize()
{
    settings->prepareDeviceInfoList();

    //activate all devices
    for (int i = 0; i < settings->deviceCount();){
        const cDeviceInfo info = settings->at(i);
        if (!checkDevice(info)){
            cTcpIpServer::consoleWriteError(QString("initialization device with address %1 FAILED.").arg(info.address()));
            settings->remove(i);
        }
        else {
            cTcpIpServer::consoleWriteDebug(QString("initialization device with address %1 SUCCESS.").arg(info.address()));
            i++;
        }
    }

    cTcpIpServer::consoleWriteDebug(QString("Initialized devices: %1").arg(settings->deviceCount()));
    emit inited();
}

void cCommandExecutor::parse(const QByteArray &data)
{
    cTcpIpServer::consoleWriteDebug("cCommandExecutor::parse().");
    if (isRawData(data)){
        cTcpIpServer::consoleWriteDebug("Raw command packet");
        // смотрим адрес устройства
        if (static_cast<quint8>(data.at(0)) == SERVER_ADDRESS){
            cTcpIpServer::consoleWriteDebug(QString("SERVER: %1").arg(data.toHex().data()));
            // это запрос к серверу
            //TODO: serverRequest
            serverRequest(data);
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

    cTcpIpServer::consoleWriteDebug(QString("SCPI command: %1 Params: %2").arg(cmd).arg(params));
}

bool cCommandExecutor::isRawData(const QByteArray &ba)
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

bool cCommandExecutor::checkDevice(const cDeviceInfo &info)
{
    if (!mInterface)
        return false;

    AbstractDriver driver;
    driver.setIOInterface(mInterface);
    driver.setDevAddress(info.address());

    // Проверяем Id устройства,
    // Более ничего не проверяем, считаем что UDID уникален и 2 устройства с одинаковым UDID не может быть
    // как следствие UDID точно соответствует его Type и т.д.
    bool ok;
    cUDID UDID = driver.getUDID()->getValueSequence(&ok, 10); // крайне важная операция будем пробовать аж до 10 раз
    if (!ok) return false;
    if (info.UDID().toString() == UDID.toString()) return true;
    return false;
}

void cCommandExecutor::serverRequest(const QByteArray &packet)
{
    // команда серверу
    // здесь будет обработка его команд
    //    qDebug()<<"Server request";
    quint8 command = static_cast<quint8>(packet[1]);
    const char* data = packet.data() + 3;

    switch (command) {
    // полный список всех доступных устройств
    case CMD_SERVER_GET_DEVICE_LIST:
    {
        QString tmpStr;
        tmpStr.clear();
        tmpStr.append(QString("DevCount: %1").arg(settings->deviceCount()));
        for (int i = 0; i < settings->deviceCount(); ++i){
            const cDeviceInfo &info= settings->at(i);
            tmpStr.append(";<br>");
            tmpStr.append(QString("Dev%1: address=%2: type=%3")
                          .arg(i)
                          .arg(info.address())
                          .arg(info.type()));
        }
        prepareAnswer(SERVER_ADDRESS, command, static_cast<quint8>(tmpStr.count()), tmpStr.toLocal8Bit().data());
        break;
    }
    case CMD_SERVER_SEARCH_DEVICES:
    {
        for (quint8 i = 0; i < MAX_COUNT_DEVICES; ++i)
            addDevice(i);
        char tmpUint8 = static_cast<char>(settings->deviceCount());
        prepareAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }
    case CMD_SERVER_ADD_DEVICE:
    {
        char tmpUint8 = static_cast<char>(addDevice(*reinterpret_cast<const quint8*>(data)));
        prepareAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }
    case CMD_SERVER_SAVE_DEVICE_LIST:
    {
        char tmpUint8 = 1;
        settings->saveDeviceInformation();
        prepareAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }
    default:
        break;
    }
}

void cCommandExecutor::prepareAnswer(quint8 address, quint8 command, quint8 dataLength, char *data)
{
    cTcpIpServer::consoleWriteDebug("Preparing answer");

    QByteArray ba;
    ba.append(*reinterpret_cast<char*>(&address));
    ba.append(*reinterpret_cast<char*>(&command));
    ba.append(*reinterpret_cast<char*>(&dataLength));
    ba.append(data, dataLength);

    cTcpIpServer::consoleWriteDebug(QString("Answer %1").arg(data));

    emit sendAnswer(cmdList.begin()->process(), ba);
}

bool cCommandExecutor::addDevice(quint8 address)
{
    cTcpIpServer::consoleWriteDebug(QString("Add device with address %1").arg(address));
    settings->removeDeviceWithAddress(address);

    AbstractDriver driver;
    driver.setIOInterface(mInterface);
    driver.setDevAddress(address);
    cDeviceInfo info;
    bool ok = false;
    info.setAddress(address);
    info.setType(driver.getDeviceType()->getValueSequence(&ok, 5));
    if (!ok) return false;

    info.setUDID(driver.getUDID()->getValueSequence(&ok,5));
    if (!ok) return false;

    info.setModificationVersion(driver.getModificationVersion()->getValueSequence(&ok, 5));
    if (!ok) return false;

    info.setHardwareVersion(driver.getHardwareVersion()->getValueSequence(&ok, 5));
    if (!ok) return false;

    info.setFirmwareVersion(driver.getFirmwareVersion()->getValueSequence(&ok, 5));
    if (!ok) return false;

    info.setDescription(driver.getDeviceDescription()->getValueSequence(&ok, 5));
    if (!ok) return false;

    settings->append(info);
    cTcpIpServer::consoleWriteDebug(QString("SUCCESS"));
    return true;
}
