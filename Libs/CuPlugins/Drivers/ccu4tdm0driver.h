#ifndef CCU4TDM0DRIVER_H
#define CCU4TDM0DRIVER_H

#include <QObject>
#include "adriver.h"
#include "star_prc_structs.h"

class cCu4TdM0Driver : public AbstractDriver
{
    Q_OBJECT
public:
    explicit cCu4TdM0Driver(QObject *parent = nullptr);

    void commutatorTurnOn(bool value);

    bool sendTempTable();
    bool receiveTempTable();

    CU4TDM0V1_Temp_Table_Item_t* tempTable();

    cuDeviceParam<CU4TDM0V1_Data_t> *deviceData() const;
    cuDeviceParam<float> *temperature() const;
    cuDeviceParam<float> *pressure() const;
    cuDeviceParam_settable<float> *tempSensorCurrent() const;
    cuDeviceParam<float> *tempSensorVoltage() const;
    cuDeviceParam<float> *pressSensorVoltageP() const;
    cuDeviceParam<float> *pressSensorVoltageN() const;
    cuDeviceParam_settable<bool> *commutatorOn() const;

    cuDeviceParam_settable<CU4TDM0V1_EEPROM_Const_t> *eepromConst() const;
    cuDeviceParam_settable<pair_t<float> > *tempSensorCurrentAdcCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *tempSensorCurrentDacCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *tempSensorVoltageCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *pressSensorVoltagePCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *pressSensorVoltageNCoeff() const;
    cuDeviceParam_settable<pair_t<float> > *pressSensorCoeff() const;

signals:
    void dataUpdated(CU4TDM0V1_Data_t);
    void eepromConstUpdated(CU4TDM0V1_EEPROM_Const_t);

    void valueSetted();
    void eepromConstSetted();

protected:
    bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* deviceData);
private:
    cuDeviceParam<CU4TDM0V1_Data_t> *mDeviceData;
    // Devicce data
    cuDeviceParam<float> *mTemperature;
    cuDeviceParam<float> *mPressure;
    cuDeviceParam_settable<float> *mTempSensorCurrent;
    cuDeviceParam<float> *mTempSensorVoltage;
    cuDeviceParam<float> *mPressSensorVoltageP;
    cuDeviceParam<float> *mPressSensorVoltageN;
    cuDeviceParam_settable<bool>  *mCommutatorOn;

    //Device EEPROM
    cuDeviceParam_settable<CU4TDM0V1_EEPROM_Const_t> *mEepromConst;
    cuDeviceParam_settable<pair_t<float> > *mTempSensorCurrentAdcCoeff;
    cuDeviceParam_settable<pair_t<float> > *mTempSensorCurrentDacCoeff;
    cuDeviceParam_settable<pair_t<float> > *mTempSensorVoltageCoeff;
    cuDeviceParam_settable<pair_t<float> > *mPressSensorVoltagePCoeff;
    cuDeviceParam_settable<pair_t<float> > *mPressSensorVoltageNCoeff;
    cuDeviceParam_settable<pair_t<float> > *mPressSensorCoeff;

    CU4TDM0V1_Temp_Table_Item_t mTempTable[TEMP_TABLE_SIZE];
    quint8 offset, count;
};

#endif // CCU4TDM0DRIVER_H
