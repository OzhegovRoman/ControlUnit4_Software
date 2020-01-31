#include "sspddriverm0.h"
#include "star_prc_commands.h"

SspdDriverM0::SspdDriverM0(QObject *parent)
    : CommonDriver(parent)

    //DeviceData
    , mDeviceData(new DriverPropertyReadOnly<CU4SDM0V1_Data_t>(this, cmd::SD_GetData))

    //data of DeviceData :)
    , mCurrent(new DriverProperty<float>(this, cmd::SD_GetCurrent, cmd::SD_SetCurrent))
    , mVoltage(new DriverPropertyReadOnly<float>(this, cmd::SD_GetVoltage))
    , mCounts(new DriverPropertyReadOnly<float>(this, cmd::SD_GetCounts))
    , mDeviceStatus(new DriverProperty<CU4SDM0_Status_t>(this, cmd::SD_GetStatus, cmd::SD_SetStatus))

    //atomic properties for Data.Status
    , mShortEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetShortEnable))
    , mAmplifierEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetAmpEnable))
    , mRfKeyEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetRfKeyToComparatorEnable))
    , mCmpLatchEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetComparatorLatchEnable))
    , mCounterEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetCounterEnable))
    , mAutoResetEnable(new DriverPropertyWriteOnly<bool>(this, cmd::SD_SetAutoResEnable))

    //params
    , mDeviceParams(new DriverProperty<CU4SDM0V1_Param_t> (this, cmd::SD_GetParams, cmd::SD_SetParams))

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

    mRfKeyEnable->settedSignal()->connect([=](){
        auto data = mDeviceStatus->currentValue();
        data.stRfKeyToCmp = mRfKeyEnable->lastSettedValue();
        updateStatus(data);
    });

    mCmpLatchEnable->settedSignal()->connect([=](){
        //TODO: есть подозрение, что это будет работать плохо и неправильно
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

    });
}

SspdDriverM0::~SspdDriverM0()
{
    delete  mDeviceData;
    delete  mCurrent;
    delete  mVoltage;
    delete  mCounts;
    delete  mDeviceStatus;
    delete  mShortEnable;
    delete  mAmplifierEnable;
    delete  mRfKeyEnable;
    delete  mCmpLatchEnable;
    delete  mCounterEnable;
    delete  mAutoResetEnable;
    delete  mDeviceParams;
}

DriverPropertyReadOnly<CU4SDM0V1_Data_t> *SspdDriverM0::data() const
{
    return mDeviceData;
}

DriverProperty<float> *SspdDriverM0::current() const
{
    return mCurrent;
}

DriverPropertyReadOnly<float> *SspdDriverM0::voltage() const
{
    return mVoltage;
}

DriverPropertyReadOnly<float> *SspdDriverM0::counts() const
{
    return mCounts;
}

DriverProperty<CU4SDM0_Status_t> *SspdDriverM0::status() const
{
    return mDeviceStatus;
}

void SspdDriverM0::updateData(const CU4SDM0V1_Data_t &data)
{
    mDeviceData->setCurrentValue(data);
    emit dataUpdated(data);
}

void SspdDriverM0::updateStatus(const CU4SDM0_Status_t &status)
{
    auto data = mDeviceData->currentValue();
    data.Status = status;
    updateData(data);
}

void SspdDriverM0::updateParams(const CU4SDM0V1_Param_t &params)
{
    mDeviceParams->setCurrentValue(params);
    emit paramsUpdated(params);
}

DriverProperty<CU4SDM0V1_Param_t> *SspdDriverM0::params() const
{
    return mDeviceParams;
}

DriverPropertyWriteOnly<bool> *SspdDriverM0::autoResetEnable() const
{
    return mAutoResetEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM0::counterEnable() const
{
    return mCounterEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM0::cmpLatchEnable() const
{
    return mCmpLatchEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM0::rfKeyEnable() const
{
    return mRfKeyEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM0::amplifierEnable() const
{
    return mAmplifierEnable;
}

DriverPropertyWriteOnly<bool> *SspdDriverM0::shortEnable() const
{
    return mShortEnable;
}
