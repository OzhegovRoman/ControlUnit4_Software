#include "sspdscpicommands.h"
#include "../ctcpipserver.h"
#include "Drivers/ccu4sdm0driver.h"
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
 * SSPD:DEVice<N>:VOLTage? - получение напряжения
 * SSPD:DEVice<N>:COUNts? - получение числа отсчетов
 * SSPD:DEVice<N>:STATus? - получение статуса устройства
 * SSPD:DEVice<N>:STATus <Value|Byte> - установка статуса устройства
 *
 * Атомарные функции чтения статуса
 * SSPD:DEVice<N>:SHORt? - закоротка
 * SSPD:DEVice<N>:AMPlifierEnable? - включен ли усилитель
 * SSPD:DEVice<N>:RFKeytoComparatorenable? - переключение усилителя на компаратор
 * SSPD:DEVice<N>:ComparatorLatchEnable? - включен ли компаратор
 * SSPD:DEVice<N>:COUnterEnable? - включен ли счетчик
 * SSPD:DEVice<N>:AutoRestoreEnable? - включена ли функция автовосстановления тока
 *
 * Атомарные функции записи статуса
 * SSPD:DEVice<N>:SHORt <Value|Boolean> - установка закоротки
 * SSPD:DEVice<N>:AMPlifierEnable <Value|Boolean> - включение усилителя
 * SSPD:DEVice<N>:RFKeytoComparatorenable <Value|Boolean> - переключение выхода усилителя на компаратор
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
 * SSPD:DEVice<N>:VoltageADCcoeff? - получение коэффициентов
 * SSPD:DEVice<N>:VoltageADCcoeff <Value|JSON> - установка коэффициентов
 * SSPD:DEVice<N>:CurrentDACcoeff? - получение коэффициентов
 * SSPD:DEVice<N>:CurrentDACcoeff <Value|JSON> - установка коэффициентов
 * SSPD:DEVice<N>:CoMParatorCoeff? - получение коэффициентов
 * SSPD:DEVice<N>:CoMParatorCoeff <Value|JSON> - установка коэффициентов
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

    command = strList[1]; //уточняем команду

    cCu4SdM0Driver driver;

    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);
    processingAnswer answer = PA_None;

    if (command == "DATA?"){
        CU4SDM0V1_Data_t data = driver.deviceData()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Current"]  = static_cast<double>(data.Current);
            jsonObj["Voltage"]  = static_cast<double>(data.Voltage);
            jsonObj["Counts"]   = static_cast<double>(data.Counts);
            jsonObj["Status"]   = static_cast<double>(data.Status.Data);
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "CURR?"){
        double data = static_cast<double>(driver.current()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CURR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.current()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VOLT?"){
        double data = static_cast<double>(driver.voltage()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "COUN?"){
        double data = static_cast<double>(driver.counts()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "STAT?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok).Data;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "STAT"){
        CU4SDM0_Status_t Status;
        Status.Data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.deviceStatus()->setValue(Status);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command  == "SHOR?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stShorted;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "SHOR"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setShortEnable(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "AMPE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stAmplifierOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "AMPE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setAmpEnable(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "RFKC?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stRfKeyToCmp;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "RFKC"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setRfKeyEnable(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "CLE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stComparatorOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CLE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setCmpLatchEnable(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "COUE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stCounterOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "COUE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setCounterEnable(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "ARE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stAutoResetOn;
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "ARE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setAutoResetEnable(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
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
        driver.deviceParams()->setValue(data);
        if (driver.waitingAnswer()) answer = PA_Ok;
        else answer = PA_TimeOut;
    }
    if (command == "PARA?"){
        CU4SDM0V1_Param_t data = driver.deviceParams()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["AutoResetCounts"]      = static_cast<int>(data.AutoResetCounts);
            jsonObj["AutoResetThreshold"]   = static_cast<double>(data.AutoResetThreshold);
            jsonObj["AutoResetTimeOut"]     = static_cast<double>(data.AutoResetTimeOut);
            jsonObj["Cmp_Ref_Level"]        = static_cast<double>(data.Cmp_Ref_Level);
            jsonObj["Time_Const"]           = static_cast<double>(data.Time_Const);
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "CMPR?"){
        double data = static_cast<double>(driver.cmpReferenceLevel()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CMPR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.cmpReferenceLevel()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "TIMC?"){
        double data = static_cast<double>(driver.timeConst()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "TIMC"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.timeConst()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "ARTH?"){
        double data = static_cast<double>(driver.autoResetThreshold()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "ARTH"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetThreshold()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "ARTO?"){
        double data = static_cast<double>(driver.autoResetTimeOut()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "ARTO"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetTimeOut()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "ARCO?"){
        double data = static_cast<double>(driver.autoResetAlarmsCounts()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "ARCO"){
        unsigned int data = params.toUInt(&ok);
        if (ok) {
            driver.autoResetAlarmsCounts()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "EEPR?"){
        CU4SDM0V1_EEPROM_Const_t data = driver.eepromConst()->getValueSequence(&ok);
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
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4SDM0V1_EEPROM_Const_t data;

        QJsonArray value = jsonObj["Current_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_ADC value");
            return true;
        }
        data.Current_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Current_ADC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Voltage_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Voltage_ADC value");
            return true;
        }
        data.Voltage_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Voltage_ADC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Current_DAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_DAC value");
            return true;
        }
        data.Current_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Current_DAC.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Cmp_Ref_DAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Cmp_Ref_DAC value");
            return true;
        }
        data.Cmp_Ref_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Cmp_Ref_DAC.second = static_cast<float>(value[1].toDouble());

        driver.eepromConst()->setValue(data);
        if (driver.waitingAnswer()) answer = PA_Ok;
        else answer = PA_TimeOut;
    }

    if (command == "CADC?"){
        pair_t<float> data = driver.currentAdcCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentAdcCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VADC?"){
        pair_t<float> data = driver.voltageAdcCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.voltageAdcCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "CDAC?"){
        pair_t<float> data = driver.currentDacCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CDAC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.currentDacCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "CMPC?"){
        pair_t<float> data = driver.cmpReferenceCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CMPC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.cmpReferenceCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
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
    }

    return true;
}
