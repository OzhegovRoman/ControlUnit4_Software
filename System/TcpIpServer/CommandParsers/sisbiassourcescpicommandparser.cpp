#include "sisbiassourcescpicommandparser.h"

#include "../ctcpipserver.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "Drivers/sisbiassourcedriverm0.h"


SisBiasSourceScpiCommandParser::SisBiasSourceScpiCommandParser(QObject *parent)
    : CommonScpiCommands (parent)
{

}

/** формат команд для  BiasSource
 * BIASsource:DEVice<N>:DATA?                           - получение всех данных
 * BIASsource:DEVice<N>:CURRent?                        - получение тока смещения SIS [A]
 * BIASsource:DEVice<N>:CURRent <Value|Float>           - установка тока смещения SIS  [A]
 * BIASsource:DEVice<N>:VOLTage?                        - получение напряжения смещения SIS [V]
 * BIASsource:DEVice<N>:VOLTage <Value|Float>           - установка напряжения смещения SIS [V]
 * BIASsource:DEVice<N>:SHORt?                          - получение статуса закоротки (Shorted = 1, Unshorted = 1)
 * BIASsource:DEVice<N>:SHORt <Value|Bool>              - установка статуса закоротки (Shorted = 1, Unshorted = 1)
 * BIASsource:DEVice<N>:MODE?                           - получение статуса режима смещения (IMode = 'I', Umode = 'U')
 * BIASsource:DEVice<N>:MODE <Value|Char>               - установка статуса режима смещения (IMode = 'I', Umode = 'U')
 * BIASsource:DEVice<N>:EEPRomconst?                    - получение коэффициентов из постоянной памяти
 * BIASsource:DEVice<N>:EEPRomconst <Value|JSON>        - установка коэффициентов из постоянной памяти
 * BIASsource:DEVice<N>:CurrentADCcoeff?                - получение коэффициентов
 * BIASsource:DEVice<N>:CurrentADCcoeff <Value|JSON>    - установка коэффициентов
 * BIASsource:DEVice<N>:CurrentDACcoeff?                - получение коэффициентов
 * BIASsource:DEVice<N>:CurrentDACcoeff <Value|JSON>    - установка коэффициентов
 * BIASsource:DEVice<N>:CurrentLIMits?                  - получение пределов изменения тока
 * BIASsource:DEVice<N>:CurrentLIMits <Value|JSON>      - установка пределов изменения тока
 * BIASsource:DEVice<N>:CurrentSTeP?                    - получение шага перестройки тока
 * BIASsource:DEVice<N>:CurrentSTeP <Value|float>       - установка шага перестройки тока
 * BIASsource:DEVice<N>:VoltageADCcoeff?                - получение коэффициентов
 * BIASsource:DEVice<N>:VoltageADCcoeff <Value|JSON>    - установка коэффициентов
 * BIASsource:DEVice<N>:VoltageDACcoeff?                - получение коэффициентов
 * BIASsource:DEVice<N>:VoltageDACcoeff <Value|JSON>    - установка коэффициентов
 * BIASsource:DEVice<N>:VoltageLIMits?                  - получение пределов изменения напряжения
 * BIASsource:DEVice<N>:VoltageLIMits <Value|JSON>      - установка пределов изменения напряжения
 * BIASsource:DEVice<N>:VoltageSTeP?                    - получение шага перестройки напряжения
 * BIASsource:DEVice<N>:VoltageSTeP <Value|float>       - установка шага перестройки напряжения
 * BIASsource:DEVice<N>:CurrentMonitorREsistance?                   - получение эквивалентного сопротивления [Ohm]
 * BIASsource:DEVice<N>:CurrentMonitorREsistance <Value|float>      - установка эквивалентного сопротивления [Ohm]
 **/


bool SisBiasSourceScpiCommandParser::executeCommand(QString command, QString params)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }

    if (command.indexOf("BIAS:") != 0)
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
    if (!devInfo->type().contains("CU4BS")){
        executor()->prepareAnswer("ERROR: This address not correspond to SIS Bias Source\r\n");
        return true;
    }

    command = strList[1]; //уточняем команду
    processingAnswer answer = PA_None;

    SisBiasSourceDriverM0 driver;
    cTcpIpServer::consoleWrite("processing SIS Bias Source commands");
    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);

    if (command == "DATA?"){
        auto data = driver.data()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Current"]          = static_cast<double>(data.current);
            jsonObj["Voltage"]          = static_cast<double>(data.voltage);
            jsonObj["ShortCircuit"]     = static_cast<char>(data.shortCircuit);
            jsonObj["Mode"]             = (data.mode == CU4BSM0_IMODE) ? "I" : "U";
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

    if (command == "VOLT?"){
        auto data = static_cast<double>(driver.voltage()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VOLT"){
        auto data = params.toFloat(&ok);
        if (ok) {
            driver.voltage()->setValueSync(data, &ok);
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

    if (command == "MODE?"){
        auto data = driver.mode()->getValueSync(&ok);
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg((data== CU4BSM0_IMODE) ? "I" : "U"));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "MODE"){
        if (ok) {
            driver.mode()->setValueSync((params == "I") ? CU4BSM0_IMODE : CU4BSM0_UMODE, &ok);
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
            jsonObj["CurrentStep"]      = static_cast<double>(data.currentStep);
            value[0] = static_cast<double>(data.voltageAdc.first);
            value[1] = static_cast<double>(data.voltageAdc.second);
            jsonObj["VoltageAdc"]       = value;
            value[0] = static_cast<double>(data.voltageDac.first);
            value[1] = static_cast<double>(data.voltageDac.second);
            jsonObj["VoltageDac"]       = value;
            value[0] = static_cast<double>(data.voltageLimits.first);
            value[1] = static_cast<double>(data.voltageLimits.second);
            jsonObj["VoltageLimits"]    = value;
            jsonObj["VoltageStep"]      = static_cast<double>(data.voltageStep);
            jsonObj["CurrentMonitorResistance"]      = static_cast<double>(data.currentMonitorResistance);

            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4BSM0V0_EEPROM_Const_t data;

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

        value = jsonObj["VoltageAdc"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at VoltageAdc value");
            return true;
        }
        data.voltageAdc.first  = static_cast<float>(value[0].toDouble());
        data.voltageAdc.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["VoltageDac"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at VoltageDac value");
            return true;
        }
        data.voltageDac.first  = static_cast<float>(value[0].toDouble());
        data.voltageDac.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["VoltageLimits"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at VoltageLimits value");
            return true;
        }
        data.voltageLimits.first  = static_cast<float>(value[0].toDouble());
        data.voltageLimits.second = static_cast<float>(value[1].toDouble());

        data.voltageStep                = static_cast<float>(jsonObj["VoltageStep"].toDouble(0.0));
        data.currentMonitorResistance   = static_cast<float>(jsonObj["CurrentMonitorResistance"].toDouble(0.0));

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

    if (command == "VADC?"){
        pair_t<float> data = driver.voltageAdcCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.voltageAdcCoeff()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VDAC?"){
        pair_t<float> data = driver.voltageDacCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VDAC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.voltageDacCoeff()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VLIM?"){
        pair_t<float> data = driver.voltageLimits()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VLIM"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.voltageLimits()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VSTP?"){
        auto data = static_cast<double>(driver.voltageStep()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VSTP"){
        auto data = params.toFloat(&ok);
        if (ok){
            driver.voltageStep()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "CMRE?"){
        auto data = static_cast<double>(driver.currentMonitorResistance()->getValueSync(&ok));
        if (ok){
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CMRE"){
        auto data = params.toFloat(&ok);
        if (ok){
            driver.currentMonitorResistance()->setValueSync(data, &ok);
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
