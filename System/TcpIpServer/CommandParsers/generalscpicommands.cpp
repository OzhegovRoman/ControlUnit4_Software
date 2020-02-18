#include "generalscpicommands.h"
#include "../ctcpipserver.h"
#include "Drivers/commondriver.h"

GeneralScpiCommands::GeneralScpiCommands(QObject * parent)
    : CommonScpiCommands(parent)
{

}

/** формат строки
 * GENeral:DEVice<N>:INIT - инициализация устройства
 * GENeral:DEVice<N>:PARKing - парковка устройства перед его отключением
 * GENeral:DEVice<N>:ListeningOFF - устройство перестает откликаться на команды, далее работает после перезагрузки
 * GENeral:DEVice<N>:STATus? - получение статуса устройства
 * GENeral:DEVice<N>:ERRor? - получение последней ошибки
 * GENeral:DEVice<N>:DeviceID? - получение уникального идентификатора устройства
 * GENeral:DEVice<N>:DeviceTYPe? - получение типа устройства
 * GENeral:DEVice<N>:DeviceDEScription? - получение полного описания устройства
 * GENeral:DEVice<N>:MODificationVersion? - получение номера модификации устройства
 * GENeral:DEVice<N>:HardWareVersion? - получение версии железа
 * GENeral:DEVice<N>:FirmWareVersion? - получение версии программного обеспечения
 * GENeral:DEVice<N>:reBOOT - перезагрузка устройства
 * GENeral:DEVice<N>:WriteEEProm - запись основных констант в память
 */

bool GeneralScpiCommands::executeCommand(QString command, QString params)
{
    Q_UNUSED(params)

    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }

    if (command.indexOf("GEN:") != 0)
        return false;

    QStringList strList = command.split(":");
    if (strList.count() != 3) // все команды имеют 3 оставляющие
        return false;

    strList.removeFirst();

    if (strList[0].indexOf("DEV")!=0)
        return false;

    bool ok;
    quint8 address = static_cast<quint8>(strList[0].remove(0,3).toInt(&ok));
    if (!ok){
        executor()->prepareAnswer("ERROR: Wrong device address\r\n");
        return true;
    }
    // проверяем полученный адрес на предмет того, что он зарегистрирован в системе

    cDeviceInfo* devInfo = executor()->settings()->deviceByAddress(address);
    if (!devInfo){
        executor()->prepareAnswer("ERROR: Wrong device address\r\n");
        return true;
    }

    command = strList[1]; //уточняем команду

    CommonDriver driver;

    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);


    if (command == "INIT"){
        bool ok = false;
        driver.init()->executeSync(&ok);
        executor()->prepareAnswer(ok ? "OK\r\n"
                                     : "ERROR: Timeout\r\n");
        return true;
    }
    if (command == "PARK"){
        bool ok;
        driver.shutDown()->executeSync(&ok);
        executor()->prepareAnswer(ok ? "OK\r\n"
                                     : "ERROR: Timeout\r\n");
        return true;
    }
    if (command == "LOFF"){
        driver.silence()->execute();
        executor()->prepareAnswer("OK\r\n");
        return true;
    }
    if (command == "STAT?"){ //пока пустышка
        executor()->prepareAnswer("OK\r\n");
        return true;
    }
    if (command == "ERR?"){ //пока пустышка
        executor()->prepareAnswer("OK\r\n");
        return true;
    }
    if (command == "DID?"){
        executor()->prepareAnswer(QString("%1\r\n").arg(devInfo->UDID().toString()));
        return true;
    }
    if (command == "DTYP?"){
        executor()->prepareAnswer(QString("%1\r\n").arg(devInfo->type()));
        return true;
    }
    if (command == "DDES?"){
        executor()->prepareAnswer(QString("%1\r\n").arg(devInfo->description()));
        return true;
    }
    if (command == "MODV?"){
        executor()->prepareAnswer(QString("%1\r\n").arg(devInfo->modificationVersion()));
        return true;
    }
    if (command == "HWV?"){
        executor()->prepareAnswer(QString("%1\r\n").arg(devInfo->hardwareVersion()));
        return true;
    }
    if (command == "FWV?"){
        executor()->prepareAnswer(QString("%1\r\n").arg(devInfo->firmwareVersion()));
        return true;
    }
    if (command == "BOOT"){
        driver.reboot()->execute();
        executor()->prepareAnswer("OK\r\n");
        return true;
    }
    if (command == "WEEP"){
        bool ok;
        driver.writeEeprom()->executeSync(&ok);
        executor()->prepareAnswer(ok ? "OK\r\n"
                                     : "ERROR: Timeout\r\n");
        return true;
    }
    return false;
}
