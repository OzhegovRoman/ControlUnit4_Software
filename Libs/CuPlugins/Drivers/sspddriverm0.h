#ifndef SSPDDRIVERM0_H
#define SSPDDRIVERM0_H

#include <QObject>
#include "commondriver.h"
#include "driverproperty.h"
#include "../StarProtocol/star_prc_structs.h"

class SspdDriverM0 : public CommonDriver
{
    Q_OBJECT
public:
    explicit SspdDriverM0(QObject *parent = nullptr);
    ~SspdDriverM0();

    DriverPropertyReadOnly<CU4SDM0V1_Data_t> *data() const;

    DriverProperty<float> *current() const;
    DriverPropertyReadOnly<float> *voltage() const;
    DriverPropertyReadOnly<float> *counts() const;
    DriverProperty<CU4SDM0_Status_t> *status() const;

    DriverPropertyWriteOnly<bool> *shortEnable() const;
    DriverPropertyWriteOnly<bool> *amplifierEnable() const;
    DriverPropertyWriteOnly<bool> *rfKeyEnable() const;
    DriverPropertyWriteOnly<bool> *cmpLatchEnable() const;
    DriverPropertyWriteOnly<bool> *counterEnable() const;
    DriverPropertyWriteOnly<bool> *autoResetEnable() const;

    DriverProperty<CU4SDM0V1_Param_t> *params() const;

    DriverProperty<float> *cmpReferenceLevel() const;
    DriverProperty<float> *timeConst() const;
    DriverProperty<float> *autoResetThreshold() const;
    DriverProperty<float> *autoResetTimeOut() const;
    DriverProperty<unsigned int> *autoResetAlarmsCounts() const;

    DriverProperty<CU4SDM0V1_EEPROM_Const_t> *eepromConst() const;

    DriverProperty<pair_t<float> > *currentAdcCoeff() const;
    DriverProperty<pair_t<float> > *voltageAdcCoeff() const;
    DriverProperty<pair_t<float> > *currentDacCoeff() const;
    DriverProperty<pair_t<float> > *cmpReferenceCoeff() const;

    DriverProperty<bool> *PIDEnableStatus() const;

signals:
    void dataUpdated(CU4SDM0V1_Data_t);
    void paramsUpdated(CU4SDM0V1_Param_t);
    void eepromConstUpdated(CU4SDM0V1_EEPROM_Const_t);

private slots:
    void updateData(const CU4SDM0V1_Data_t &data);
    void updateStatus(const CU4SDM0_Status_t &status);
    void updateParams(const CU4SDM0V1_Param_t &params);
    void updateEEPROM(const CU4SDM0V1_EEPROM_Const_t &eeprom);

private:
    //DeviceData
    DriverPropertyReadOnly<CU4SDM0V1_Data_t> *mDeviceData;
    DriverProperty<float> *mCurrent;
    DriverPropertyReadOnly<float> *mVoltage;
    DriverPropertyReadOnly<float> *mCounts;
    DriverProperty<CU4SDM0_Status_t> *mDeviceStatus;

    //DeviceDataStatus
    DriverPropertyWriteOnly<bool> *mShortEnable;
    DriverPropertyWriteOnly<bool> *mAmplifierEnable;
    DriverPropertyWriteOnly<bool> *mRfKeyEnable;
    DriverPropertyWriteOnly<bool> *mCmpLatchEnable;
    DriverPropertyWriteOnly<bool> *mCounterEnable;
    DriverPropertyWriteOnly<bool> *mAutoResetEnable;

    //DeviceParams
    DriverProperty<CU4SDM0V1_Param_t> *mDeviceParams;
    DriverProperty<float> *mCmpReferenceLevel;
    DriverProperty<float> *mTimeConst;
    DriverProperty<float> *mAutoResetThreshold;
    DriverProperty<float> *mAutoResetTimeOut;
    DriverProperty<unsigned int> *mAutoResetAlarmsCounts;

    //Device EEPROM
    DriverProperty<CU4SDM0V1_EEPROM_Const_t> *mEepromConst;
    DriverProperty<pair_t<float> > *mCurrentAdcCoeff;
    DriverProperty<pair_t<float> > *mVoltageAdcCoeff;
    DriverProperty<pair_t<float> > *mCurrentDacCoeff;
    DriverProperty<pair_t<float> > *mCmpReferenceCoeff;

    //PID Enable Status
    DriverProperty<bool> *mPIDEnableStatus;

};

#endif // SSPDDRIVERM0_H
