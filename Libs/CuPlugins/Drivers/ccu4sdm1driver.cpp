#include "ccu4sdm1driver.h"
#include "star_prc_commands.h"

using namespace cmd;

cCu4SdM1Driver::cCu4SdM1Driver(QObject *parent)
    : cCu4SdM0Driver(parent)
    , mDeviceData(new cuDeviceParam<CU4SDM1_Data_t>(this, SD_GetData))
    , mCurrentMonitor(new cuDeviceParam<float>(this, SD_GetCurrentMonitor))
    , mEepromConst(new cuDeviceParam_settable<CU4SDM1_EEPROM_Const_t>(this, SD_GetEepromConst))
    , mCurrentMonitorAdcCoeff(new cuDeviceParam_settable<pair_t<float> >(this, SD_GetCurrentMonitorAdcCoeff))
{

}

cuDeviceParam<CU4SDM1_Data_t> *cCu4SdM1Driver::deviceData() const
{
    return mDeviceData;
}

cuDeviceParam_settable<CU4SDM1_EEPROM_Const_t> *cCu4SdM1Driver::eepromConst() const
{
    return mEepromConst;
}

cuDeviceParam_settable<pair_t<float> > *cCu4SdM1Driver::currentMonitorAdcCoeff() const
{
    return mCurrentMonitorAdcCoeff;
}

bool cCu4SdM1Driver::pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{

    //TODO: Переделать переопределение типов как это сделано для TempDriver
    if (address != devAddress()) return false;

    if (AbstractDriver::pMsgReceived(address, command, dataLength, data))
        return true;

    CU4SDM1_Data_t tmpData;
    CU4SDM1_EEPROM_Const_t tmpEeprom;

    switch (command) {
    //Device Data
    case SD_GetData:
        if (dataLength == sizeof(CU4SDM1_Data_t)) {
#ifdef ANDROID
            memcpy(&tmpData, data, sizeof (CU4SDM1_Data_t));
#else
            tmpData = *(reinterpret_cast<CU4SDM1_Data_t*>(data));
#endif
            mDeviceData->setCurrentValue(tmpData);
            current()->setCurrentValue(tmpData.Current);
            mCurrentMonitor->setCurrentValue(tmpData.CurrentMonitor);
            voltage()->setCurrentValue(tmpData.Voltage);
            counts()->setCurrentValue(tmpData.Counts);
            deviceStatus()->setCurrentValue(tmpData.Status);
            emit dataUpdated(tmpData);
        }
        break;
    case SD_GetCurrentMonitor:
        if (dataLength == sizeof(float)) {
            tmpData = mDeviceData->getCurrentValue();
            tmpData.CurrentMonitor = *((float*) data);
            mDeviceData->setCurrentValue(tmpData);
            mCurrentMonitor->setCurrentValue(tmpData.Current);
            emit dataUpdated(tmpData);
        }
        break;

        // Device EEPROM
    case SD_GetEepromConst:
        if (dataLength == sizeof(CU4SDM1_EEPROM_Const_t)) {
            tmpEeprom = *((CU4SDM1_EEPROM_Const_t*) data);
            mEepromConst->setCurrentValue(tmpEeprom);
            currentAdcCoeff()->setCurrentValue(tmpEeprom.Current_ADC);
            mCurrentMonitorAdcCoeff->setCurrentValue(tmpEeprom.CurrentMonitor_ADC);
            voltageAdcCoeff()->setCurrentValue(tmpEeprom.Voltage_ADC);
            currentDacCoeff()->setCurrentValue(tmpEeprom.Current_DAC);
            cmpReferenceCoeff()->setCurrentValue(tmpEeprom.Cmp_Ref_DAC);
            emit eepromConstUpdated(tmpEeprom);
        }
        break;
    case SD_GetCurrentMonitorAdcCoeff:
        if (dataLength == sizeof(pair_t<float>)) {
            tmpEeprom = mEepromConst->getCurrentValue();
            tmpEeprom.CurrentMonitor_ADC = *((pair_t<float>*) data);
            mEepromConst->setCurrentValue(tmpEeprom);
            mCurrentMonitorAdcCoeff->setCurrentValue(tmpEeprom.CurrentMonitor_ADC);
            emit eepromConstUpdated(tmpEeprom);
        }
        break;
    default:
        return cCu4SdM0Driver::pMsgReceived(address,command, dataLength, data);
    }
    return true;
}

cuDeviceParam<float> *cCu4SdM1Driver::currentMonitor() const
{
    return mCurrentMonitor;
}
