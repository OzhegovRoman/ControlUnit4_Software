#include "temperaturescpicommands.h"
#include "../ctcpipserver.h"
#include "Drivers/ccu4tdm0driver.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

TemperatureScpiCommands::TemperatureScpiCommands(QObject *parent)
    : CommonScpiCommands(parent)
{

}

/** формат команды
 * TEMperatureDriver:DEVice<N>:DATA? - получение всех данных
 * TEMperatureDriver:DEVice<N>:TEMPerature? - получение температуры
 * TEMperatureDriver:DEVice<N>:PRESsure? - получение давления
 * TEMperatureDriver:DEVice<N>:CURRent? - получение тока смещения температурного сенсора
 * TEMperatureDriver:DEVice<N>:CURRent <Value/Float> - установка тока смещения температурного сенсора
 * TEMperatureDriver:DEVice<N>:VOLTage? - получение напряжения смещения температурного сенсора
 * TEMperatureDriver:DEVice<N>:PRessVoltagePositive? - получение напряжения с положительного контакта датчика давления
 * TEMperatureDriver:DEVice<N>:PRessVoltageNegative? - получение напряжения с негативного контакта датчика давления
 * TEMperatureDriver:DEVice<N>:THermometerON? - получение статуса термометра: включен/выключен
 * TEMperatureDriver:DEVice<N>:THermometerON <Value|Bool> - включение/выключение термометра
 * TEMperatureDriver:DEVice<N>:EEPRomconst? - получение констант
 * TEMperatureDriver:DEVice<N>:EEPRomconst <Value|JSON> - установка констант
 * TEMperatureDriver:DEVice<N>:CurrentADCcoeff? - получение констант для измерения величины тока смещения термометра
 * TEMperatureDriver:DEVice<N>:CurrentADCcoeff <Value|JSON> - установка констант для измерения величины тока смещения термометра
 * TEMperatureDriver:DEVice<N>:CurrentDACcoeff? - получение констант для установки величины тока смещения термометра
 * TEMperatureDriver:DEVice<N>:CurrentDACcoeff <Value|JSON> - установка констант для установки величины тока смещения термометра
 * TEMperatureDriver:DEVice<N>:VoltageADCcoeff? - получение констант для измерения напряжения смещения термометра
 * TEMperatureDriver:DEVice<N>:VoltageADCcoeff <Value|JSON> - установка констант для измерения напряжения смещения термометра
 * TEMperatureDriver:DEVice<N>:VoltagePressPositiveCoeff? - получение констант для измерения напряжения позитивного контакта датчика давления
 * TEMperatureDriver:DEVice<N>:VoltagePressPositiveCoeff <Value|JSON> - уствновка констант для измерения напряжения позитивного контакта датчика давления
 * TEMperatureDriver:DEVice<N>:VoltagePressNegativeCoeff? - получение констант для измерения напряжения негативного контакта датчика давления
 * TEMperatureDriver:DEVice<N>:VoltagePressNegativeCoeff <Value|JSON> - уствновка констант для измерения напряжения негативного контакта датчика давления
 * TEMperatureDriver:DEVice<N>:PRessSensorCoeff? - получение констант для измерения давления
 * TEMperatureDriver:DEVice<N>:PRessSensorCoeff <Value|JSON> - установка констант для измерения давления
 * TEMperatureDriver:DEVice<N>:TEMperatureTable? - получение температурной таблицы
 * TEMperatureDriver:DEVice<N>:TEMperatureTable <Value|JSON> - установка температурной таблицы
 */
bool TemperatureScpiCommands::executeCommand(QString command, QString params)
{
    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }

    if (command.indexOf("TEMD:") != 0)
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

    cCu4TdM0Driver driver;

    driver.setIOInterface(executor()->interface());
    driver.setDevAddress(address);

    processingAnswer answer = PA_None;

    if (command == "DATA?"){
        CU4TDM0V1_Data_t data = driver.deviceData()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Temperature"]          = static_cast<double>(data.Temperature);
            jsonObj["Pressure"]             = static_cast<double>(data.Pressure);
            jsonObj["TempSensorCurrent"]    = static_cast<double>(data.TempSensorCurrent);
            jsonObj["TempSensorVoltage"]    = static_cast<double>(data.TempSensorVoltage);
            jsonObj["PressSensorVoltageP"]  = static_cast<double>(data.PressSensorVoltageP);
            jsonObj["PressSensorVoltageN"]  = static_cast<double>(data.PressSensorVoltageN);
            jsonObj["CommutatorOn"]         = static_cast<quint8>(data.CommutatorOn);
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "TEMP?"){
        double data = static_cast<double>(driver.temperature()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "PRES?"){
        double data = static_cast<double>(driver.pressure()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "CURR?"){
        double data = static_cast<double>(driver.tempSensorCurrent()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CURR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.tempSensorCurrent()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VOLT?"){
        double data = static_cast<double>(driver.tempSensorVoltage()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "PRVP?"){
        double data = static_cast<double>(driver.pressSensorVoltageP()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "PRVN?"){
        double data = static_cast<double>(driver.pressSensorVoltageN()->getValueSequence(&ok));
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }

    if (command == "THON?"){
        bool data = driver.commutatorOn()->getValueSequence(&ok);
        if (ok) {
            executor()->prepareAnswer(QString("%1\r\n").arg(data));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "THON"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.commutatorOn()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "EEPR?"){
        CU4TDM0V1_EEPROM_Const_t data = driver.eepromConst()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            QJsonArray value;
            value.append(static_cast<double>(data.pressSensorCoeffs.first));
            value.append(static_cast<double>(data.pressSensorCoeffs.second));
            jsonObj["Pressure"] = value;
            value[0] = static_cast<double>(data.tempSensorCurrentAdc.first);
            value[1] = static_cast<double>(data.tempSensorCurrentAdc.second);
            jsonObj["Current_ADC"] = value;
            value[0] = static_cast<double>(static_cast<double>(data.tempSensorCurrentDac.first));
            value[1] = static_cast<double>(static_cast<double>(data.tempSensorCurrentDac.second));
            jsonObj["Current_DAC"] = value;
            value[0] = static_cast<double>(data.tempSensorVoltage.first);
            value[1] = static_cast<double>(data.tempSensorVoltage.second);
            jsonObj["Voltage_ADC"] = value;
            value[0] = static_cast<double>(data.pressSensorVoltageP.first);
            value[1] = static_cast<double>(data.pressSensorVoltageP.second);
            jsonObj["Press_VoltageP_ADC"] = value;
            value[0] = static_cast<double>(data.pressSensorVoltageN.first);
            value[1] = static_cast<double>(data.pressSensorVoltageN.second);
            jsonObj["Press_VoltageN_ADC"] = value;
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4TDM0V1_EEPROM_Const_t data;

        QJsonArray value = jsonObj["Pressure"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Pressure value");
            return true;
        }
        data.pressSensorCoeffs.first  = static_cast<float>(value[0].toDouble());
        data.pressSensorCoeffs.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Current_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_ADC value");
            return true;
        }
        data.tempSensorCurrentAdc.first  = static_cast<float>(value[0].toDouble());
        data.tempSensorCurrentAdc.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Current_DAC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Current_DAC value");
            return true;
        }
        data.tempSensorCurrentDac.first  = static_cast<float>(value[0].toDouble());
        data.tempSensorCurrentDac.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Voltage_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Voltage_ADC value");
            return true;
        }
        data.tempSensorVoltage.first    = static_cast<float>(value[0].toDouble());
        data.tempSensorVoltage.second   = static_cast<float>(value[1].toDouble());

        value = jsonObj["Press_VoltageP_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Press_VoltageP_ADC value");
            return true;
        }
        data.pressSensorVoltageP.first  = static_cast<float>(value[0].toDouble());
        data.pressSensorVoltageP.second = static_cast<float>(value[1].toDouble());

        value = jsonObj["Press_VoltageN_ADC"].toArray();
        if (value.size()!=2) {
            executor()->prepareAnswer("Error at Press_VoltageN_ADC value");
            return true;
        }
        data.pressSensorVoltageN.first  = static_cast<float>(value[0].toDouble());
        data.pressSensorVoltageN.second = static_cast<float>(value[1].toDouble());

        driver.eepromConst()->setValue(data);
        if (driver.waitingAnswer()) answer = PA_Ok;
        else answer = PA_TimeOut;
    }

    if (command == "CADC?"){
        pair_t<float> data = driver.tempSensorCurrentAdcCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.tempSensorCurrentAdcCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command=="CDAC?"){
        pair_t<float> data = driver.tempSensorCurrentDacCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "CDAC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.tempSensorCurrentDacCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VADC?"){
        pair_t<float> data = driver.tempSensorVoltageCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VADC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.tempSensorVoltageCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VPPC?"){
        pair_t<float> data = driver.pressSensorVoltagePCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VPPC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.pressSensorVoltagePCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "VPNC?"){
        pair_t<float> data = driver.pressSensorVoltageNCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "VPNC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.pressSensorVoltageNCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "PRSC?"){
        pair_t<float> data = driver.pressSensorCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "PRSC"){
        pair_t<float> data = pairFromJsonString(params, &ok);
        if (ok){
            driver.pressSensorCoeff()->setValue(data);
            if (driver.waitingAnswer()) answer = PA_Ok;
            else answer = PA_TimeOut;
        }
        else answer = PA_ErrorData;
    }

    if (command == "TEMT?"){
        ok = driver.receiveTempTable();
        if (ok){
            CU4TDM0V1_Temp_Table_Item_t* tempTable = driver.tempTable();
            QJsonObject jsonObj;
            QJsonArray table;
            for (int i = 0; i<TEMP_TABLE_SIZE; ++i){
                QJsonArray value;
                value.append(static_cast<double>(tempTable[i].Temperature));
                value.append(static_cast<double>(tempTable[i].Voltage));
                table.append(value);
            }
            jsonObj["TempTable"] = table;
            QJsonDocument jsonDoc(jsonObj);
            executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
            return true;
        }
        answer = PA_TimeOut;
    }
    if (command == "TEMT"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        QJsonArray table = jsonObj["TempTable"].toArray();
        CU4TDM0V1_Temp_Table_Item_t* tempTable = driver.tempTable();
        for (int i = 0; i<TEMP_TABLE_SIZE; ++i){
            QJsonArray value = table[i].toArray();
            tempTable[i].Temperature    = static_cast<float>(value[0].toDouble());
            tempTable[i].Voltage        = static_cast<float>(value[1].toDouble());
        }
        if (driver.waitingAnswer()) answer = PA_Ok;
        else answer = PA_TimeOut;
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
