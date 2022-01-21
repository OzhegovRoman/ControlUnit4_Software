#ifndef SISBIASSOURCEDRIVERM0_H
#define SISBIASSOURCEDRIVERM0_H

#include "commondriver.h"
#include <QObject>

#include "driverproperty.h"
#include "../StarProtocol/star_prc_structs.h"
#include "../StarProtocol/star_prc_commands.h"



class SisBiasSourceDriverM0 : public CommonDriver
{
    Q_OBJECT
public:
    explicit SisBiasSourceDriverM0(QObject *parent = nullptr);
    ~SisBiasSourceDriverM0();

    DriverPropertyReadOnly<CU4BSM0_Data_t>      *data()                     const;
    DriverProperty<float>                       *current()                  const;
    DriverProperty<float>                       *voltage()                  const;
    DriverProperty<bool>                        *shortEnable()              const;
    DriverProperty<char>                        *mode()                     const;

    DriverProperty<CU4BSM0V0_EEPROM_Const_t>    *eepromConst()              const;
    DriverProperty<pair_t<float> >              *currentAdcCoeff()          const;
    DriverProperty<pair_t<float> >              *currentDacCoeff()          const;
    DriverProperty<pair_t<float> >              *currentLimits()            const;
    DriverProperty<float>                       *currentStep()              const;
    DriverProperty<pair_t<float> >              *voltageAdcCoeff()          const;
    DriverProperty<pair_t<float> >              *voltageDacCoeff()          const;
    DriverProperty<pair_t<float> >              *voltageLimits()            const;
    DriverProperty<float>                       *voltageStep()              const;
    DriverProperty<float>                       *currentMonitorResistance() const;

    DriverProperty<bool>                        *pidEnable()                const;

signals:
    void dataUpdated(CU4BSM0_Data_t);
    void eepromConstUpdated(CU4BSM0V0_EEPROM_Const_t);

private slots:
    void updateData(const CU4BSM0_Data_t &data);
    void updateEEPROM(const CU4BSM0V0_EEPROM_Const_t &eeprom);

private:
    //DeviceData
    DriverPropertyReadOnly  <CU4BSM0_Data_t>            *mDeviceData                ;
    DriverProperty          <float>                     *mCurrent                   ;
    DriverProperty          <float>                     *mVoltage                   ;
    DriverProperty          <bool>                      *mShortEnable               ;
    DriverProperty          <char>                      *mMode                      ;

    //Device EEPROM
    DriverProperty          <CU4BSM0V0_EEPROM_Const_t>  *mEepromConst               ;
    DriverProperty          <pair_t<float> >            *mCurrentAdcCoeff           ;
    DriverProperty          <pair_t<float> >            *mCurrentDacCoeff           ;
    DriverProperty          <pair_t<float> >            *mCurrentLimits             ;
    DriverProperty          <float>                     *mCurrentStep               ;
    DriverProperty          <pair_t<float> >            *mVoltageAdcCoeff           ;
    DriverProperty          <pair_t<float> >            *mVoltageDacCoeff           ;
    DriverProperty          <pair_t<float> >            *mVoltageLimits             ;
    DriverProperty          <float>                     *mVoltageStep               ;
    DriverProperty          <float>                     *mCurrentMonitorResistance  ;

    //Params
    DriverProperty           <bool>                     *mPidEnable                 ;
};

#endif // SISBIASSOURCEDRIVERM0_H
