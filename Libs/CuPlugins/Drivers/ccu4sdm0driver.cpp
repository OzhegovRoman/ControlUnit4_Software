#include "ccu4sdm0driver.h"
#include "star_prc_commands.h"
#include <QDebug>

using namespace cmd;

cCu4SdM0Driver::cCu4SdM0Driver(QObject *parent)
    : AbstractDriver(parent)
    , mDeviceData(new cuDeviceParam<CU4SDM0V1_Data_t>(this, SD_GetData))
    , mCurrent(new cuDeviceParam_settable<float>(this, SD_GetCurrent))
    , mVoltage(new cuDeviceParam<float>(this, SD_GetVoltage))
    , mCounts(new cuDeviceParam<float>(this, SD_GetCounts))
    , mDeviceStatus(new cuDeviceParam_settable<CU4SDM0_Status_t>(this, SD_GetStatus))
    , mDeviceParams(new cuDeviceParam_settable<CU4SDM0V1_Param_t>(this, SD_GetParams))
    , mCmpReferenceLevel(new cuDeviceParam_settable<float>(this, SD_GetCmpRef))
    , mTimeConst(new cuDeviceParam_settable<float>(this, SD_GetTimeConst))
    , mAutoResetThreshold(new cuDeviceParam_settable<float>(this, SD_GetARThreshold))
    , mAutoResetTimeOut(new cuDeviceParam_settable<float>(this, SD_GetARTimeOut))
    , mAutoResetAlarmsCounts(new cuDeviceParam_settable<unsigned int>(this, SD_GetARCounts))
    , mEepromConst(new cuDeviceParam_settable<CU4SDM0V1_EEPROM_Const_t>(this, SD_GetEepromConst))
    , mCurrentAdcCoeff(new cuDeviceParam_settable<pair_t<float> >(this, SD_GetCurrentAdcCoeff))
    , mVoltageAdcCoeff(new cuDeviceParam_settable<pair_t<float> >(this, SD_GetVoltageAdcCoeff))
    , mCurrentDacCoeff(new cuDeviceParam_settable<pair_t<float> >(this, SD_GetCurrentDacCoeff))
    , mCmpReferenceCoeff(new cuDeviceParam_settable<pair_t<float> >(this, SD_GetComparatorCoeff))
    , mPIDEnableStatus(new cuDeviceParam_settable<bool>(this, SD_GetPIDStatus))
{

}

cuDeviceParam<CU4SDM0V1_Data_t> *cCu4SdM0Driver::deviceData() const
{
    return mDeviceData;
}

cuDeviceParam_settable<float> *cCu4SdM0Driver::current() const
{
    return mCurrent;
}

cuDeviceParam<float> *cCu4SdM0Driver::voltage() const
{
    return mVoltage;
}

cuDeviceParam<float> *cCu4SdM0Driver::counts() const
{
    return mCounts;
}

cuDeviceParam_settable<CU4SDM0_Status_t> *cCu4SdM0Driver::deviceStatus() const
{
    return mDeviceStatus;
}

cuDeviceParam_settable<CU4SDM0V1_Param_t> *cCu4SdM0Driver::deviceParams() const
{
    return mDeviceParams;
}

cuDeviceParam_settable<float> *cCu4SdM0Driver::cmpReferenceLevel() const
{
    return mCmpReferenceLevel;
}

cuDeviceParam_settable<float> *cCu4SdM0Driver::timeConst() const
{
    return mTimeConst;
}

cuDeviceParam_settable<float> *cCu4SdM0Driver::autoResetThreshold() const
{
    return mAutoResetThreshold;
}

cuDeviceParam_settable<float> *cCu4SdM0Driver::autoResetTimeOut() const
{
    return mAutoResetTimeOut;
}

cuDeviceParam_settable<unsigned int> *cCu4SdM0Driver::autoResetAlarmsCounts() const
{
    return mAutoResetAlarmsCounts;
}

cuDeviceParam_settable<CU4SDM0V1_EEPROM_Const_t> *cCu4SdM0Driver::eepromConst() const
{
    return mEepromConst;
}

bool cCu4SdM0Driver::pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{

    //TODO: Переделать переопределение типов как это сделано для TempDriver
    if (address != devAddress()) return false;

    if (AbstractDriver::pMsgReceived(address, command, dataLength, data))
        return true;

    CU4SDM0V1_Data_t tmpData;
    CU4SDM0V1_Param_t tmpParams;
    CU4SDM0V1_EEPROM_Const_t tmpEeprom;

    switch (command) {
    //Device Data
    case SD_GetData:
        if (dataLength == sizeof(CU4SDM0V1_Data_t)) {
#ifdef ANDROID
            memcpy(&tmpData, data, sizeof (CU4SDM0V1_Data_t));
#else
            tmpData = *(reinterpret_cast<CU4SDM0V1_Data_t*>(data));
#endif
            mDeviceData->setCurrentValue(tmpData);
            mCurrent->setCurrentValue(tmpData.Current);
            mVoltage->setCurrentValue(tmpData.Voltage);
            mCounts->setCurrentValue(tmpData.Counts);
            mDeviceStatus->setCurrentValue(tmpData.Status);
            emit dataUpdated(tmpData);
        }
        break;
    case SD_GetCurrent:
        if (dataLength == sizeof(float)) {
            tmpData = mDeviceData->getCurrentValue();
            tmpData.Current = *((float*) data);
            mDeviceData->setCurrentValue(tmpData);
            mCurrent->setCurrentValue(tmpData.Current);
            emit dataUpdated(tmpData);
        }
        break;
    case SD_GetVoltage:
        if (dataLength == sizeof(float)) {
            tmpData = mDeviceData->getCurrentValue();
            tmpData.Voltage = *((float*) data);
            mDeviceData->setCurrentValue(tmpData);
            mVoltage->setCurrentValue(tmpData.Voltage);
            emit dataUpdated(tmpData);
        }
        break;
    case SD_GetCounts:
        if (dataLength == sizeof(float)) {
            tmpData = mDeviceData->getCurrentValue();
            tmpData.Counts = *((float*) data);
            //mDeviceData->setCurrentValue(tmpData);
            mCounts->setCurrentValue(tmpData.Counts);
            emit dataUpdated(tmpData);
        }
        break;
    case SD_GetStatus:
        if (dataLength == sizeof(CU4SDM0_Status_t)) {
            tmpData = mDeviceData->getCurrentValue();
            tmpData.Status.Data = * data;
            mDeviceData->setCurrentValue(tmpData);
            mDeviceStatus->setCurrentValue(tmpData.Status);
            emit dataUpdated(tmpData);
        }
        break;
    case SD_GetPIDStatus:
        if (dataLength == sizeof(bool)) {
            mPIDEnableStatus->setCurrentValue(*reinterpret_cast<bool*>(data));
        }
        break;
    case SD_SetCurrent:  //не знаю что делать на эту команду
    case SD_SetStatus:
    case SD_SetShortEnable:
    case SD_SetAmpEnable:
    case SD_SetRfKeyToComparatorEnable:
    case SD_SetComparatorLatchEnable:
    case SD_SetCounterEnable:
    case SD_SetAutoResEnable:
    case SD_SetPIDStatus:
        emit valueSetted();
        break;

        //Device Params
    case SD_GetParams:
        if (dataLength == sizeof(CU4SDM0V1_Param_t)) {
#ifdef ANDROID // я не знаю почему так
            memcpy(&tmpParams, data, sizeof(CU4SDM0V1_Param_t));
#else
            tmpParams = *(reinterpret_cast<CU4SDM0V1_Param_t*>(data));
#endif
            mDeviceParams->setCurrentValue(tmpParams);
            mCmpReferenceLevel->setCurrentValue(tmpParams.Cmp_Ref_Level);
            mTimeConst->setCurrentValue(tmpParams.Time_Const);
            mAutoResetThreshold->setCurrentValue(tmpParams.AutoResetThreshold);
            mAutoResetTimeOut->setCurrentValue(tmpParams.AutoResetTimeOut);
            mAutoResetAlarmsCounts->setCurrentValue(tmpParams.AutoResetCounts);
            emit paramsUpdated(tmpParams);
        }
        break;
    case SD_GetCmpRef:
        if (dataLength == sizeof(float)) {
            tmpParams = mDeviceParams->getCurrentValue();
            tmpParams.Cmp_Ref_Level = *((float*) data);
            mDeviceParams->setCurrentValue(tmpParams);
            mCmpReferenceLevel->setCurrentValue(tmpParams.Cmp_Ref_Level);
            emit paramsUpdated(tmpParams);
        }
        break;
    case SD_GetTimeConst:
        if (dataLength == sizeof(float)) {
            tmpParams = mDeviceParams->getCurrentValue();
            tmpParams.Time_Const = *((float*) data);
            mDeviceParams->setCurrentValue(tmpParams);
            mTimeConst->setCurrentValue(tmpParams.Time_Const);
            emit paramsUpdated(tmpParams);
        }
        break;
    case SD_GetARThreshold:
        if (dataLength == sizeof(float)) {
            tmpParams = mDeviceParams->getCurrentValue();
            tmpParams.AutoResetThreshold = *((float*) data);
            mDeviceParams->setCurrentValue(tmpParams);
            mAutoResetThreshold->setCurrentValue(tmpParams.AutoResetThreshold);
            emit paramsUpdated(tmpParams);
        }
        break;
    case SD_GetARTimeOut:
        if (dataLength == sizeof(float)) {
            tmpParams = mDeviceParams->getCurrentValue();
            tmpParams.AutoResetTimeOut = *((float*) data);
            mDeviceParams->setCurrentValue(tmpParams);
            mAutoResetTimeOut->setCurrentValue(tmpParams.AutoResetTimeOut);
            emit paramsUpdated(tmpParams);
        }
        break;
    case SD_GetARCounts:
        if (dataLength == sizeof(unsigned int)) {
            tmpParams = mDeviceParams->getCurrentValue();
            tmpParams.AutoResetCounts = *((unsigned int*) data);
            mDeviceParams->setCurrentValue(tmpParams);
            mAutoResetAlarmsCounts->setCurrentValue(tmpParams.AutoResetCounts);
            emit paramsUpdated(tmpParams);
        }
        break;
    case SD_SetParams:   //не знаю что делать на эту команду
    case SD_SetCmpRef:
    case SD_SetTimeConst:
    case SD_SetARThreshold:
    case SD_SetARTimeOut:
    case SD_SetARCounts:
        emit paramsSetted();
        break;

        // Device EEPROM
    case SD_GetEepromConst:
        if (dataLength == sizeof(CU4SDM0V1_EEPROM_Const_t)) {
            tmpEeprom = *((CU4SDM0V1_EEPROM_Const_t*) data);
            mEepromConst->setCurrentValue(tmpEeprom);
            mCurrentAdcCoeff->setCurrentValue(tmpEeprom.Current_ADC);
            mVoltageAdcCoeff->setCurrentValue(tmpEeprom.Voltage_ADC);
            mCurrentDacCoeff->setCurrentValue(tmpEeprom.Current_DAC);
            mCmpReferenceCoeff->setCurrentValue(tmpEeprom.Cmp_Ref_DAC);
            emit eepromConstUpdated(tmpEeprom);
        }
        break;
    case SD_GetCurrentAdcCoeff:
        if (dataLength == sizeof(pair_t<float>)) {
            tmpEeprom = mEepromConst->getCurrentValue();
            tmpEeprom.Current_ADC = *((pair_t<float>*) data);
            mEepromConst->setCurrentValue(tmpEeprom);
            mCurrentAdcCoeff->setCurrentValue(tmpEeprom.Current_ADC);
            emit eepromConstUpdated(tmpEeprom);
        }
        break;
    case SD_GetVoltageAdcCoeff:
        if (dataLength == sizeof(pair_t<float>)) {
            tmpEeprom = mEepromConst->getCurrentValue();
            tmpEeprom.Voltage_ADC = *((pair_t<float>*) data);
            mEepromConst->setCurrentValue(tmpEeprom);
            mVoltageAdcCoeff->setCurrentValue(tmpEeprom.Voltage_ADC);
            emit eepromConstUpdated(tmpEeprom);
        }
        break;
    case SD_GetCurrentDacCoeff:
        if (dataLength == sizeof(pair_t<float>)) {
            tmpEeprom = mEepromConst->getCurrentValue();
            tmpEeprom.Current_DAC = *((pair_t<float>*) data);
            mEepromConst->setCurrentValue(tmpEeprom);
            mCurrentDacCoeff->setCurrentValue(tmpEeprom.Current_DAC);
            emit eepromConstUpdated(tmpEeprom);
        }
        break;
    case SD_GetComparatorCoeff:
        if (dataLength == sizeof(pair_t<float>)) {
            tmpEeprom = mEepromConst->getCurrentValue();
            tmpEeprom.Cmp_Ref_DAC = *((pair_t<float>*) data);
            mEepromConst->setCurrentValue(tmpEeprom);
            mCmpReferenceCoeff->setCurrentValue(tmpEeprom.Cmp_Ref_DAC);
            emit eepromConstUpdated(tmpEeprom);
        }
        break;
    case SD_SetEepromConst: //не знаю что делать на эту команду
    case SD_SetCurrentAdcCoeff:
    case SD_SetVoltageAdcCoeff:
    case SD_SetCurrentDacCoeff:
    case SD_SetComparatorCoeff:
        emit eepromConstSetted();
        break;
    default:
        return false;
    }
    return true;

}

void cCu4SdM0Driver::setShortEnable(bool value)
{
    sendMsg(SD_SetShortEnable, sizeof(bool), (quint8 *) &value);
}

void cCu4SdM0Driver::setAmpEnable(bool value)
{
    sendMsg(SD_SetAmpEnable, sizeof(bool), (quint8 *) &value);
}

void cCu4SdM0Driver::setRfKeyEnable(bool value)
{
    sendMsg(SD_SetRfKeyToComparatorEnable, sizeof(bool), (quint8 *) &value);
}

void cCu4SdM0Driver::setCmpLatchEnable(bool value)
{
    sendMsg(SD_SetComparatorLatchEnable, sizeof(bool), (quint8 *) &value);
}

void cCu4SdM0Driver::setCounterEnable(bool value)
{
    sendMsg(SD_SetCounterEnable, sizeof(bool), (quint8 *) &value);
}

void cCu4SdM0Driver::setAutoResetEnable(bool value)
{
    sendMsg(SD_SetAutoResEnable, sizeof(bool), (quint8 *) &value);
}

cuDeviceParam_settable<bool> *cCu4SdM0Driver::PIDEnableStatus() const
{
    return mPIDEnableStatus;
}

cuDeviceParam_settable<pair_t<float> > *cCu4SdM0Driver::cmpReferenceCoeff() const
{
    return mCmpReferenceCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4SdM0Driver::currentDacCoeff() const
{
    return mCurrentDacCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4SdM0Driver::voltageAdcCoeff() const
{
    return mVoltageAdcCoeff;
}

cuDeviceParam_settable<pair_t<float> > *cCu4SdM0Driver::currentAdcCoeff() const
{
    return mCurrentAdcCoeff;
}
