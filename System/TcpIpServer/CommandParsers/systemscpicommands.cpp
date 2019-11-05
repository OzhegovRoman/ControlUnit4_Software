#include "systemscpicommands.h"
#include "../ctcpipserver.h"
#include "Drivers/adriver.h"

SystemScpiCommands::SystemScpiCommands(QObject *parent)
    : CommonScpiCommands(parent)
{

}

/** формат строки
 * SYSTem:DEViceList? - device list
 * SYSTem:DEViceList:ADD <N> - добавить новый, N - адрес
 * SYSTem:DEViceList:FIND - поиск новых устройств. Очень долгая команда
 * SYSTem:DEViceList:SAVE - сохранить список устройств
 */
bool SystemScpiCommands::executeCommand(QString command, QString params)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }

    if (command.indexOf("SYST:") != 0)
        return false;

    QStringList strList = command.split(":");
    strList.removeFirst();

    if (strList.count() == 0) //команда неправильная
        return false;

    if (strList[0] == "DEVL?"){
        QString tmpStr;
        tmpStr.clear();
        tmpStr.append(QString("DevCount: %1\r\n").arg(executor()->settings()->deviceCount()));
        for (int i = 0; i < executor()->settings()->deviceCount(); ++i){
            tmpStr.append(";<br>");
            tmpStr.append(QString("Dev%1: address=%2: type=%3\r\n")
                          .arg(i)
                          .arg(executor()->settings()->at(i).address())
                          .arg(executor()->settings()->at(i).type()));
        }
        executor()->prepareAnswer(tmpStr);
        return true;
    }

    if (strList[0] != "DEVL")
        return false;

    strList.removeFirst();      // удаляем DEVL
    if (strList.count() == 0)   //команда неправильная
        return false;

    if (strList[0] == "SAVE"){
        executor()->prepareAnswer("OK\r\n");
        executor()->settings()->saveDeviceInformation();
        return true;
    }

    if (strList[0] == "FIND"){
        for (quint8 i = 0; i < MAX_COUNT_DEVICES; ++i)
            executor()->addDevice(i);
        int devCount = executor()->settings()->deviceCount();
        executor()->prepareAnswer(QString("Was founded %1 devices\r\n").arg(devCount));
        return true;
    }

    if (strList[0] == "ADD"){
        bool ok;
        quint8 address = static_cast<quint8>(params.toInt(&ok));
        if (!ok) return false;
        ok = executor()->addDevice(address);
        executor()->prepareAnswer(ok ? "OK\r\n" : QString("ERROR AT ADDING DEVICE %1\r\n").arg(address));
        return true;
    }

    return false;
}
