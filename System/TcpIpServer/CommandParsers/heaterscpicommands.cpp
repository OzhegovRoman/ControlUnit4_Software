#include "heaterscpicommands.h"

#include "../ctcpipserver.h"
#include "Drivers/heaterdriverm0.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


HeaterScpiCommands::HeaterScpiCommands(QObject * parent)
    : CommonScpiCommands(parent)
{

}

//HEAT:DEV1:
/** формат команд для  Heater
 * HEATer:DEVice<N>:HEAT                            - включение нагрева
 * HEATer:DEVice<N>:STOP                            - остановка
 * HEATer:DEVice<N>:DATA?                           - получение всех данных
 * HEATer:DEVice<N>:CURRent?                        - получение тока печки [A]
 * HEATer:DEVice<N>:CURRent <Value|Float>           - установка тока печки [A]
 * HEATer:DEVice<N>:RELayStatus?                    - получение статуса реле  (RELAY_OPEN=0, RELAY_CLOSE=1)
 * HEATer:DEVice<N>:RELayStatus <Value|Byte>        - установка статуса реле
 * HEATer:DEVice<N>:MODE?                           - получение режима работы (MANUAL=0, AUTO=1)
 * HEATer:DEVice<N>:MODE <Value|Byte>               - установка режима работы
 * HEATer:DEVice<N>:EEPRomconst?                    - получение коэффициентов из постоянной памяти
 * HEATer:DEVice<N>:EEPRomconst <Value|JSON>        - установка коэффициентов из постоянной памяти
 * HEATer:DEVice<N>:CurrentADCcoeff?                - получение коэффициентов
 * HEATer:DEVice<N>:CurrentADCcoeff <Value|JSON>    - установка коэффициентов
 * HEATer:DEVice<N>:CurrentDACcoeff?                - получение коэффициентов
 * HEATer:DEVice<N>:CurrentDACcoeff <Value|JSON>    - установка коэффициентов
 * HEATer:DEVice<N>:MAXimumCurrent?                 - получение максимального тока печки
 * HEATer:DEVice<N>:MAXimumCurrent <Value|float>    - установка максимального тока печки
 * HEATer:DEVice<N>:FrontedgeTIMe?                  - получение времени нарастания тока печки
 * HEATer:DEVice<N>:FrontedgeTIMe <Value|float>     - установка времени нарастания тока печки
 * HEATer:DEVice<N>:HoldTIMe?                       - получение времени удержания тока печки
 * HEATer:DEVice<N>:HoldTIMe <Value|float>          - установка времени удержания тока печки
 * HEATer:DEVice<N>:RearedgeTIMe?                   - получение времени спада тока печки
 * HEATer:DEVice<N>:RearedgeTIMe <Value|float>      - установка времени спада тока печки
 **/

bool HeaterScpiCommands::executeCommand(QString command, QString params)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }

    if (command.indexOf("HEAT:") != 0)
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
    if (!devInfo->type().contains("CU4HT")){
        executor()->prepareAnswer("ERROR: This address not correspond to SspdDriver\r\n");
        return true;
    }

    command = strList[1]; //уточняем команду
    processingAnswer answer = PA_None;

    HeaterDriverM0 driver;
    cTcpIpServer::consoleWrite("processing Heater commands");
    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);

    if (command == "HEAT"){
        driver.startHeating()->executeSync(&ok);
        answer = ok ? PA_Ok : PA_TimeOut;
    }
    if (command == "STOP"){
        driver.emergencyStop()->executeSync(&ok);
        answer = ok ? PA_Ok : PA_TimeOut;
    }

    if (command == "DATA?"){
        auto data = driver.data()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Current"]          = static_cast<double>(data.current);
            jsonObj["RelayStatus"]             = static_cast<double>(data.relayStatus);
            jsonObj["Mode"]    = static_cast<double>(data.mode);
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
    if (command == "RELS?"){
        auto data = driver.relayStatus()->getValueSync(&ok);
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data ? "1" : "0"));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "RELS"){
        auto data = params.toInt(&ok);
        if (ok) {
            driver.relayStatus()->setValueSync(data & 0xFF, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }
    if (command == "MODE?"){
        auto data = driver.mode()->getValueSync(&ok);
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data ? "1" : "0"));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "MODE"){
        auto data = params.toInt(&ok);
        if (ok) {
            driver.mode()->setValueSync(data & 0xFF, &ok);
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
            jsonObj["CurrentADC"] = value;
            value[0] = static_cast<double>(data.currentDac.first);
            value[1] = static_cast<double>(data.currentDac.second);
            jsonObj["CurrentDAC"]       = value;
            jsonObj["MaximumCurrent"]   = static_cast<double>(data.maximumCurrent);
            jsonObj["FrontEdge"]        = static_cast<double>(data.frontEdgeTime);
            jsonObj["HoldTime"]         = static_cast<double>(data.holdTime);
            jsonObj["RearEdge"]         = static_cast<double>(data.rearEdgeTime);

            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4HT0V0_EEPROM_Const_t data;

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

        data.maximumCurrent = static_cast<float>(jsonObj["MaximumCurrent"].toDouble(0.0));
        data.frontEdgeTime  = static_cast<float>(jsonObj["FrontEdge"].toDouble(0.0));
        data.holdTime       = static_cast<float>(jsonObj["HoldTime"].toDouble(0.0));
        data.rearEdgeTime   = static_cast<float>(jsonObj["RearEdge"].toDouble(0.0));

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

    if (command == "MAXC?"){
        auto data = static_cast<double>(driver.maximumCurrent()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "MAXC"){
        auto data = params.toFloat(&ok);
        if (ok){
            driver.maximumCurrent()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "FTIM?"){
        auto data = static_cast<double>(driver.frontEdgeTime()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "FTIM"){
        auto data = params.toFloat(&ok);
        if (ok){
            driver.frontEdgeTime()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "HTIM?"){
        auto data = static_cast<double>(driver.holdTime()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "HTIM"){
        auto data = params.toFloat(&ok);
        if (ok){
            driver.holdTime()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "RTIM?"){
        auto data = static_cast<double>(driver.rearEdgeTime()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "RTIM"){
        auto data = params.toFloat(&ok);
        if (ok){
            driver.rearEdgeTime()->setValueSync(data, &ok);
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

