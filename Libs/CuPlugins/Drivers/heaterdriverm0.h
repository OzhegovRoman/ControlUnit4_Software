#ifndef HEATERDRIVERM0_H
#define HEATERDRIVERM0_H

#include "commondriver.h"
#include <QObject>
#include "driverproperty.h"
#include "../StarProtocol/star_prc_structs.h"
#include "../StarProtocol/star_prc_commands.h"

class HeaterDriverM0 : public CommonDriver
{
    Q_OBJECT
public:
    explicit HeaterDriverM0(QObject *parent = nullptr);
    ~HeaterDriverM0();

    //DeviceData
    DriverPropertyReadOnly<CU4HTM0V0_Data_t> *data() const;
    DriverProperty<float> *current() const;
    DriverProperty<char> *relayStatus() const;
    DriverProperty<char> *mode() const;

    //Device EEPROM
    DriverProperty<CU4HT0V0_EEPROM_Const_t> *eepromConst() const;
    DriverProperty<pair_t<float> > *currentAdcCoeff() const;
    DriverProperty<pair_t<float> > *currentDacCoeff() const;
    DriverProperty<float> *maximumCurrent() const;
    DriverProperty<float> *frontEdgeTime() const;
    DriverProperty<float> *holdTime() const;
    DriverProperty<float> *rearEdgeTime() const;

signals:
    void dataUpdated(CU4HTM0V0_Data_t);
    void eepromConstUpdated(CU4HT0V0_EEPROM_Const_t);

private slots:
    void updateData(const CU4HTM0V0_Data_t &data);
    void updateEEPROM(const CU4HT0V0_EEPROM_Const_t &eeprom);

private:
    //DeviceData
    DriverPropertyReadOnly  <CU4HTM0V0_Data_t>          *mDeviceData;
    DriverProperty          <float>                     *mCurrent;
    DriverProperty          <char>                      *mRelayStatus;
    DriverProperty          <char>                      *mMode;

    //Device EEPROM
    DriverProperty          <CU4HT0V0_EEPROM_Const_t>   *mEepromConst;
    DriverProperty          <pair_t<float> >            *mCurrentAdcCoeff;
    DriverProperty          <pair_t<float> >            *mCurrentDacCoeff;
    DriverProperty          <float>                     *mMaximumCurrent;
    DriverProperty          <float>                     *mFrontEdgeTime;
    DriverProperty          <float>                     *mHoldTime;
    DriverProperty          <float>                     *mRearEdgeTime;
};

#endif // HEATERDRIVERM0_H
