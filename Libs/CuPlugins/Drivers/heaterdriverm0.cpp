#include "heaterdriverm0.h"

HeaterDriverM0::HeaterDriverM0(QObject *parent)
    : CommonDriver(parent)

    //DeviceData
    , mDeviceData       (new DriverPropertyReadOnly<CU4HTM0V0_Data_t>   (this, cmd::HT_GetData))
    , mCurrent          (new DriverProperty<float>                      (this, cmd::HT_GetCurrent,          cmd::HT_SetCurrent))
    , mRelayStatus      (new DriverProperty<char>                       (this, cmd::HT_GetRelayStatus,      cmd::HT_SetRelayStatus))
    , mMode             (new DriverProperty<char>                       (this, cmd::HT_GetMode,             cmd::HT_SetMode))

    //DeviceEeprom
    , mEepromConst      (new DriverProperty<CU4HT0V0_EEPROM_Const_t>    (this, cmd::HT_GetEepromConst,      cmd::HT_SetEepromConst))
    , mCurrentAdcCoeff  (new DriverProperty<pair_t<float> >             (this, cmd::HT_GetCurrentAdcCoeff,  cmd::HT_SetCurrentAdcCoeff))
    , mCurrentDacCoeff  (new DriverProperty<pair_t<float> >             (this, cmd::HT_GetCurrentDacCoeff,  cmd::HT_SetCurrentDacCoeff))
    , mMaximumCurrent   (new DriverProperty<float>                      (this, cmd::HT_GetMaxCurrent,       cmd::HT_SetMaxCurrent))
    , mFrontEdgeTime    (new DriverProperty<float>                      (this, cmd::HT_GetFrontEdgeTime,    cmd::HT_SetFrontEdgeTime))
    , mHoldTime         (new DriverProperty<float>                      (this, cmd::HT_GetHoldTime,         cmd::HT_SetHoldTime))
    , mRearEdgeTime     (new DriverProperty<float>                      (this, cmd::HT_GetRearEdgeTime,     cmd::HT_SetRearEdgeTime))

{
    // Обработка Сигналов при посылке-приеме информации от устройства

    //All data
    mDeviceData->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        mCurrent        ->  setCurrentValue(data.current);
        mRelayStatus    ->  setCurrentValue(data.relayStatus);
        mMode           ->  setCurrentValue(data.mode);
        updateData(data);
    });
    mCurrent->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.current = mCurrent->currentValue();
        updateData(data);
    });
    mRelayStatus->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.relayStatus = mRelayStatus->currentValue();
        updateData(data);
    });
    mMode->gettedSignal()->connect([=](){
       auto data = mDeviceData->currentValue();
       data.mode = mMode->currentValue();
       updateData(data);
    });

    //EEPROM
    mEepromConst->gettedSignal()->connect([=](){
        auto data = mEepromConst->currentValue();
        mCurrentAdcCoeff    -> setCurrentValue(data.currentAdc);
        mCurrentDacCoeff    -> setCurrentValue(data.currentDac);
        mMaximumCurrent     -> setCurrentValue(data.maximumCurrent);
        mFrontEdgeTime      -> setCurrentValue(data.frontEdgeTime);
        mHoldTime           -> setCurrentValue(data.holdTime);
        mRearEdgeTime       -> setCurrentValue(data.rearEdgeTime);
        updateEEPROM(data);
    });
    mCurrentAdcCoeff->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.currentAdc = mCurrentAdcCoeff->currentValue();
       updateEEPROM(data);
    });
    mCurrentDacCoeff->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.currentDac = mCurrentDacCoeff->currentValue();
       updateEEPROM(data);
    });
    mMaximumCurrent->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.maximumCurrent = mMaximumCurrent->currentValue();
       updateEEPROM(data);
    });
    mFrontEdgeTime->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.frontEdgeTime = mFrontEdgeTime->currentValue();
       updateEEPROM(data);
    });
    mHoldTime->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.holdTime = mHoldTime->currentValue();
       updateEEPROM(data);
    });
    mRearEdgeTime->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.rearEdgeTime = mRearEdgeTime->currentValue();
       updateEEPROM(data);
    });
}

HeaterDriverM0::~HeaterDriverM0()
{
    delete mDeviceData      ;
    delete mCurrent         ;
    delete mRelayStatus     ;
    delete mMode            ;
    delete mEepromConst     ;
    delete mCurrentAdcCoeff ;
    delete mCurrentDacCoeff ;
    delete mMaximumCurrent  ;
    delete mFrontEdgeTime   ;
    delete mHoldTime        ;
    delete mRearEdgeTime    ;
}

DriverPropertyReadOnly<CU4HTM0V0_Data_t> *HeaterDriverM0::data() const
{
    return mDeviceData;
}

DriverProperty<float> *HeaterDriverM0::current() const
{
    return mCurrent;
}

DriverProperty<char> *HeaterDriverM0::relayStatus() const
{
    return mRelayStatus;
}

DriverProperty<char> *HeaterDriverM0::mode() const
{
    return mMode;
}

DriverProperty<CU4HT0V0_EEPROM_Const_t> *HeaterDriverM0::eepromConst() const
{
    return mEepromConst;
}

DriverProperty<pair_t<float> > *HeaterDriverM0::currentAdcCoeff() const
{
    return mCurrentAdcCoeff;
}

DriverProperty<pair_t<float> > *HeaterDriverM0::currentDacCoeff() const
{
    return mCurrentDacCoeff;
}

DriverProperty<float> *HeaterDriverM0::maximumCurrent() const
{
    return mMaximumCurrent;
}

DriverProperty<float> *HeaterDriverM0::frontEdgeTime() const
{
    return mFrontEdgeTime;
}

DriverProperty<float> *HeaterDriverM0::holdTime() const
{
    return mHoldTime;
}

DriverProperty<float> *HeaterDriverM0::rearEdgeTime() const
{
    return mRearEdgeTime;
}

void HeaterDriverM0::updateData(const CU4HTM0V0_Data_t &data)
{
    mDeviceData->setCurrentValue(data);
    emit dataUpdated(data);
}

void HeaterDriverM0::updateEEPROM(const CU4HT0V0_EEPROM_Const_t &eeprom)
{
    mEepromConst->setCurrentValue(eeprom);
    emit eepromConstUpdated(eeprom);
}
