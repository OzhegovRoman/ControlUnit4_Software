#include "siscontrollinescpicommands.h"
#include "../ctcpipserver.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "Drivers/siscontrollinedriverm0.h"


SisControlLineScpiCommands::SisControlLineScpiCommands(QObject *parent)
    : CommonScpiCommands (parent)
{

}

/** формат команд для  Control Line
 * ConTRolLine:DEVice<N>:DATA?                           - получение всех данных
 * ConTRolLine:DEVice<N>:CURRent?                        - получение тока контрольной линии [A]
 * ConTRolLine:DEVice<N>:CURRent <Value|Float>           - установка тока контрольной линии [A]
 * ConTRolLine:DEVice<N>:SHORt?                          - статус закоротки (Shorted = 1, Unshorted = 1)
 * ConTRolLine:DEVice<N>:SHORt <Value|Bool>              - статус закоротки
 * ConTRolLine:DEVice<N>:EEPRomconst?                    - получение коэффициентов из постоянной памяти
 * ConTRolLine:DEVice<N>:EEPRomconst <Value|JSON>        - установка коэффициентов из постоянной памяти
 * ConTRolLine:DEVice<N>:CurrentADCcoeff?                - получение коэффициентов
 * ConTRolLine:DEVice<N>:CurrentADCcoeff <Value|JSON>    - установка коэффициентов
 * ConTRolLine:DEVice<N>:CurrentDACcoeff?                - получение коэффициентов
 * ConTRolLine:DEVice<N>:CurrentDACcoeff <Value|JSON>    - установка коэффициентов
 * ConTRolLine:DEVice<N>:CurrentLIMits?                  - получение пределов изменения тока
 * ConTRolLine:DEVice<N>:CurrentLIMits <Value|JSON>      - установка пределов изменения тока
 * ConTRolLine:DEVice<N>:CurrentSTeP?                    - получение шага перестройки тока
 * ConTRolLine:DEVice<N>:CurrentSTeP <Value|float>       - установка шага перестройки тока
 **/
bool SisControlLineScpiCommands::executeCommand(QString command, QString params)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }

    if (command.indexOf("CTRL:") != 0)
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

    // должны проверить тип устройства и выбрать правильный драйвер
    if (!devInfo->type().contains("CU4CL")){
        executor()->prepareAnswer("ERROR: This address not correspond to SIS ControlLine\r\n");
        return true;
    }

    command = strList[1]; //уточняем команду
    processingAnswer answer = PA_None;

    SisControlLineDriverM0 driver;
    cTcpIpServer::consoleWrite("processing SIS ControlLine commands");
    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);

    if (command == "DATA?"){
        auto data = driver.data()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Current"]          = static_cast<double>(data.current);
            jsonObj["ShortCircuit"]     = static_cast<double>(data.shortCircuit);
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "CURR?"){
        auto data = static_cast<double>(driver.current()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CURR"){
        auto data = params.toFloat(&ok);
        if (ok) {
            driver.current()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "SHOR?"){
        auto data = driver.shortEnable()->getValueSync(&ok);
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data ? "1" : "0"));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "SHOR"){
        auto data = params.toInt(&ok);
        if (ok) {
            driver.shortEnable()->setValueSync(data & 0xFF, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "EEPR?"){
        auto data = driver.eepromConst()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            QJsonArray value;
            value.append(static_cast<double>(data.currentAdc.first));
            value.append(static_cast<double>(data.currentAdc.second));
            jsonObj["CurrentADC"]       = value;
            value[0] = static_cast<double>(data.currentDac.first);
            value[1] = static_cast<double>(data.currentDac.second);
            jsonObj["CurrentDAC"]       = value;
            value[0] = static_cast<double>(data.currentLimits.first);
            value[1] = static_cast<double>(data.currentLimits.second);
            jsonObj["CurrentLimits"]    = value;
            jsonObj["CurrentStep"]   = static_cast<double>(data.currentStep);

            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4CLM0V0_EEPROM_Const_t data;

        QJsonArray value = jsonObj["CurrentADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at CurrentADC value");
            return true;
        }
        data.currentAdc.first  = static_cast<float>(value[0].toDouble());
        data.currentAdc.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["CurrentDAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at CurrentDAC value");
            return true;
        }
        data.currentDac.first  = static_cast<float>(value[0].toDouble());
        data.currentDac.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["CurrentLimits"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at CurrentLimits value");
            return true;
        }
        data.currentLimits.first  = static_cast<float>(value[0].toDouble());
        data.currentLimits.second = static_cast<float>(value[1].toDouble());

        data.currentStep    = static_cast<float>(jsonObj["CurrentStep"].toDouble(0.0));

        bool ok = false;
        driver.eepromConst()->setValueSync(data, &ok);
        answer = ok ? PA_Ok : PA_TimeOut;
    }

    if (command == "CADC?"){
        pair_t<float> data = driver.currentAdcCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentAdcCoeff()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "CDAC?"){
        pair_t<float> data = driver.currentDacCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CDAC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentDacCoeff()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "CLIM?"){
        pair_t<float> data = driver.currentLimits()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CLIM"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentLimits()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "CSTP?"){
        auto data = static_cast<double>(driver.currentStep()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CSTP"){
        auto data = params.toFloat(&ok);
        if (ok){
            driver.currentStep()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    switch (answer) {
    case CommonScpiCommands::PA_Ok:
        executor()->prepareAnswer("OK\r\n");
        break;
    case CommonScpiCommands::PA_TimeOut:
        executor()->prepareAnswer("ERROR: Timeout\r\n");
        break;
    case CommonScpiCommands::PA_ErrorData:
        executor()->prepareAnswer("ERROR: Wrong parameters\r\n");
        break;
    case CommonScpiCommands::PA_None:
        return false;
    case CommonScpiCommands::PA_WithoutAnswer:
        return true;
    }
    return true;
}
