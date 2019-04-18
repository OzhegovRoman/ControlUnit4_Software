#ifndef CCU4SDM0DRIVER_H
#define CCU4SDM0DRIVER_H

#include <QObject>
#include "adriver.h"
#include "star_prc_structs.h"

class cCu4SdM0Driver : public AbstractDriver
{
    Q_OBJECT
public:
    explicit cCu4SdM0Driver(QObject *parent = nullptr);

    cuDeviceParam<CU4SDM0V1_Data_t> *deviceData() const;
    cuDeviceParam_settable<float> *current() const;
    cuDeviceParam<float> *voltage() const;
    cuDeviceParam<float> *counts() const;
    cuDeviceParam_settable<CU4SDM0_Status_t> *deviceStatus() const;

    cuDeviceParam_settable<CU4SDM0V1_Param_t> *deviceParams() const;
    cuDeviceParam_settable<float> *cmpReferenceLevel() const;
    cuDeviceParam_settable<float> *timeConst() const;
    cuDeviceParam_settable<float> *autoResetThreshold() const;
    cuDeviceParam_settable<float> *autoResetTimeOut() const;
    cuDeviceParam_settable<unsigned int> *autoResetAlarmsCounts() const;

    cuDeviceParam_settable<CU4SDM0V1_EEPROM_Const_t> *eepromConst() const;
    cuDeviceParam_settable<pair_t<float> > *currentAdcCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *voltageAdcCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *currentDacCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *cmpReferenceCoeff() const;

    cuDeviceParam_settable<bool> *PIDEnableStatus() const;

protected:
    bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data);

signals:
    void dataUpdated(CU4SDM0V1_Data_t);
    void paramsUpdated(CU4SDM0V1_Param_t);
    void eepromConstUpdated(CU4SDM0V1_EEPROM_Const_t);
    void valueSetted();
    void paramsSetted();
    void eepromConstSetted();

public slots:
    // побитовые операции со статусом
    void setShortEnable(bool value);
    void setAmpEnable(bool value);
    void setRfKeyEnable(bool value);
    void setCmpLatchEnable(bool value);
    void setCounterEnable(bool value);
    void setAutoResetEnable(bool value);

private:
    //DeviceData
    cuDeviceParam<CU4SDM0V1_Data_t> *mDeviceData;
    cuDeviceParam_settable<float> *mCurrent;
    cuDeviceParam<float> *mVoltage;
    cuDeviceParam<float> *mCounts;
    cuDeviceParam_settable<CU4SDM0_Status_t> *mDeviceStatus;

    //DeviceParams
    cuDeviceParam_settable<CU4SDM0V1_Param_t> *mDeviceParams;
    cuDeviceParam_settable<float> *mCmpReferenceLevel;
    cuDeviceParam_settable<float> *mTimeConst;
    cuDeviceParam_settable<float> *mAutoResetThreshold;
    cuDeviceParam_settable<float> *mAutoResetTimeOut;
    cuDeviceParam_settable<unsigned int> *mAutoResetAlarmsCounts;

    //Device EEPROM
    cuDeviceParam_settable<CU4SDM0V1_EEPROM_Const_t> *mEepromConst;
    cuDeviceParam_settable<pair_t<float> > *mCurrentAdcCoeff;
    cuDeviceParam_settable<pair_t<float> > *mVoltageAdcCoeff;
    cuDeviceParam_settable<pair_t<float> > *mCurrentDacCoeff;
    cuDeviceParam_settable<pair_t<float> > *mCmpReferenceCoeff;

    //PID Enable Status
    cuDeviceParam_settable<bool> *mPIDEnableStatus;
};

#endif // CCU4SDM0DRIVER_H
