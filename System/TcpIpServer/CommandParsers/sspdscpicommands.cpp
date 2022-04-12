#include "sspdscpicommands.h"
#include "../ctcpipserver.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

SspdScpiCommands::SspdScpiCommands(QObject *parent)
    : CommonScpiCommands(parent)
{

}

/** формат строки
 * SSPD:DEVice<N>:DATA? - получение всех данных
 * SSPD:DEVice<N>:CURRent? - получение тока
 * SSPD:DEVice<N>:CURRent <Value/Float> - установка тока
 * SSPD:DEVice<N>:CURrentMonitor? - получение тока монитора  - только для SDM1
 * SSPD:DEVice<N>:VOLTage? - получение напряжения
 * SSPD:DEVice<N>:COUNts? - получение числа отсчетов
 * SSPD:DEVice<N>:STATus? - получение статуса устройства
 * SSPD:DEVice<N>:STATus <Value|Byte> - установка статуса устройства
 *
 * Атомарные функции чтения статуса
 * SSPD:DEVice<N>:SHORt? - закоротка
 * SSPD:DEVice<N>:AMPlifierEnable? - включен ли усилитель
 * SSPD:DEVice<N>:RFKeytoComparatorenable? - переключение усилителя на компаратор - только для SDM0
 * SSPD:DEVice<N>:HighFrequencyModeEnable? - режим для высокоскоростного счета - только для SDM1
 * SSPD:DEVice<N>:ComparatorLatchEnable? - включен ли компаратор
 * SSPD:DEVice<N>:COUnterEnable? - включен ли счетчик
 * SSPD:DEVice<N>:AutoRestoreEnable? - включена ли функция автовосстановления тока
 *
 * Атомарные функции записи статуса
 * SSPD:DEVice<N>:SHORt <Value|Boolean> - установка закоротки
 * SSPD:DEVice<N>:AMPlifierEnable <Value|Boolean> - включение усилителя
 * SSPD:DEVice<N>:RFKeytoComparatorenable <Value|Boolean> - переключение выхода усилителя на компаратор - только для SDM0
 * SSPD:DEVice<N>:HighFrequencyModeEnable <Value|Boolean> - режим для высокоскоростного счета - только для SDM1
 * SSPD:DEVice<N>:ComparatorLatchEnable <Value|Boolean> - включение компаратора
 * SSPD:DEVice<N>:COUnterEnable <Value|Boolean> - включение счетчика
 * SSPD:DEVice<N>:AutoRestoreEnable <Value|Boolean> - включение авто восстановления рабочей точки
 *
 * SSPD:DEVice<N>:PARAms? - получение параметров устройства
 * SSPD:DEVice<N>:PARAms <Value|JSON> - установка параметров устройства
 * SSPD:DEVice<N>:CoMParatorReference? - получение уровня компарации устройства
 * SSPD:DEVice<N>:CoMParatorReference <Value|Float> - установка уровня компарации устройства
 * SSPD:DEVice<N>:TIMeConst? - получение постоянной времени счетчика
 * SSPD:DEVice<N>:TIMeConst <Value|Float> - установка постоянной времени счетчика
 * SSPD:DEVice<N>:AutoRestoreTHreshold? - получение напряжения срабатывания системы автовосстановления
 * SSPD:DEVice<N>:AutoRestoreTHreshold <Value|Float> - установка напряжения срабатывания системы автовосстановления
 * SSPD:DEVice<N>:AutoRestoreTimeOut? - получение постоянной времени системы автовосстановления
 * SSPD:DEVice<N>:AutoRestoreTimeOut <Value|Float> - установка постоянной времени системы автовосстановления
 * SSPD:DEVice<N>:AutoRestoreCOunts? - получение числа срабатываний системы автовосстановления
 * SSPD:DEVice<N>:AutoRestoreCOunts <Value|Float> - сброс числа срабатываний системы автовосстановления
 * SSPD:DEVice<N>:EEPRomconst? - получение коэффициентов из постоянной памяти
 * SSPD:DEVice<N>:EEPRomconst <Value|JSON> - установка коэффициентов из постоянной памяти
 * SSPD:DEVice<N>:CurrentADCcoeff? - получение коэффициентов
 * SSPD:DEVice<N>:CurrentADCcoeff <Value|JSON> - установка коэффициентов
 * SSPD:DEVice<N>:CurrentMonitorADCcoeff? - получение коэффициентов - только для SDM1
 * SSPD:DEVice<N>:CurrentMonitorADCcoeff <Value|JSON> - установка коэффициентов - только для SDM1
 * SSPD:DEVice<N>:VoltageADCcoeff? - получение коэффициентов
 * SSPD:DEVice<N>:VoltageADCcoeff <Value|JSON> - установка коэффициентов
 * SSPD:DEVice<N>:CurrentDACcoeff? - получение коэффициентов
 * SSPD:DEVice<N>:CurrentDACcoeff <Value|JSON> - установка коэффициентов
 * SSPD:DEVice<N>:CoMParatorCoeff? - получение коэффициентов
 * SSPD:DEVice<N>:CoMParatorCoeff <Value|JSON> - установка коэффициентов
 *
 * SSPD:DEVice<N>:PIDStatus? - получение статуса работы PID регулятора
 * SSPD:DEVice<N>:PIDStatus <Value|bool> - включение PID регулятора
 * SSPD:DEVice<N>:PWMStatus? - получение статуса работы PWM на закоротке
 * SSPD:DEVice<N>:PWMStatus <Value|bool> - включение PWM на закоротке
 * SSPD:DEVice<N>:PWMFrequency? - получение частоты работы PWM на закоротке
 * SSPD:DEVice<N>:PWMFrequency <Value|float> - учтановка частоты PWM на закоротке
 * SSPD:DEVice<N>:PWMDuty? - получение скважности PWM на закоротке
 * SSPD:DEVice<N>:PWMDuty <Value|float> - установка скважности PWM на закоротке
 */

bool SspdScpiCommands::executeCommand(QString command, QString params)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }

    if (command.indexOf("SSPD:") != 0)
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
    if (!devInfo->type().contains("CU4SD")){
        executor()->prepareAnswer("ERROR: This address not correspond to SspdDriver\r\n");
        return true;
    }

    command = strList[1]; //уточняем команду
    processingAnswer answer = PA_None;

    if (devInfo->type().contains("CU4SDM0")){
        answer = processingCommandSDM0(address, command, params);
    }

    else if (devInfo->type().contains("CU4SDM1")){
        answer = processingCommandSDM1(address, command, params);
    }
    else {
        executor()->prepareAnswer("ERROR: This address correspond to not supported Sspd Driver device type. Please update software\r\n");
        return true;
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

CommonScpiCommands::processingAnswer SspdScpiCommands::processingCommandSDM0(quint8 address, QString command, QString params)
{
    SspdDriverM0 driver;
    bool ok = false;

    cTcpIpServer::consoleWrite("processingCommandSDM0");
    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);

    if (command == "DATA?"){
        auto data = driver.data()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Current"]                  = static_cast<double>(data.Current);
            jsonObj["Voltage"]                  = static_cast<double>(data.Voltage);
            jsonObj["Counts"]                   = static_cast<int>(data.Counts);
            QJsonObject status;
            status["Shorted"]                   = static_cast<uint8_t>(static_cast<bool>(data.Status.stShorted));
            status["AmplifierEnable"]           = static_cast<uint8_t>(static_cast<bool>(data.Status.stAmplifierOn));
            status["RfKeyToComparatorEnable"]   = static_cast<uint8_t>(static_cast<bool>(data.Status.stRfKeyToCmp));
            status["ComparatorEnable"]          = static_cast<uint8_t>(static_cast<bool>(data.Status.stComparatorOn));
            status["CounterEnable"]             = static_cast<uint8_t>(static_cast<bool>(data.Status.stCounterOn));
            status["AutoResetEnable"]           = static_cast<uint8_t>(static_cast<bool>(data.Status.stAutoResetOn));
            jsonObj["Status"]                   = status;
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "CURR?"){
        auto data = driver.current()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<double>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CURR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.current()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "VOLT?"){
        auto data = driver.voltage()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<double>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "COUN?"){
        auto data = driver.counts()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<int>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "STAT?"){
        QJsonObject jsonObj;
        auto status = driver.status()->getValueSync(&ok);
        if (ok){
            jsonObj["Shorted"]                   = static_cast<uint8_t>(static_cast<bool>(status.stShorted));
            jsonObj["AmplifierEnable"]           = static_cast<uint8_t>(static_cast<bool>(status.stAmplifierOn));
            jsonObj["RfKeyToComparatorEnable"]   = static_cast<uint8_t>(static_cast<bool>(status.stRfKeyToCmp));
            jsonObj["ComparatorEnable"]          = static_cast<uint8_t>(static_cast<bool>(status.stComparatorOn));
            jsonObj["CounterEnable"]             = static_cast<uint8_t>(static_cast<bool>(status.stCounterOn));
            jsonObj["AutoResetEnable"]           = static_cast<uint8_t>(static_cast<bool>(status.stAutoResetOn));
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "STAT"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        auto status = driver.status()->currentValue();
        status.stShorted        = (jsonObj["Shorted"].toInt(status.stShorted)                   != 0) ? 1 : 0;
        status.stAmplifierOn    = (jsonObj["AmplifierEnable"].toInt(status.stAmplifierOn)       != 0) ? 1 : 0;
        status.stRfKeyToCmp     = (jsonObj["RfKeyToComparatorEnable"].toInt(status.stRfKeyToCmp)!= 0) ? 1 : 0;
        status.stComparatorOn   = (jsonObj["ComparatorEnable"].toInt(status.stComparatorOn)     != 0) ? 1 : 0;
        status.stCounterOn      = (jsonObj["CounterEnable"].toInt(status.stCounterOn)           != 0) ? 1 : 0;
        status.stAutoResetOn    = (jsonObj["AutoResetEnable"].toInt(status.stAutoResetOn)       != 0) ? 1 : 0;
        driver.status()->setValueSync(status, &ok);
        return ok ? PA_Ok : PA_TimeOut;
    }

    if (command  == "SHOR?"){
        auto data = driver.status()->getValueSync(&ok).stShorted;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<bool>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "SHOR"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.shortEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "AMPE?"){
            auto data = driver.status()->getValueSync(&ok).stAmplifierOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<bool>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "AMPE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.amplifierEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "RFKC?"){
        quint8 data = driver.status()->getValueSync(&ok).stRfKeyToCmp;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "RFKC"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.rfKeyEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CLE?"){
        quint8 data = driver.status()->getValueSync(&ok).stComparatorOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CLE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.cmpLatchEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "COUE?"){
        quint8 data = driver.status()->getValueSync(&ok).stCounterOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "COUE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.counterEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARE?"){
        quint8 data = driver.status()->getValueSync(&ok).stAutoResetOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.autoResetEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "PARA"){
        //For example SSPD:DEV0:PARA\s{"AutoResetCounts":1,"AutoResetThreshold":0.2,"AutoResetTimeOut":0,"Cmp_Ref_Level":2,"Time_Const":0}\r\n
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4SDM0V1_Param_t data;
        data.AutoResetCounts    = static_cast<quint32>(jsonObj["AutoResetCounts"].toInt());
        data.AutoResetThreshold = static_cast<float>(jsonObj["AutoResetThreshold"].toDouble());
        data.AutoResetTimeOut   = static_cast<float>(jsonObj["AutoResetTimeOut"].toDouble());
        data.Cmp_Ref_Level      = static_cast<float>(jsonObj["Cmp_Ref_Level"].toDouble());
        data.Time_Const         = static_cast<float>(jsonObj["Time_Const"].toDouble());
        bool ok = false;
        driver.params()->setValueSync(data, &ok);
        return ok ? PA_Ok : PA_TimeOut;
    }
    if (command == "PARA?"){
        CU4SDM0V1_Param_t data = driver.params()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["AutoResetCounts"]      = static_cast<int>(data.AutoResetCounts);
            jsonObj["AutoResetThreshold"]   = static_cast<double>(data.AutoResetThreshold);
            jsonObj["AutoResetTimeOut"]     = static_cast<double>(data.AutoResetTimeOut);
            jsonObj["Cmp_Ref_Level"]        = static_cast<double>(data.Cmp_Ref_Level);
            jsonObj["Time_Const"]           = static_cast<double>(data.Time_Const);
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "CMPR?"){
        double data = static_cast<double>(driver.cmpReferenceLevel()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CMPR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.cmpReferenceLevel()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "TIMC?"){
        double data = static_cast<double>(driver.timeConst()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "TIMC"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.timeConst()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARTH?"){
        double data = static_cast<double>(driver.autoResetThreshold()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARTH"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetThreshold()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARTO?"){
        double data = static_cast<double>(driver.autoResetTimeOut()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARTO"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetTimeOut()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARCO?"){
        double data = static_cast<double>(driver.autoResetAlarmsCounts()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARCO"){
        unsigned int data = params.toUInt(&ok);
        if (ok) {
            driver.autoResetAlarmsCounts()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "EEPR?"){
        CU4SDM0V1_EEPROM_Const_t data = driver.eepromConst()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            QJsonArray value;
            value.append(static_cast<double>(data.Current_ADC.first));
            value.append(static_cast<double>(data.Current_ADC.second));
            jsonObj["Current_ADC"]      = value;
            value[0] = static_cast<double>(data.Voltage_ADC.first);
            value[1] = static_cast<double>(data.Voltage_ADC.second);
            jsonObj["Voltage_ADC"]      = value;
            value[0] = static_cast<double>(data.Current_DAC.first);
            value[1] = static_cast<double>(data.Current_DAC.second);
            jsonObj["Current_DAC"]      = value;
            value[0] = static_cast<double>(data.Cmp_Ref_DAC.first);
            value[1] = static_cast<double>(data.Cmp_Ref_DAC.second);
            jsonObj["Cmp_Ref_DAC"]      = value;
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4SDM0V1_EEPROM_Const_t data;

        QJsonArray value = jsonObj["Current_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_ADC value");
            return PA_WithoutAnswer;
        }
        data.Current_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Current_ADC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Voltage_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Voltage_ADC value");
            return PA_WithoutAnswer;
        }
        data.Voltage_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Voltage_ADC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Current_DAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_DAC value");
            return PA_WithoutAnswer;
        }
        data.Current_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Current_DAC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Cmp_Ref_DAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Cmp_Ref_DAC value");
            return PA_WithoutAnswer;
        }
        data.Cmp_Ref_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Cmp_Ref_DAC.second = static_cast<float>(value[1].toDouble());

        driver.eepromConst()->setValueSync(data, &ok);
        return ok ? PA_Ok : PA_TimeOut;
    }

    if (command == "CADC?"){
        pair_t<float> data = driver.currentAdcCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentAdcCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "VADC?"){
        pair_t<float> data = driver.voltageAdcCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "VADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.voltageAdcCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CDAC?"){
        pair_t<float> data = driver.currentDacCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CDAC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentDacCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CMPC?"){
        pair_t<float> data = driver.cmpReferenceCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CMPC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.cmpReferenceCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "PIDS?"){
        quint8 data = driver.PIDEnableStatus()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "PIDS"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.PIDEnableStatus()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    return PA_None;
}

CommonScpiCommands::processingAnswer SspdScpiCommands::processingCommandSDM1(quint8 address, QString command, QString params)
{
    SspdDriverM1 driver;
    bool ok = false;
    cTcpIpServer::consoleWrite("processingCommandSDM1");

    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);

    if (command == "DATA?"){
        auto data = driver.data()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Current"]                  = static_cast<double>(data.Current);
            jsonObj["CurrentMonitor"]           = static_cast<double>(data.CurrentMonitor);
            jsonObj["Voltage"]                  = static_cast<double>(data.Voltage);
            jsonObj["Counts"]                   = static_cast<int>(data.Counts);
            QJsonObject status;
            status["Shorted"]                   = static_cast<uint8_t>(static_cast<bool>(data.Status.stShorted));
            status["AmplifierEnable"]           = static_cast<uint8_t>(static_cast<bool>(data.Status.stAmplifierOn));
            status["HFModeEnable"]              = static_cast<uint8_t>(static_cast<bool>(data.Status.stHFModeOn));
            status["ComparatorEnable"]          = static_cast<uint8_t>(static_cast<bool>(data.Status.stComparatorOn));
            status["CounterEnable"]             = static_cast<uint8_t>(static_cast<bool>(data.Status.stCounterOn));
            status["AutoResetEnable"]           = static_cast<uint8_t>(static_cast<bool>(data.Status.stAutoResetOn));
            jsonObj["Status"]                   = status;
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "CURR?"){
        auto data = driver.current()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<double>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CURR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.current()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CURM?"){
        auto data = driver.currentMonitor()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<double>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "VOLT?"){
        auto data = driver.voltage()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<double>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "COUN?"){
        auto data = driver.counts()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<int>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "STAT?"){
        QJsonObject jsonObj;
        auto status = driver.status()->getValueSync(&ok);
        if (ok){
            jsonObj["Shorted"]          = static_cast<uint8_t>(static_cast<bool>(status.stShorted));
            jsonObj["AmplifierEnable"]  = static_cast<uint8_t>(static_cast<bool>(status.stAmplifierOn));
            jsonObj["HFModeEnable"]     = static_cast<uint8_t>(static_cast<bool>(status.stHFModeOn));
            jsonObj["ComparatorEnable"] = static_cast<uint8_t>(static_cast<bool>(status.stComparatorOn));
            jsonObj["CounterEnable"]    = static_cast<uint8_t>(static_cast<bool>(status.stCounterOn));
            jsonObj["AutoResetEnable"]  = static_cast<uint8_t>(static_cast<bool>(status.stAutoResetOn));
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "STAT"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        auto status = driver.status()->currentValue();
        status.stShorted        = (jsonObj["Shorted"].toInt(status.stShorted)               != 0) ? 1 : 0;
        status.stAmplifierOn    = (jsonObj["AmplifierEnable"].toInt(status.stAmplifierOn)   != 0) ? 1 : 0;
        status.stHFModeOn       = (jsonObj["HFModeEnable"].toInt(status.stHFModeOn)         != 0) ? 1 : 0;
        status.stComparatorOn   = (jsonObj["ComparatorEnable"].toInt(status.stComparatorOn) != 0) ? 1 : 0;
        status.stCounterOn      = (jsonObj["CounterEnable"].toInt(status.stCounterOn)       != 0) ? 1 : 0;
        status.stAutoResetOn    = (jsonObj["AutoResetEnable"].toInt(status.stAutoResetOn)   != 0) ? 1 : 0;
        driver.status()->setValueSync(status, &ok);
        return ok ? PA_Ok : PA_TimeOut;
    }

    if (command  == "SHOR?"){
        auto data = driver.status()->getValueSync(&ok).stShorted;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<bool>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "SHOR"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.shortEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "AMPE?"){
            auto data = driver.status()->getValueSync(&ok).stAmplifierOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<bool>(data)));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "AMPE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.amplifierEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "HFME?"){
        quint8 data = driver.status()->getValueSync(&ok).stHFModeOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "HFME"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.highFrequencyModeEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CLE?"){
        quint8 data = driver.status()->getValueSync(&ok).stComparatorOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CLE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.cmpLatchEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "COUE?"){
        quint8 data = driver.status()->getValueSync(&ok).stCounterOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "COUE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.counterEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARE?"){
        quint8 data = driver.status()->getValueSync(&ok).stAutoResetOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARE"){
        auto data = static_cast<bool>(params.toInt(&ok));
        if (ok) {
            driver.autoResetEnable()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "PARA"){
        //For example SSPD:DEV0:PARA\s{"AutoResetCounts":1,"AutoResetThreshold":0.2,"AutoResetTimeOut":0,"Cmp_Ref_Level":2,"Time_Const":0}\r\n
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4SDM0V1_Param_t data;
        data.AutoResetCounts    = static_cast<quint32>(jsonObj["AutoResetCounts"].toInt());
        data.AutoResetThreshold = static_cast<float>(jsonObj["AutoResetThreshold"].toDouble());
        data.AutoResetTimeOut   = static_cast<float>(jsonObj["AutoResetTimeOut"].toDouble());
        data.Cmp_Ref_Level      = static_cast<float>(jsonObj["Cmp_Ref_Level"].toDouble());
        data.Time_Const         = static_cast<float>(jsonObj["Time_Const"].toDouble());
        bool ok = false;
        driver.params()->setValueSync(data, &ok);
        return ok ? PA_Ok : PA_TimeOut;
    }
    if (command == "PARA?"){
        CU4SDM0V1_Param_t data = driver.params()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["AutoResetCounts"]      = static_cast<int>(data.AutoResetCounts);
            jsonObj["AutoResetThreshold"]   = static_cast<double>(data.AutoResetThreshold);
            jsonObj["AutoResetTimeOut"]     = static_cast<double>(data.AutoResetTimeOut);
            jsonObj["Cmp_Ref_Level"]        = static_cast<double>(data.Cmp_Ref_Level);
            jsonObj["Time_Const"]           = static_cast<double>(data.Time_Const);
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }

    if (command == "CMPR?"){
        double data = static_cast<double>(driver.cmpReferenceLevel()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CMPR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.cmpReferenceLevel()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "TIMC?"){
        double data = static_cast<double>(driver.timeConst()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "TIMC"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.timeConst()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARTH?"){
        double data = static_cast<double>(driver.autoResetThreshold()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARTH"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetThreshold()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARTO?"){
        double data = static_cast<double>(driver.autoResetTimeOut()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARTO"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetTimeOut()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "ARCO?"){
        double data = static_cast<double>(driver.autoResetAlarmsCounts()->getValueSync(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "ARCO"){
        unsigned int data = params.toUInt(&ok);
        if (ok) {
            driver.autoResetAlarmsCounts()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "EEPR?"){
        auto data = driver.eepromConst()->getValueSync(&ok);
        if (ok){
            QJsonObject jsonObj;
            QJsonArray value;
            value.append(static_cast<double>(data.Current_ADC.first));
            value.append(static_cast<double>(data.Current_ADC.second));
            jsonObj["Current_ADC"]      = value;
            value[0] = static_cast<double>(data.Voltage_ADC.first);
            value[1] = static_cast<double>(data.Voltage_ADC.second);
            jsonObj["Voltage_ADC"]      = value;
            value[0] = static_cast<double>(data.CurrentMonitor_ADC.first);
            value[1] = static_cast<double>(data.CurrentMonitor_ADC.second);
            jsonObj["CurrentMonitor_ADC"]      = value;
            value[0] = static_cast<double>(data.Current_DAC.first);
            value[1] = static_cast<double>(data.Current_DAC.second);
            jsonObj["Current_DAC"]      = value;
            value[0] = static_cast<double>(data.Cmp_Ref_DAC.first);
            value[1] = static_cast<double>(data.Cmp_Ref_DAC.second);
            jsonObj["Cmp_Ref_DAC"]      = value;
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4SDM1_EEPROM_Const_t data;

        QJsonArray value = jsonObj["Current_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_ADC value");
            return PA_WithoutAnswer;
        }
        data.Current_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Current_ADC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Voltage_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Voltage_ADC value");
            return PA_WithoutAnswer;
        }
        data.Voltage_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Voltage_ADC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["CurrentMonitor_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Voltage_ADC value");
            return PA_WithoutAnswer;
        }
        data.CurrentMonitor_ADC.first  = static_cast<float>(value[0].toDouble());
        data.CurrentMonitor_ADC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Current_DAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_DAC value");
            return PA_WithoutAnswer;
        }
        data.Current_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Current_DAC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Cmp_Ref_DAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Cmp_Ref_DAC value");
            return PA_WithoutAnswer;
        }
        data.Cmp_Ref_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Cmp_Ref_DAC.second = static_cast<float>(value[1].toDouble());

        driver.eepromConst()->setValueSync(data, &ok);
        return ok ? PA_Ok : PA_TimeOut;
    }

    if (command == "CADC?"){
        pair_t<float> data = driver.currentAdcCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentAdcCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CMADC?"){
        pair_t<float> data = driver.currentMonitorAdcCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CMADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentMonitorAdcCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "VADC?"){
        pair_t<float> data = driver.voltageAdcCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "VADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.voltageAdcCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CDAC?"){
        pair_t<float> data = driver.currentDacCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CDAC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentDacCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "CMPC?"){
        pair_t<float> data = driver.cmpReferenceCoeff()->getValueSync(&ok);
        if (ok){
            sendPairFloat(data);
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "CMPC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.cmpReferenceCoeff()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    if (command == "PIDS?"){
        quint8 data = driver.PIDEnableStatus()->getValueSync(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return PA_WithoutAnswer;
        }
        return PA_TimeOut;
    }
    if (command == "PIDS"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.PIDEnableStatus()->setValueSync(data, &ok);
            return ok ? PA_Ok : PA_TimeOut;
        }
        return PA_ErrorData;
    }

    return PA_None;
}
