#include "temperaturescpicommands.h"
#include "../ctcpipserver.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

TemperatureScpiCommands::TemperatureScpiCommands(QObject *parent)
    : CommonScpiCommands(parent)
{

}

/** формат команды для М0
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

/** формат команды для М1
 * TEMperatureDriver:DEVice<N>:TEMPerature<K>? - получение температуры
 * TEMperatureDriver:DEVice<N>:TEMPerature? - получение температуры всех каналов? ответ в JSON
 * TEMperatureDriver:DEVice<N>:CURRent<K> <Value/Float> - установка тока смещения температурного сенсора
 * TEMperatureDriver:DEVice<N>:CURRent <Value/Float> - установка токов смещения всех каналов температурного сенсора
 * TEMperatureDriver:DEVice<N>:VOLTage<K>? - получение напряжения смещения температурного сенсора
 * TEMperatureDriver:DEVice<N>:VOLTage? - получение напряжения смещения всех температурных сенсоров
 * TEMperatureDriver:DEVice<N>:SwithMODe? - получение режима работы ключа? Ответ строка <AC, Connect, Disconnect>
 * TEMperatureDriver:DEVice<N>:SwithMODe <Value|<Ac, Connect, Disconnect>> - получение режима работы ключа? Ответ строка <Ac, Connect, Disconnect>
 * TEMperatureDriver:DEVice<N>:RELayStatus? - получение состояния релюх
 * TEMperatureDriver:DEVice<N>:RELayStatus <Value|JSON> - установка состояний релюх

 * TEMperatureDriver:DEVice<N>:EEPRomconst? - получение констант
 * TEMperatureDriver:DEVice<N>:EEPRomconst <Value|JSON> - установка констант
 * TEMperatureDriver:DEVice<N>:DEFaultParameters? - получение параметров по умолчанию
 * TEMperatureDriver:DEVice<N>:DEFaultParameters <Value|JSON> - установка параметров по умолчанию

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
    processingAnswer answer = PA_None;

    if (devInfo->type().contains("CU4TDM0")){
        TempDriverM0 driver;

        driver.setIOInterface(executor()->interface());
        driver.setDevAddress(address);

        if (command == "DATA?"){
            CU4TDM0V1_Data_t data = driver.data()->getValueSync(&ok);
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
            auto data = static_cast<double>(driver.temperature()->getValueSync(&ok));
            if (ok) {
                executor()->prepareAnswer(QString("%1\r\n").arg(data));
                return true;
            }
            answer = PA_TimeOut;
        }

        if (command == "PRES?"){
            auto data = static_cast<double>(driver.pressure()->getValueSync(&ok));
            if (ok) {
                executor()->prepareAnswer(QString("%1\r\n").arg(data));
                return true;
            }
            answer = PA_TimeOut;
        }

        if (command == "CURR?"){
            auto data = static_cast<double>(driver.tempSensorCurrent()->getValueSync(&ok));
            if (ok) {
                executor()->prepareAnswer(QString("%1\r\n").arg(data));
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "CURR"){
            float data = params.toFloat(&ok);
            if (ok) {
                driver.tempSensorCurrent()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "VOLT?"){
            double data = static_cast<double>(driver.tempSensorVoltage()->getValueSync(&ok));
            if (ok) {
                executor()->prepareAnswer(QString("%1\r\n").arg(data));
                return true;
            }
            answer = PA_TimeOut;
        }

        if (command == "PRVP?"){
            double data = static_cast<double>(driver.pressSensorVoltageP()->getValueSync(&ok));
            if (ok) {
                executor()->prepareAnswer(QString("%1\r\n").arg(data));
                return true;
            }
            answer = PA_TimeOut;
        }

        if (command == "PRVN?"){
            double data = static_cast<double>(driver.pressSensorVoltageN()->getValueSync(&ok));
            if (ok) {
                executor()->prepareAnswer(QString("%1\r\n").arg(data));
                return true;
            }
            answer = PA_TimeOut;
        }

        if (command == "THON?"){
            bool data = driver.commutator()->getValueSync(&ok);
            if (ok) {
                executor()->prepareAnswer(QString("%1\r\n").arg(data));
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "THON"){
            quint8 data = static_cast<quint8>(params.toInt(&ok));
            if (ok) {
                driver.commutator()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "EEPR?"){
            CU4TDM0V1_EEPROM_Const_t data = driver.eepromConst()->getValueSync(&ok);
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

            bool ok = false;
            driver.eepromConst()->setValueSync(data, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }

        if (command == "CADC?"){
            pair_t<float> data = driver.tempSensorCurrentAdcCoeff()->getValueSync(&ok);
            if (ok){
                sendPairFloat(data);
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "CADC"){
            pair_t<float> data = pairFromJsonString(params, &ok);
            if (ok){
                driver.tempSensorCurrentAdcCoeff()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command=="CDAC?"){
            pair_t<float> data = driver.tempSensorCurrentDacCoeff()->getValueSync(&ok);
            if (ok){
                sendPairFloat(data);
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "CDAC"){
            pair_t<float> data = pairFromJsonString(params, &ok);
            if (ok){
                driver.tempSensorCurrentDacCoeff()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "VADC?"){
            pair_t<float> data = driver.tempSensorVoltageCoeff()->getValueSync(&ok);
            if (ok){
                sendPairFloat(data);
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "VADC"){
            pair_t<float> data = pairFromJsonString(params, &ok);
            if (ok){
                driver.tempSensorVoltageCoeff()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "VPPC?"){
            pair_t<float> data = driver.pressSensorVoltagePCoeff()->getValueSync(&ok);
            if (ok){
                sendPairFloat(data);
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "VPPC"){
            pair_t<float> data = pairFromJsonString(params, &ok);
            if (ok){
                driver.pressSensorVoltagePCoeff()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "VPNC?"){
            pair_t<float> data = driver.pressSensorVoltageNCoeff()->getValueSync(&ok);
            if (ok){
                sendPairFloat(data);
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "VPNC"){
            pair_t<float> data = pairFromJsonString(params, &ok);
            if (ok){
                driver.pressSensorVoltageNCoeff()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "PRSC?"){
            pair_t<float> data = driver.pressSensorCoeff()->getValueSync(&ok);
            if (ok){
                sendPairFloat(data);
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "PRSC"){
            pair_t<float> data = pairFromJsonString(params, &ok);
            if (ok){
                driver.pressSensorCoeff()->setValueSync(data, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
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
            bool ok = driver.sendTempTable();
            answer = ok ? PA_Ok : PA_TimeOut;
        }
    }

    if (devInfo->type().contains("CU4TDM1")){
        TempDriverM1 driver;

        driver.setIOInterface(executor()->interface());
        driver.setDevAddress(address);
        if (command == "TEMP?"){
            ok = driver.updateTemperature();
            if (ok) {
                QJsonObject jsonObj;
                for (uint8_t i = 0; i < 4; ++i){
                    QString index = QString("T%1").arg(i);
                    jsonObj[index] = static_cast<double>(driver.currentTemperature(i));
                }
                QJsonDocument jsonDoc(jsonObj);
                executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
                return true;
            }
            answer = PA_TimeOut;
        }
        else if (command.indexOf("TEMP") == 0) {
            auto index = static_cast<uint8_t>(QString(command[4]).toUInt(&ok));
            if (command[5] == '?' && (command.size() == 6) && ok && index < 4){
                ok = driver.updateTemperature(index);
                if (ok){
                    executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<double>(driver.currentTemperature(index))));
                    return true;
                }
                answer = PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "VOLT?"){
            ok = driver.updateVoltage();
            if (ok) {
                QJsonObject jsonObj;
                for (uint8_t i = 0; i < 4; ++i){
                    QString index = QString("U%1").arg(i);
                    jsonObj[index] = static_cast<double>(driver.currentVoltage(i));
                }
                QJsonDocument jsonDoc(jsonObj);
                executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
                return true;
            }
            answer = PA_TimeOut;
        }
        else if (command.indexOf("VOLT") == 0) {
            auto index = static_cast<uint8_t>(QString(command[4]).toUInt(&ok));
            if (command[5] == '?' && (command.size() == 6) && ok && index < 4){
                ok = driver.updateVoltage(index);
                if (ok){
                    executor()->prepareAnswer(QString("%1\r\n").arg(static_cast<double>(driver.currentVoltage(index))));
                    return true;
                }
                answer = PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "CURR"){
            float data = params.toFloat(&ok);
            if (ok) {
                ok = driver.setCurrent(data);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else answer = PA_ErrorData;
        }

        if (command == "SMOD?"){
            auto mode = driver.switcherMode()->getValueSync(&ok);
            if (ok){
                QString out;
                switch (mode) {
                case cmd::enum_CU4TDM1_SwitcherMode::smConnect:
                    out = "Connect\r\n";
                    break;
                case cmd::enum_CU4TDM1_SwitcherMode::smDisconnect:
                    out = "Disconnect\r\n";
                    break;
                case cmd::enum_CU4TDM1_SwitcherMode::smAC:
                    out = "Ac\r\n";
                    break;
                default:
                    break;
                }
                executor()->prepareAnswer(out);
                return true;
            }
            else answer = PA_TimeOut;
        }
        if (command == "SMOD"){
            cmd::enum_CU4TDM1_SwitcherMode mode = cmd::enum_CU4TDM1_SwitcherMode::smNone;
            if (params =="A")
                mode = cmd::enum_CU4TDM1_SwitcherMode::smAC;
            else if (params =="C")
                mode = cmd::enum_CU4TDM1_SwitcherMode::smConnect;
            else if (params =="D")
                mode = cmd::enum_CU4TDM1_SwitcherMode::smDisconnect;

            if (mode != cmd::enum_CU4TDM1_SwitcherMode::smNone){
                driver.switcherMode()->setValueSync(mode, &ok);
                answer = ok ? PA_Ok : PA_TimeOut;
            }
            else
                answer = PA_ErrorData;
        }
        if (command == "RELS?"){
            driver.relaysStatus()->getValueSync(&ok);
            if (ok){
                QJsonObject jsonObj;
                jsonObj["5V"] = (driver.relaysStatus()->currentValue().status() & cRelaysStatus::ri5V)   ? true: false;
                jsonObj["25V"] = (driver.relaysStatus()->currentValue().status() & cRelaysStatus::ri25V) ? true: false;
                QJsonDocument jsonDoc(jsonObj);
                executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
                return true;
            }
            else answer = PA_TimeOut;

        }
        if (command == "RELS"){
            QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
            QJsonObject jsonObj = jsonDoc.object();
            cRelaysStatus rStatus;
            uint8_t status = 0x30;
            if (jsonObj["25V"].toBool()) status++;
            if (jsonObj["5V"].toBool()) status+=2;
            rStatus.setStatus(status);
            driver.relaysStatus()->setValueSync(rStatus, &ok);
            answer = ok ? PA_Ok : PA_TimeOut;
        }

        if (command == "EEPR?"){
            ok = driver.readEepromCoeffs();
            if (ok){
                QJsonObject jsonObj;
                for  (uint8_t i = 0; i < 4; i++){
                    QJsonObject channel;
                    QJsonArray value;
                    value.append(static_cast<double>(driver.eepromCoeff(i).current.first));
                    value.append(static_cast<double>(driver.eepromCoeff(i).current.second));
                    channel["CurrentCoeffs"] = value;
                    value[0] = static_cast<double>(driver.eepromCoeff(i).voltage.first);
                    value[1] = static_cast<double>(driver.eepromCoeff(i).voltage.second);
                    channel["VoltageCoeffs"] = value;
                    jsonObj[QString("Channel%1").arg(i)] = channel;
                }
                QJsonDocument jsonDoc(jsonObj);
                executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "EEPR"){
            QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
            QJsonObject jsonObj = jsonDoc.object();
            TDM1_EepromCoeff data;

            for (uint8_t i = 0; i < 4; ++i){
                QJsonObject channel = jsonObj[QString("Channel%1").arg(i)].toObject();
                QJsonArray value = channel["CurrentCoeffs"].toArray();
                if (value.size() != 2) {
                    executor()->prepareAnswer("Error at CurrentCoeffs value");
                    return true;
                }
                data.current.first  = static_cast<float>(value[0].toDouble());
                data.current.second = static_cast<float>(value[1].toDouble());

                value = channel["VoltageCoeffs"].toArray();
                if (value.size() != 2) {
                    executor()->prepareAnswer("Error at VoltageCoeffs value");
                    return true;
                }
                data.voltage.first  = static_cast<float>(value[0].toDouble());
                data.voltage.second = static_cast<float>(value[1].toDouble());

                driver.setEepromCoeff(i, data);
            }

            ok = driver.writeEepromCoeffs();
            answer = ok ? PA_Ok : PA_TimeOut;
        }
        if (command == "DEFP?"){
            ok = driver.readDefaultParams();
            if (ok){
                QJsonObject jsonObj;
                for  (uint8_t i = 0; i < 4; i++){
                    QJsonObject channel;
                    channel["Current"] = static_cast<double>(driver.defaultParam(i).current);
                    channel["Enable"] = driver.defaultParam(i).enable;
                    jsonObj[QString("Channel%1").arg(i)] = channel;
                }
                QJsonDocument jsonDoc(jsonObj);
                executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "DEFP"){
            QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
            QJsonObject jsonObj = jsonDoc.object();
            DefaultParam data;

            for (uint8_t i = 0; i < 4; ++i){
                QJsonObject channel = jsonObj[QString("Channel%1").arg(i)].toObject();
                data.current = static_cast<float>(channel["Current"].toDouble(0));
                data.enable = channel["Enable"].toBool();
                driver.setDefaultParam(i, data);
            }

            ok = driver.writeDefaultParams();
            answer = ok ? PA_Ok : PA_TimeOut;
        }

        if (command == "TEMT?"){
            ok = driver.receiveTempTables();
            if (ok){
                QJsonObject jsonObj;
                for (uint8_t i = 0; i < 4; ++i){
                    QJsonArray table;
                    CU4TDM0V1_Temp_Table_Item_t *tTable = driver.tempTable(i);
                    QJsonArray value;
                    value.append(0);
                    value.append(1);
                    for (int j = 0; j < TEMP_TABLE_SIZE; ++j){
                        value[0] = static_cast<double>(tTable[j].Voltage);
                        value[1] = static_cast<double>(tTable[j].Temperature);
                        table.append(value);
                    }
                    jsonObj[QString("Channel%1").arg(i)] = table;
                }
                QJsonDocument jsonDoc(jsonObj);
                executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
                return true;
            }
            answer = PA_TimeOut;
        }
        if (command == "TEMT"){
            QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
            QJsonObject jsonObj = jsonDoc.object();
            for (uint8_t i = 0; i < 4; ++i){
                QJsonArray table = jsonObj[QString("Channel%1").arg(i)].toArray();
                CU4TDM0V1_Temp_Table_Item_t tempTable[TEMP_TABLE_SIZE];
                for (int i = 0; i < TEMP_TABLE_SIZE; ++i){
                    QJsonArray value = table[i].toArray();
                    tempTable[i].Temperature    = static_cast<float>(value[0].toDouble());
                    tempTable[i].Voltage        = static_cast<float>(value[1].toDouble());
                }
                driver.setTempTable(i, tempTable);
            }
            bool ok = driver.sendTempTables();
            answer = ok ? PA_Ok : PA_TimeOut;
        }
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
