#include "siscontrollinedriverm0.h"

SisControlLineDriverM0::SisControlLineDriverM0(QObject *parent)
    : CommonDriver(parent)
    , mDeviceData        (new DriverPropertyReadOnly<CU4CLM0V0_Data_t>  (this, cmd::CL_GetData))
    , mCurrent           (new DriverProperty<float>                     (this, cmd::CL_GetCurrent,          cmd::CL_SetCurrent))
    , mShortEnable       (new DriverProperty<bool>                      (this, cmd::CL_GetShortEnable,      cmd::CL_SetShortEnable))

    , mEepromConst       (new DriverProperty<CU4CLM0V0_EEPROM_Const_t>  (this, cmd::CL_GetEepromConst,      cmd::CL_SetEepromConst))
    , mCurrentAdcCoeff   (new DriverProperty<pair_t<float> >            (this, cmd::CL_GetCurrentAdcCoeff,  cmd::CL_SetCurrentAdcCoeff))
    , mCurrentDacCoeff   (new DriverProperty<pair_t<float> >            (this, cmd::CL_GetCurrentDacCoeff,  cmd::CL_SetCurrentDacCoeff))
    , mCurrentLimits     (new DriverProperty<pair_t<float> >            (this, cmd::CL_GetCurrentLimits,    cmd::CL_SetCurrentLimits))
    , mCurrentStep       (new DriverProperty<float>                     (this, cmd::CL_GetCurrentStep,      cmd::CL_SetCurrentStep))
{

}

SisControlLineDriverM0::~SisControlLineDriverM0()
{
    delete mDeviceData        ;
    delete mCurrent           ;
    delete mShortEnable       ;
    delete mEepromConst       ;
    delete mCurrentAdcCoeff   ;
    delete mCurrentDacCoeff   ;
    delete mCurrentLimits     ;
    delete mCurrentStep       ;
}

void SisControlLineDriverM0::updateData(const CU4CLM0V0_Data_t &data)
{
    mDeviceData->setCurrentValue(data);
    emit dataUpdated(data);
}

void SisControlLineDriverM0::updateEEPROM(const CU4CLM0V0_EEPROM_Const_t &eeprom)
{
    mEepromConst->setCurrentValue(eeprom);
    emit eepromConstUpdated(eeprom);
}

DriverProperty<float> *SisControlLineDriverM0::currentStep() const
{
    return mCurrentStep;
}

DriverProperty<pair_t<float> > *SisControlLineDriverM0::currentLimits() const
{
    return mCurrentLimits;
}

DriverProperty<pair_t<float> > *SisControlLineDriverM0::currentDacCoeff() const
{
    return mCurrentDacCoeff;
}

DriverProperty<pair_t<float> > *SisControlLineDriverM0::currentAdcCoeff() const
{
    return mCurrentAdcCoeff;
}

DriverProperty<CU4CLM0V0_EEPROM_Const_t> *SisControlLineDriverM0::eepromConst() const
{
    return mEepromConst;
}

DriverProperty<bool> *SisControlLineDriverM0::shortEnable() const
{
    return mShortEnable;
}

DriverProperty<float> *SisControlLineDriverM0::current() const
{
    return mCurrent;
}

DriverPropertyReadOnly<CU4CLM0V0_Data_t> *SisControlLineDriverM0::data() const
{
    return mDeviceData;
}
