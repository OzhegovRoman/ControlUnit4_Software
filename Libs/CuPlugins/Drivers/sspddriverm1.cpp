#include "sspddriverm1.h"

SspdDriverM1::SspdDriverM1(QObject *parent)
    : CommonDriver(parent)

    //DeviceData
    , mDeviceData(new DriverPropertyReadOnly<CU4SDM1_Data_t>(this, cmd::SD_GetData))

    //data of DeviceData :)
    , mCurrent(new DriverProperty<float>(this, cmd::SD_GetCurrent, cmd::SD_SetCurrent))
    , mVoltage(new DriverPropertyReadOnly<float>(this, cmd::SD_GetVoltage))
    , mCounts(new DriverPropertyReadOnly<float>(this, cmd::SD_GetCounts))
    , mDeviceStatus(new DriverProperty<CU4SDM1_Status_t>(this, cmd::SD_GetStatus, cmd::SD_SetStatus))

    //atomic properties for Data.Status
    , mShortEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetShortEnable))
    , mAmplifierEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetAmpEnable))
    , mHighFrequencyModeEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetHFModeEnable))
    , mCmpLatchEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetComparatorLatchEnable))
    , mCounterEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetCounterEnable))
    , mAutoResetEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetAutoResEnable))

    //params
    , mDeviceParams(new DriverProperty<CU4SDM0V1_Param_t> (this, cmd::SD_GetParams, cmd::SD_SetParams))

    //data of Params
    , mCmpReferenceLevel(new DriverProperty<float>(this, cmd::SD_GetCmpRef, cmd::SD_SetCmpRef))
    , mTimeConst(new DriverProperty<float>(this, cmd::SD_GetTimeConst, cmd::SD_SetTimeConst))
    , mAutoResetThreshold(new DriverProperty<float>(this, cmd::SD_GetARThreshold, cmd::SD_SetARThreshold))
    , mAutoResetTimeOut(new DriverProperty<float>(this, cmd::SD_GetARTimeOut, cmd::SD_SetARTimeOut))
    , mAutoResetAlarmsCounts(new DriverProperty<unsigned int>(this, cmd::SD_GetARCounts, cmd::SD_SetARCounts))

    // EEPROM
    , mEepromConst(new DriverProperty<CU4SDM1_EEPROM_Const_t> (this, cmd::SD_GetEepromConst, cmd::SD_SetEepromConst))

    // const
    , mCurrentAdcCoeff(new DriverProperty<pair_t<float> >(this, cmd::SD_GetCurrentAdcCoeff, cmd::SD_SetCurrentAdcCoeff))
    , mVoltageAdcCoeff(new DriverProperty<pair_t<float> >(this, cmd::SD_GetVoltageAdcCoeff, cmd::SD_SetVoltageAdcCoeff))
    , mCurrentDacCoeff(new DriverProperty<pair_t<float> >(this, cmd::SD_GetCurrentDacCoeff, cmd::SD_SetCurrentDacCoeff))
    , mCmpReferenceCoeff(new DriverProperty<pair_t<float> >(this, cmd::SD_GetComparatorCoeff, cmd::SD_SetComparatorCoeff))
    , mPulseWidthCoeff(new DriverProperty<float >(this, cmd::SD_GetHFPulseWidth, cmd::SD_SetHFPulseWidth))

    , mPIDEnableStatus(new DriverProperty<bool>(this, cmd::SD_GetPIDStatus, cmd::SD_SetPIDStatus))

{
    // Обработка Сигналов при посылке-приеме информации от устройства

    //All data
    mDeviceData->gettedSignal()->connect([=](){
        //установить данные для всех частных
        auto data = mDeviceData->currentValue();
        mCurrent->setCurrentValue(data.Current);
        mVoltage->setCurrentValue(data.Voltage);
        mCounts->setCurrentValue(data.Counts);
        mDeviceStatus->setCurrentValue(data.Status);
        updateData(data);
    });

    //Data of Data
    mCurrent->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.Current = mCurrent->currentValue();
        updateData(data);
    });
    mCurrent->settedSignal()->connect([=](){ // в предыдущем драйвере этой функции почему то не было, К чему бы это
        auto data = mDeviceData->currentValue();
        data.Current = mCurrent->lastSettedValue();
        updateData(data);
    });

    mVoltage->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.Voltage = mVoltage->currentValue();
        updateData(data);
    });

    mCounts->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.Counts = mCounts->currentValue();
        updateData(data);
    });

    mDeviceStatus->gettedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        updateStatus(data);
    });
    mDeviceStatus->settedSignal()->connect([=](){
       auto data = mDeviceStatus->lastSettedValue();
       updateStatus(data);
    });

    // atomic function of status
    mShortEnable->settedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        data.stShorted = mShortEnable->lastSettedValue();
        updateStatus(data);
    });

    mAmplifierEnable->settedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        data.stAmplifierOn = mAmplifierEnable->lastSettedValue();
        updateStatus(data);
    });

    mHighFrequencyModeEnable->settedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        data.stHFModeOn = mHighFrequencyModeEnable->lastSettedValue();
        updateStatus(data);
    });

    mCmpLatchEnable->settedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        data.stComparatorOn = mCmpLatchEnable->lastSettedValue();
        updateStatus(data);
    });

    mCounterEnable->settedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        data.stCounterOn = mCounterEnable->lastSettedValue();
        updateStatus(data);
    });

    mAutoResetEnable->settedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        data.stAutoResetOn = mAutoResetEnable->lastSettedValue();
        updateStatus(data);
    });

    //Params
    mDeviceParams->gettedSignal()->connect([=](){
        auto data = mDeviceParams->currentValue();
        mCmpReferenceLevel->setCurrentValue(data.Cmp_Ref_Level);
        mTimeConst->setCurrentValue(data.Time_Const);
        mAutoResetThreshold->setCurrentValue(data.AutoResetThreshold);
        mAutoResetTimeOut->setCurrentValue(data.AutoResetTimeOut);
        mAutoResetAlarmsCounts->setCurrentValue(data.AutoResetCounts);
        updateParams(data);
    });

    //params of params
    mCmpReferenceLevel->gettedSignal()->connect([=](){
       auto data = mDeviceParams->currentValue();
       data.Cmp_Ref_Level = mCmpReferenceLevel->currentValue();
       updateParams(data);
    });

    mTimeConst->gettedSignal()->connect([=](){
       auto data = mDeviceParams->currentValue();
       data.Time_Const = mTimeConst->currentValue();
       updateParams(data);
    });

    mAutoResetThreshold->gettedSignal()->connect([=](){
       auto data = mDeviceParams->currentValue();
       data.AutoResetThreshold = mAutoResetThreshold->currentValue();
       updateParams(data);
    });

    mAutoResetTimeOut->gettedSignal()->connect([=](){
       auto data = mDeviceParams->currentValue();
       data.AutoResetTimeOut = mAutoResetTimeOut->currentValue();
       updateParams(data);
    });

    mAutoResetAlarmsCounts->gettedSignal()->connect([=](){
       auto data = mDeviceParams->currentValue();
       data.AutoResetCounts = mAutoResetAlarmsCounts->currentValue();
       updateParams(data);
    });

    //EEPROM
    mEepromConst->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       mCurrentAdcCoeff->setCurrentValue(data.Current_ADC);
       mCurrentDacCoeff->setCurrentValue(data.Current_DAC);
       mVoltageAdcCoeff->setCurrentValue(data.Voltage_ADC);
       mCmpReferenceCoeff->setCurrentValue(data.Cmp_Ref_DAC);
       mPulseWidthCoeff->setCurrentValue(data.PulseWidth);
       updateEEPROM(data);
    });

    mCurrentAdcCoeff->gettedSignal()->connect([=](){
        auto data = mEepromConst->currentValue();
        data.Current_ADC = mCurrentAdcCoeff->currentValue();
        updateEEPROM(data);
    });

    mCurrentDacCoeff->gettedSignal()->connect([=](){
        auto data = mEepromConst->currentValue();
        data.Current_DAC = mCurrentDacCoeff->currentValue();
        updateEEPROM(data);
    });

    mVoltageAdcCoeff->gettedSignal()->connect([=](){
        auto data = mEepromConst->currentValue();
        data.Voltage_ADC = mVoltageAdcCoeff->currentValue();
        updateEEPROM(data);
    });

    mCmpReferenceCoeff->gettedSignal()->connect([=](){
        auto data = mEepromConst->currentValue();
        data.Cmp_Ref_DAC = mCmpReferenceCoeff->currentValue();
        updateEEPROM(data);
    });

    mPulseWidthCoeff->gettedSignal()->connect([=](){
        auto data = mEepromConst->currentValue();
        data.PulseWidth = mPulseWidthCoeff->currentValue();
        updateEEPROM(data);
    });
}

SspdDriverM1::~SspdDriverM1()
{
    delete mDeviceData;
    delete mCurrent;
    delete mVoltage;
    delete mCounts;
    delete mDeviceStatus;
    delete mShortEnable;
    delete mAmplifierEnable;
    delete mHighFrequencyModeEnable;
    delete mCmpLatchEnable;
    delete mCounterEnable;
    delete mAutoResetEnable;
    delete mDeviceParams;
    delete mCmpReferenceLevel;
    delete mTimeConst;
    delete mAutoResetThreshold;
    delete mAutoResetTimeOut;
    delete mAutoResetAlarmsCounts;
    delete mEepromConst;
    delete mCurrentAdcCoeff;
    delete mVoltageAdcCoeff;
    delete mCurrentDacCoeff;
    delete mCmpReferenceCoeff;
    delete mPulseWidthCoeff;     // время срабатывания детектора в секундах
    delete mPIDEnableStatus;
}

void SspdDriverM1::updateData(const CU4SDM1_Data_t &data)
{
    mDeviceData->setCurrentValue(data);
    emit dataUpdated(data);
}

void SspdDriverM1::updateStatus(const CU4SDM1_Status_t &status)
{
    auto data = mDeviceData->currentValue();
    data.Status = status;
    updateData(data);
}

void SspdDriverM1::updateParams(const CU4SDM1_Param_t &params)
{
    mDeviceParams->setCurrentValue(params);
    emit paramsUpdated(params);
}

void SspdDriverM1::updateEEPROM(const CU4SDM1_EEPROM_Const_t &eeprom)
{
    mEepromConst->setCurrentValue(eeprom);
    emit eepromConstUpdated(eeprom);
}

DriverProperty<bool> *SspdDriverM1::PIDEnableStatus() const
{
    return mPIDEnableStatus;
}

DriverProperty<float> *SspdDriverM1::pulseWidthCoeff() const
{
    return mPulseWidthCoeff;
}

DriverProperty<pair_t<float> > *SspdDriverM1::cmpReferenceCoeff() const
{
    return mCmpReferenceCoeff;
}

DriverProperty<pair_t<float> > *SspdDriverM1::currentDacCoeff() const
{
    return mCurrentDacCoeff;
}

DriverProperty<pair_t<float> > *SspdDriverM1::voltageAdcCoeff() const
{
    return mVoltageAdcCoeff;
}

DriverProperty<pair_t<float> > *SspdDriverM1::currentAdcCoeff() const
{
    return mCurrentAdcCoeff;
}

DriverProperty<CU4SDM1_EEPROM_Const_t> *SspdDriverM1::eepromConst() const
{
    return mEepromConst;
}

DriverProperty<unsigned int> *SspdDriverM1::autoResetAlarmsCounts() const
{
    return mAutoResetAlarmsCounts;
}

DriverProperty<float> *SspdDriverM1::autoResetTimeOut() const
{
    return mAutoResetTimeOut;
}

DriverProperty<float> *SspdDriverM1::autoResetThreshold() const
{
    return mAutoResetThreshold;
}

DriverProperty<float> *SspdDriverM1::timeConst() const
{
    return mTimeConst;
}

DriverProperty<float> *SspdDriverM1::cmpReferenceLevel() const
{
    return mCmpReferenceLevel;
}

DriverProperty<CU4SDM1_Param_t> *SspdDriverM1::params() const
{
    return mDeviceParams;
}

DriverPropertyWriteOnly<bool> *SspdDriverM1::autoResetEnable() const
{
    return mAutoResetEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM1::counterEnable() const
{
    return mCounterEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM1::cmpLatchEnable() const
{
    return mCmpLatchEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM1::highFrequencyModeEnable() const
{
    return mHighFrequencyModeEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM1::amplifierEnable() const
{
    return mAmplifierEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM1::shortEnable() const
{
    return mShortEnable;
}

DriverProperty<CU4SDM1_Status_t> *SspdDriverM1::deviceStatus() const
{
    return mDeviceStatus;
}

DriverPropertyReadOnly<float> *SspdDriverM1::counts() const
{
    return mCounts;
}

DriverPropertyReadOnly<float> *SspdDriverM1::voltage() const
{
    return mVoltage;
}

DriverProperty<float> *SspdDriverM1::current() const
{
    return mCurrent;
}

DriverPropertyReadOnly<CU4SDM1_Data_t> *SspdDriverM1::deviceData() const
{
    return mDeviceData;
}
