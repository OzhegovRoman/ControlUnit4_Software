#include "sisbiassourcedriverm0.h"

SisBiasSourceDriverM0::SisBiasSourceDriverM0(QObject *parent)
    : CommonDriver(parent)
    , mDeviceData               (new DriverPropertyReadOnly<CU4BSM0_Data_t>    (this, cmd::BS_GetData))
    , mCurrent                  (new DriverProperty<float>                     (this, cmd::BS_GetCurrent,                   cmd::BS_SetCurrent))
    , mVoltage                  (new DriverProperty<float>                     (this, cmd::BS_GetVoltage,                   cmd::BS_SetVoltage))
    , mShortEnable              (new DriverProperty<bool>                      (this, cmd::BS_GetShortEnable,               cmd::BS_SetShortEnable))
    , mMode                     (new DriverProperty<char>                      (this, cmd::BS_GetMode,                      cmd::BS_SetMode))

    , mEepromConst              (new DriverProperty<CU4BSM0V0_EEPROM_Const_t>  (this, cmd::BS_GetEepromConst,               cmd::BS_SetEepromConst))
    , mCurrentAdcCoeff          (new DriverProperty<pair_t<float> >            (this, cmd::BS_GetCurrentAdcCoeff,           cmd::BS_SetCurrentAdcCoeff))
    , mCurrentDacCoeff          (new DriverProperty<pair_t<float> >            (this, cmd::BS_GetCurrentDacCoeff,           cmd::BS_SetCurrentDacCoeff))
    , mCurrentLimits            (new DriverProperty<pair_t<float> >            (this, cmd::BS_GetCurrentLimits,             cmd::BS_SetCurrentLimits))
    , mCurrentStep              (new DriverProperty<float>                     (this, cmd::BS_GetCurrentStep,               cmd::BS_SetCurrentStep))
    , mVoltageAdcCoeff          (new DriverProperty<pair_t<float> >            (this, cmd::BS_GetVoltageAdcCoeff,           cmd::BS_SetVoltageAdcCoeff))
    , mVoltageDacCoeff          (new DriverProperty<pair_t<float> >            (this, cmd::BS_GetVoltageDacCoeff,           cmd::BS_SetVoltageDacCoeff))
    , mVoltageLimits            (new DriverProperty<pair_t<float> >            (this, cmd::BS_GetVoltageLimits,             cmd::BS_SetVoltageLimits))
    , mVoltageStep              (new DriverProperty<float>                     (this, cmd::BS_GetVoltageStep,               cmd::BS_SetVoltageStep))
    , mCurrentMonitorResistance (new DriverProperty<float>                     (this, cmd::BS_GetCurrentMonitoResistance,   cmd::BS_SetCurrentMonitoResistance))

    , mPidEnable                (new DriverProperty<bool>                      (this, cmd::BS_GetPIDStatus,                 cmd::BS_SetPIDStatus))
{
    //All data
    mDeviceData->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        mCurrent        ->  setCurrentValue(data.current);
        mVoltage        ->  setCurrentValue(data.voltage);
        mShortEnable    ->  setCurrentValue(data.shortCircuit);
        mMode           ->  setCurrentValue(data.mode);
        updateData(data);
    });
    mCurrent->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.current = mCurrent->currentValue();
        updateData(data);
    });
    mVoltage->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.voltage = mVoltage->currentValue();
        updateData(data);
    });
    mShortEnable->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.shortCircuit = mShortEnable->currentValue();
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
        mCurrentAdcCoeff            -> setCurrentValue(data.currentAdc);
        mCurrentDacCoeff            -> setCurrentValue(data.currentDac);
        mCurrentLimits              -> setCurrentValue(data.currentLimits);
        mCurrentStep                -> setCurrentValue(data.currentStep);
        mVoltageAdcCoeff            -> setCurrentValue(data.voltageAdc);
        mVoltageDacCoeff            -> setCurrentValue(data.voltageDac);
        mVoltageLimits              -> setCurrentValue(data.voltageLimits);
        mVoltageStep                -> setCurrentValue(data.voltageStep);
        mCurrentMonitorResistance   -> setCurrentValue(data.currentMonitorResistance);
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
    mCurrentLimits->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.currentLimits = mCurrentLimits->currentValue();
       updateEEPROM(data);
    });
    mCurrentStep->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.currentStep = mCurrentStep->currentValue();
       updateEEPROM(data);
    });
    mVoltageAdcCoeff->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.voltageAdc = mVoltageAdcCoeff->currentValue();
       updateEEPROM(data);
    });
    mVoltageDacCoeff->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.voltageDac = mVoltageDacCoeff->currentValue();
       updateEEPROM(data);
    });
    mVoltageLimits->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.voltageLimits = mVoltageLimits->currentValue();
       updateEEPROM(data);
    });
    mVoltageStep->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.voltageStep = mVoltageStep->currentValue();
       updateEEPROM(data);
    });
    mCurrentMonitorResistance->gettedSignal()->connect([=](){
       auto data = mEepromConst->currentValue();
       data.currentMonitorResistance = mCurrentMonitorResistance->currentValue();
       updateEEPROM(data);
    });
}

SisBiasSourceDriverM0::~SisBiasSourceDriverM0()
{
    delete mDeviceData               ;
    delete mCurrent                  ;
    delete mVoltage                  ;
    delete mShortEnable              ;
    delete mMode                     ;

    delete mEepromConst              ;
    delete mCurrentAdcCoeff          ;
    delete mCurrentDacCoeff          ;
    delete mCurrentLimits            ;
    delete mCurrentStep              ;
    delete mVoltageAdcCoeff          ;
    delete mVoltageDacCoeff          ;
    delete mVoltageLimits            ;
    delete mVoltageStep              ;
    delete mCurrentMonitorResistance ;

    delete mPidEnable                ;

}

void SisBiasSourceDriverM0::updateData(const CU4BSM0_Data_t &data)
{
    mDeviceData->setCurrentValue(data);
    emit dataUpdated(data);
}

void SisBiasSourceDriverM0::updateEEPROM(const CU4BSM0V0_EEPROM_Const_t &eeprom)
{
    mEepromConst->setCurrentValue(eeprom);
    emit eepromConstUpdated(eeprom);
}

DriverProperty<float> *SisBiasSourceDriverM0::currentMonitorResistance() const
{
    return mCurrentMonitorResistance;
}

DriverProperty<float> *SisBiasSourceDriverM0::voltageStep() const
{
    return mVoltageStep;
}

DriverProperty<pair_t<float> > *SisBiasSourceDriverM0::voltageLimits() const
{
    return mVoltageLimits;
}

DriverProperty<pair_t<float> > *SisBiasSourceDriverM0::voltageDacCoeff() const
{
    return mVoltageDacCoeff;
}

DriverProperty<pair_t<float> > *SisBiasSourceDriverM0::voltageAdcCoeff() const
{
    return mVoltageAdcCoeff;
}

DriverProperty<float> *SisBiasSourceDriverM0::currentStep() const
{
    return mCurrentStep;
}

DriverProperty<pair_t<float> > *SisBiasSourceDriverM0::currentLimits() const
{
    return mCurrentLimits;
}

DriverProperty<pair_t<float> > *SisBiasSourceDriverM0::currentDacCoeff() const
{
    return mCurrentDacCoeff;
}

DriverProperty<pair_t<float> > *SisBiasSourceDriverM0::currentAdcCoeff() const
{
    return mCurrentAdcCoeff;
}

DriverProperty<CU4BSM0V0_EEPROM_Const_t> *SisBiasSourceDriverM0::eepromConst() const
{
    return mEepromConst;
}

DriverProperty<bool> *SisBiasSourceDriverM0::pidEnable() const
{
    return mPidEnable;
}

DriverProperty<char> *SisBiasSourceDriverM0::mode() const
{
    return mMode;
}

DriverProperty<bool> *SisBiasSourceDriverM0::shortEnable() const
{
    return mShortEnable;
}

DriverProperty<float> *SisBiasSourceDriverM0::voltage() const
{
    return mVoltage;
}

DriverProperty<float> *SisBiasSourceDriverM0::current() const
{
    return mCurrent;
}

DriverPropertyReadOnly<CU4BSM0_Data_t> *SisBiasSourceDriverM0::data() const
{
    return mDeviceData;
}
