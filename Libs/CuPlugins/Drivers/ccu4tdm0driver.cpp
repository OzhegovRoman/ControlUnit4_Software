#include "ccu4tdm0driver.h"
#include "star_prc_commands.h"
#include <QDebug>

using namespace cmd;

cCu4TdM0Driver::cCu4TdM0Driver(QObject *parent):
    AbstractDriver(parent),
    mDeviceData(new cuDeviceParam<CU4TDM0V1_Data_t>(this, TD_GetData)),
    mTemperature(new cuDeviceParam<float>(this, TD_GetTemperature)),
    mPressure(new cuDeviceParam<float>(this, TD_GetPressure)),
    mTempSensorCurrent(new cuDeviceParam_settable<float>(this, TD_GetTempSensorCurrent)),
    mTempSensorVoltage(new cuDeviceParam<float>(this, TD_GetTempSensorVoltage)),
    mPressSensorVoltageP(new cuDeviceParam<float>(this, TD_GetPressSensorVoltageP)),
    mPressSensorVoltageN(new cuDeviceParam<float>(this, TD_GetPressSensorVoltageN)),
    mCommutatorOn(new cuDeviceParam_settable<bool>(this, TD_GetTurnOnStatus)),

    mEepromConst(new cuDeviceParam_settable<CU4TDM0V1_EEPROM_Const_t>(this, TD_GetEepromConst)),
    mTempSensorCurrentAdcCoeff(new cuDeviceParam_settable<pair_t<float> >(this, TD_GetTempSensorCurrentAdcCoeff)),
    mTempSensorCurrentDacCoeff(new cuDeviceParam_settable<pair_t<float> >(this, TD_GetTempSensorCurrentDacCoeff)),
    mTempSensorVoltageCoeff(new cuDeviceParam_settable<pair_t<float> >(this, TD_GetTempSensorVoltageCoeff)),
    mPressSensorVoltagePCoeff(new cuDeviceParam_settable<pair_t<float> >(this, TD_GetPressSensorVoltagePCoeff)),
    mPressSensorVoltageNCoeff(new cuDeviceParam_settable<pair_t<float> >(this, TD_GetPressSensorVoltageNCoeff)),
    mPressSensorCoeff(new cuDeviceParam_settable<pair_t<float> >(this, TD_GetPressSensorCoeff))
{

}

void cCu4TdM0Driver::commutatorTurnOn(bool value)
{
    sendMsg(TD_SetTurnOnStatus, sizeof(bool), (quint8*) &value);
    waitingAnswer();
}

bool cCu4TdM0Driver::sendTempTable()
{
    uint8_t errorCount = 0;
    uint8_t buffer[10*sizeof(CU4TDM0V1_Temp_Table_Item_t)+2];
    offset = 0;
    while (offset<TEMP_TABLE_SIZE) {
        count = qMin(10, TEMP_TABLE_SIZE - offset);
        buffer[0] = offset;
        buffer[1] = count;
        memcpy(&buffer[2], &mTempTable[offset], sizeof(CU4TDM0V1_Temp_Table_Item_t)*count);
        sendMsg(TD_SetTempTableValues, sizeof(CU4TDM0V1_Temp_Table_Item_t) * count + sizeof(uint8_t) * 2, buffer);
        if (!waitingAnswer())
            errorCount++;
        else
            offset += count;
        if (errorCount>5)
            return false;
    }
    return true;
}

bool cCu4TdM0Driver::receiveTempTable()
{
    uint8_t errorCount = 0;
    offset = 0;
    uint8_t buffer[2];
    while (offset<TEMP_TABLE_SIZE){
        count = qMin(10, TEMP_TABLE_SIZE-offset);
        buffer[0] = offset;
        buffer[1] = count;
        sendMsg(TD_GetTempTableValues, sizeof(uint8_t) * 2, buffer);
        if (!waitingAnswer())
            errorCount++;
        else
            offset += count;
        if (errorCount > 5)
            return false;
    }
    return true;
}

CU4TDM0V1_Temp_Table_Item_t *cCu4TdM0Driver::tempTable()
{
    return (CU4TDM0V1_Temp_Table_Item_t*)&mTempTable;
}

bool cCu4TdM0Driver::pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    if (address != devAddress()) return false;

    if (AbstractDriver::pMsgReceived(address, command, dataLength, data))
        return true;
    switch (command) {
    case TD_GetData: {
        if (dataLength == sizeof(CU4TDM0V1_Data_t)){
            CU4TDM0V1_Data_t tmp;
            memcpy(&tmp, data, sizeof(CU4TDM0V1_Data_t));
            mDeviceData->setCurrentValue(tmp);
            mTemperature->setCurrentValue(tmp.Temperature);
            mPressure->setCurrentValue(tmp.Pressure);
            mTempSensorCurrent->setCurrentValue(tmp.TempSensorCurrent);
            mTempSensorVoltage->setCurrentValue(tmp.TempSensorVoltage);
            mPressSensorVoltageP->setCurrentValue(tmp.PressSensorVoltageP);
            mPressSensorVoltageN->setCurrentValue(tmp.PressSensorVoltageN);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_GetTemperature: {
        if (dataLength == sizeof(float)) {
            CU4TDM0V1_Data_t tmp = mDeviceData->getCurrentValue();
            tmp.Temperature = *((float*) data);
            mDeviceData->setCurrentValue(tmp);
            mTemperature->setCurrentValue(tmp.Temperature);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_GetPressure: {
        if (dataLength == sizeof(float)) {
            CU4TDM0V1_Data_t tmp = mDeviceData->getCurrentValue();
            tmp.Pressure = *((float*) data);
            mDeviceData->setCurrentValue(tmp);
            mPressure->setCurrentValue(tmp.Pressure);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_GetTempSensorCurrent: {
        if (dataLength == sizeof(float)) {
            CU4TDM0V1_Data_t tmp = mDeviceData->getCurrentValue();
            tmp.TempSensorCurrent = *((float*) data);
            mDeviceData->setCurrentValue(tmp);
            mTempSensorCurrent->setCurrentValue(tmp.TempSensorCurrent);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_GetTempSensorVoltage: {
        if (dataLength == sizeof(float)) {
            CU4TDM0V1_Data_t tmp = mDeviceData->getCurrentValue();
            tmp.TempSensorVoltage = *((float*) data);
            mDeviceData->setCurrentValue(tmp);
            mTempSensorVoltage->setCurrentValue(tmp.TempSensorVoltage);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_GetPressSensorVoltageP: {
        if (dataLength == sizeof(float)) {
            CU4TDM0V1_Data_t tmp = mDeviceData->getCurrentValue();
            tmp.PressSensorVoltageP = *((float*) data);
            mDeviceData->setCurrentValue(tmp);
            mPressSensorVoltageP->setCurrentValue(tmp.PressSensorVoltageP);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_GetPressSensorVoltageN: {
        if (dataLength == sizeof(float)) {
            CU4TDM0V1_Data_t tmp = mDeviceData->getCurrentValue();
            tmp.PressSensorVoltageN = *((float*) data);
            mDeviceData->setCurrentValue(tmp);
            mPressSensorVoltageN->setCurrentValue(tmp.PressSensorVoltageN);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_GetTurnOnStatus: {
        if (dataLength == sizeof(bool)) {
            CU4TDM0V1_Data_t tmp = mDeviceData->getCurrentValue();
            tmp.CommutatorOn = *((bool*) data);
            mDeviceData->setCurrentValue(tmp);
            mCommutatorOn->setCurrentValue(tmp.CommutatorOn);
            emit dataUpdated(tmp);
        }
    } break;
    case TD_SetTempSensorCurrent:           //не знаю что делать
    case TD_SetTurnOnStatus:
        emit valueSetted();
        break;
        // Device EEPROM
    case TD_GetEepromConst: {
        if (dataLength == sizeof(CU4TDM0V1_EEPROM_Const_t)){
            CU4TDM0V1_EEPROM_Const_t tmp;
            memcpy(&tmp, data, sizeof(CU4TDM0V1_EEPROM_Const_t));
            mEepromConst->setCurrentValue(tmp);
            mTempSensorCurrentAdcCoeff->setCurrentValue(tmp.tempSensorCurrentAdc);
            mTempSensorCurrentDacCoeff->setCurrentValue(tmp.tempSensorCurrentDac);
            mTempSensorVoltageCoeff->setCurrentValue(tmp.tempSensorVoltage);
            mPressSensorVoltagePCoeff->setCurrentValue(tmp.pressSensorVoltageP);
            mPressSensorVoltageNCoeff->setCurrentValue(tmp.pressSensorVoltageN);
            mPressSensorCoeff->setCurrentValue(tmp.pressSensorCoeffs);
            emit eepromConstUpdated(tmp);
        }
    } break;
    case TD_GetTempSensorCurrentAdcCoeff: {
        if (dataLength == sizeof(pair_t<float>)) {
            pair_t<float> tmp;
            memcpy(&tmp, data, sizeof(pair_t<float>));
            mTempSensorCurrentAdcCoeff->setCurrentValue(tmp);
            CU4TDM0V1_EEPROM_Const_t eeprom = mEepromConst->getCurrentValue();
            eeprom.tempSensorCurrentAdc = tmp;
            mEepromConst->setCurrentValue(eeprom);
            emit eepromConstUpdated(eeprom);
        }
    } break;
    case TD_GetTempSensorCurrentDacCoeff: {
        if (dataLength == sizeof(pair_t<float>)) {
            pair_t<float> tmp;
            memcpy(&tmp, data, sizeof(pair_t<float>));
            mTempSensorCurrentDacCoeff->setCurrentValue(tmp);
            CU4TDM0V1_EEPROM_Const_t eeprom = mEepromConst->getCurrentValue();
            eeprom.tempSensorCurrentDac = tmp;
            mEepromConst->setCurrentValue(eeprom);
            emit eepromConstUpdated(eeprom);
        }
    } break;
    case TD_GetTempSensorVoltageCoeff: {
        if (dataLength == sizeof(pair_t<float>)) {
            pair_t<float> tmp;
            memcpy(&tmp, data, sizeof(pair_t<float>));
            mTempSensorVoltageCoeff->setCurrentValue(tmp);
            CU4TDM0V1_EEPROM_Const_t eeprom = mEepromConst->getCurrentValue();
            eeprom.tempSensorVoltage = tmp;
            mEepromConst->setCurrentValue(eeprom);
            emit eepromConstUpdated(eeprom);
        }
    } break;
    case TD_GetPressSensorVoltagePCoeff: {
        if (dataLength == sizeof(pair_t<float>)) {
            pair_t<float> tmp;
            memcpy(&tmp, data, sizeof(pair_t<float>));
            mPressSensorVoltagePCoeff->setCurrentValue(tmp);
            CU4TDM0V1_EEPROM_Const_t eeprom = mEepromConst->getCurrentValue();
            eeprom.pressSensorVoltageP = tmp;
            mEepromConst->setCurrentValue(eeprom);
            emit eepromConstUpdated(eeprom);
        }
    } break;
    case TD_GetPressSensorVoltageNCoeff: {
        if (dataLength == sizeof(pair_t<float>)) {
            pair_t<float> tmp;
            memcpy(&tmp, data, sizeof(pair_t<float>));
            mPressSensorVoltageNCoeff->setCurrentValue(tmp);
            CU4TDM0V1_EEPROM_Const_t eeprom = mEepromConst->getCurrentValue();
            eeprom.pressSensorVoltageN = tmp;
            mEepromConst->setCurrentValue(eeprom);
            emit eepromConstUpdated(eeprom);
        }
    } break;
    case TD_GetPressSensorCoeff: {
        if (dataLength == sizeof(pair_t<float>)) {
            pair_t<float> tmp;
            memcpy(&tmp, data, sizeof(pair_t<float>));
            mPressSensorCoeff->setCurrentValue(tmp);
            CU4TDM0V1_EEPROM_Const_t eeprom = mEepromConst->getCurrentValue();
            eeprom.pressSensorCoeffs = tmp;
            mEepromConst->setCurrentValue(eeprom);
            emit eepromConstUpdated(eeprom);
        }
    } break;
    case TD_SetEepromConst:                 //не знаю что делать
    case TD_SetTempSensorCurrentAdcCoeff:
    case TD_SetTempSensorCurrentDacCoeff:
    case TD_SetTempSensorVoltageCoeff:
    case TD_SetPressSensorVoltagePCoeff:
    case TD_SetPressSensorVoltageNCoeff:
    case TD_SetPressSensorCoeff:
        emit eepromConstSetted();
        break;
    case TD_GetTempTableValues: {
        if (dataLength == sizeof(CU4TDM0V1_Temp_Table_Item_t)*count) {
            CU4TDM0V1_Temp_Table_Item_t* tmp = (CU4TDM0V1_Temp_Table_Item_t*) data;
            for (int i = 0; i < count; ++i)
                mTempTable[offset+i] = *(tmp+i);
        }
    } break;
    case TD_SetTempTableValues: {           //не знаю что делать
    }  break;
    default:
        return false;
        break;
    }
    return true;
}

cuDeviceParam_settable<bool> *cCu4TdM0Driver::commutatorOn() const
{
    return mCommutatorOn;
}

cuDeviceParam<float> *cCu4TdM0Driver::pressSensorVoltageN() const
{
    return mPressSensorVoltageN;
}

cuDeviceParam<float> *cCu4TdM0Driver::pressSensorVoltageP() const
{
    return mPressSensorVoltageP;
}

cuDeviceParam<float> *cCu4TdM0Driver::tempSensorVoltage() const
{
    return mTempSensorVoltage;
}

cuDeviceParam<float> *cCu4TdM0Driver::pressure() const
{
    return mPressure;
}

cuDeviceParam<float> *cCu4TdM0Driver::temperature() const
{
    return mTemperature;
}

cuDeviceParam_settable<pair_t<float> > *cCu4TdM0Driver::pressSensorCoeff() const
{
    return mPressSensorCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4TdM0Driver::pressSensorVoltageNCoeff() const
{
    return mPressSensorVoltageNCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4TdM0Driver::pressSensorVoltagePCoeff() const
{
    return mPressSensorVoltagePCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4TdM0Driver::tempSensorVoltageCoeff() const
{
    return mTempSensorVoltageCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4TdM0Driver::tempSensorCurrentDacCoeff() const
{
    return mTempSensorCurrentDacCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4TdM0Driver::tempSensorCurrentAdcCoeff() const
{
    return mTempSensorCurrentAdcCoeff;
}

cuDeviceParam_settable<CU4TDM0V1_EEPROM_Const_t> *cCu4TdM0Driver::eepromConst() const
{
    return mEepromConst;
}

cuDeviceParam<CU4TDM0V1_Data_t> *cCu4TdM0Driver::deviceData() const
{
    return mDeviceData;
}

cuDeviceParam_settable<float> *cCu4TdM0Driver::tempSensorCurrent() const
{
    return mTempSensorCurrent;
}
