#ifndef SISCONTROLLINEDRIVERM0_H
#define SISCONTROLLINEDRIVERM0_H

#include "commondriver.h"
#include <QObject>
#include "driverproperty.h"
#include "../StarProtocol/star_prc_structs.h"
#include "../StarProtocol/star_prc_commands.h"

class SisControlLineDriverM0 : public CommonDriver
{
     Q_OBJECT
public:
    explicit SisControlLineDriverM0(QObject *parent = nullptr);
    ~SisControlLineDriverM0();

    DriverPropertyReadOnly<CU4CLM0V0_Data_t>    *data()             const;
    DriverProperty<float>                       *current()          const;
    DriverProperty<bool>                        *shortEnable()      const;

    DriverProperty<CU4CLM0V0_EEPROM_Const_t>    *eepromConst()      const;
    DriverProperty<pair_t<float> >              *currentAdcCoeff()  const;
    DriverProperty<pair_t<float> >              *currentDacCoeff()  const;
    DriverProperty<pair_t<float> >              *currentLimits()    const;
    DriverProperty<float>                       *currentStep()      const;

signals:
    void dataUpdated(CU4CLM0V0_Data_t);
    void eepromConstUpdated(CU4CLM0V0_EEPROM_Const_t);

private slots:
    void updateData(const CU4CLM0V0_Data_t &data);
    void updateEEPROM(const CU4CLM0V0_EEPROM_Const_t &eeprom);

private:
    //DeviceData
    DriverPropertyReadOnly  <CU4CLM0V0_Data_t>          *mDeviceData        ;
    DriverProperty          <float>                     *mCurrent           ;
    DriverProperty          <bool>                      *mShortEnable       ;

    //Device EEPROM
    DriverProperty          <CU4CLM0V0_EEPROM_Const_t>  *mEepromConst       ;
    DriverProperty          <pair_t<float> >            *mCurrentAdcCoeff   ;
    DriverProperty          <pair_t<float> >            *mCurrentDacCoeff   ;
    DriverProperty          <pair_t<float> >            *mCurrentLimits     ;
    DriverProperty          <float>                     *mCurrentStep       ;

};

#endif // SISCONTROLLINEDRIVERM0_H
