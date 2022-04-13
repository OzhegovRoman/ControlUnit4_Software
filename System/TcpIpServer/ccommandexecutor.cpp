#include "ccommandexecutor.h"
#include <QThread>
#include <QCoreApplication>
#include <QSettings>
#include "ctcpipserver.h"
#include "../StarProtocol/servercommands.h"
#include "Drivers/commondriver.h"
#include "CommandParsers/commandparser.h"

cCommandExecutor::cCommandExecutor(QObject *parent)
    : QObject(parent)
    , mInterface(nullptr)
    , processTimer(nullptr)
    , mSettings(new SettingsProvider(this))
    , mStopFlag(false)
    , mPipeMode(false)
{
    parsers << new ServerRawDataCommandParser(this);
    parsers << new DeviceRawDataCommandParser(this);
    parsers << new CommonScpiCommands(this);
    parsers << new SystemScpiCommands(this);
    parsers << new GeneralScpiCommands(this);
    parsers << new SspdScpiCommands(this);
    parsers << new TemperatureScpiCommands(this);
    parsers << new HeaterScpiCommands(this);
    parsers << new SisControlLineScpiCommands(this);
    parsers << new SisBiasSourceScpiCommandParser(this);
}

void cCommandExecutor::doWork()
{
    cTcpIpServer::consoleWriteDebug("Command executor started");
    initialize();

    processTimer = new QTimer(this);
    processTimer->setInterval(10);
    processTimer->setSingleShot(true);
    connect(processTimer, &QTimer::timeout, this, &cCommandExecutor::process);
    process();
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
    cmdList.append(qMakePair(tcpIpProcess, data));
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
            if (iter->first == cmdList.begin()->first){
                sameProcessFounded = true;
                break;
            }
        }

        if (!sameProcessFounded){
            bool done = false;
            if (!mPipeMode)
                currentTcpIpProcess = cmdList.begin()->first;
            if ((mPipeMode && (currentTcpIpProcess == cmdList.begin()->first)) ||
                    !mPipeMode) {
                for (auto iter = parsers.begin(); iter<parsers.end() && !done; iter++)
                    done = (*iter)->parse(cmdList.begin()->second);

                if (!done)
                    prepareAnswer("UNKNOWN COMMAND\r\n");
            }
        }

        // в конце удаляем команду из начала списка команд
        cmdList.removeAt(0);
    }
    processTimer->start();
}

bool cCommandExecutor::isPipeMode() const
{
    return mPipeMode;
}

void cCommandExecutor::setPipeMode(bool pipeMode)
{
    mPipeMode = pipeMode;
}

SettingsProvider *cCommandExecutor::settings() const
{
    return mSettings;
}

void cCommandExecutor::setSettings(SettingsProvider *value)
{
    mSettings = value;
}

void cCommandExecutor::moveToThread(QThread *thread)
{
    mInterface->moveToThread(thread);
    QObject::moveToThread(thread);
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
    cTcpIpServer::consoleWriteDebug("Initialize Comand Executor");

    mSettings->prepareDeviceInfoList();

    //activate all devices
    for (int i = 0; i < mSettings->deviceCount();){
        const cDeviceInfo info = mSettings->at(i);
        if (!checkDevice(info)){
            cTcpIpServer::consoleWriteError(QString("initialization device with address %1 FAILED.").arg(info.address()));
            mSettings->remove(i);
        }
        else {
            cTcpIpServer::consoleWriteDebug(QString("initialization device with address %1 SUCCESS.").arg(info.address()));
            i++;
        }
    }

    cTcpIpServer::consoleWriteDebug("test Point");


    for (auto iter = parsers.begin(); iter < parsers.end(); iter++)
        (*iter)->initializeParser();

    cTcpIpServer::consoleWriteDebug(QString("Initialized devices: %1").arg(mSettings->deviceCount()));
    emit inited();
}

bool cCommandExecutor::checkDevice(const cDeviceInfo &info)
{
    cTcpIpServer::consoleWriteDebug(QString("Check Device: %1").arg(info.address()));

    if (!mInterface)
        return false;

    CommonDriver driver;
    driver.setIOInterface(mInterface);
    driver.setDevAddress(info.address());

    // Проверяем Id устройства,
    // Более ничего не проверяем, считаем что UDID уникален и 2 устройства с одинаковым UDID не может быть
    // как следствие UDID точно соответствует его Type и т.д.
    bool ok;

    cTcpIpServer::consoleWriteDebug(QString("Try to get UDID"));
    cUDID UDID = driver.UDID()->getValueSync(&ok, 10); // крайне важная операция будем пробовать аж до 10 раз
    cTcpIpServer::consoleWriteDebug(QString("Result: %1").arg(ok ? "success" : "failed"));
    if (!ok) return false;
    cTcpIpServer::consoleWriteDebug(QString("UDID: %1").arg(UDID.toString()));
    if (info.UDID().toString() == UDID.toString()) return true;
    return false;
}

void cCommandExecutor::prepareAnswer(quint8 address, quint8 command, quint8 dataLength, char *data)
{
    cTcpIpServer::consoleWriteDebug("Preparing answer");

    QByteArray ba;
    ba.append(*reinterpret_cast<char*>(&address));
    ba.append(*reinterpret_cast<char*>(&command));
    ba.append(*reinterpret_cast<char*>(&dataLength));
    ba.append(data, dataLength);

    if (command == cmd::BS_SweepData){
    }

    cTcpIpServer::consoleWriteDebug(QString("Answer: %1").arg(ba.toHex().data()));

    emit sendAnswer(currentTcpIpProcess, ba);
}

void cCommandExecutor::prepareAnswer(QString answer)
{
    cTcpIpServer::consoleWriteDebug("Preparing answer as string");
    cTcpIpServer::consoleWriteDebug(QString("Answer: %1").arg(answer));

    emit sendAnswer(currentTcpIpProcess, QByteArray(answer.toLocal8Bit()));
}

bool cCommandExecutor::addDevice(quint8 address)
{
    cTcpIpServer::consoleWriteDebug(QString("Add device with address %1").arg(address));
    mSettings->removeDeviceWithAddress(address);

    CommonDriver driver;
    driver.setIOInterface(mInterface);
    driver.setDevAddress(address);
    cDeviceInfo info;
    bool ok = false;
    info.setAddress(address);
    info.setType(driver.deviceType()->getValueSync(&ok, 5));
    if (!ok) return false;

    info.setUDID(driver.UDID()->getValueSync(&ok,5));
    if (!ok) return false;

    info.setModificationVersion(driver.modificationVersion()->getValueSync(&ok, 5));
    if (!ok) return false;

    info.setHardwareVersion(driver.hardwareVersion()->getValueSync(&ok, 5));
    if (!ok) return false;

    info.setFirmwareVersion(driver.firmwareVersion()->getValueSync(&ok, 5));
    if (!ok) return false;

    info.setDescription(driver.deviceDescription()->getValueSync(&ok, 5));
    if (!ok) return false;

    mSettings->append(info);
    cTcpIpServer::consoleWriteDebug(QString("SUCCESS"));
    return true;
}
