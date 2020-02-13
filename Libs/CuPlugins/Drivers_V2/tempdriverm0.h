#ifndef TEMPDRIVERM0_H
#define TEMPDRIVERM0_H

#include <QObject>
#include "commondriver.h"
#include "driverproperty.h"
#include "star_prc_structs.h"

class TempDriverM0 : public CommonDriver
{
    Q_OBJECT
public:
    explicit TempDriverM0(QObject *parent = nullptr);
    ~TempDriverM0();

    //DeviceData
    DriverPropertyReadOnly<CU4TDM0V1_Data_t> *deviceData() const;

    DriverPropertyReadOnly<float> *temperature() const;
    DriverPropertyReadOnly<float> *pressure() const;
    DriverProperty<float> *tempSensorCurrent() const;
    DriverPropertyReadOnly<float> *tempSensorVoltage() const;
    DriverPropertyReadOnly<float> *pressSensorVoltageP() const;
    DriverPropertyReadOnly<float> *pressSensorVoltageN() const;
    DriverProperty<bool> *commutator() const;

    //EEPROM Const
    DriverProperty<CU4TDM0V1_EEPROM_Const_t> *eepromConst() const;

    DriverProperty<pair_t<float> > *tempSensorCurrentAdcCoeff() const;
    DriverProperty<pair_t<float> > *tempSensorCurrentDacCoeff() const;
    DriverProperty<pair_t<float> > *tempSensorVoltageCoeff() const;
    DriverProperty<pair_t<float> > *pressSensorVoltagePCoeff() const;
    DriverProperty<pair_t<float> > *pressSensorVoltageNCoeff() const;
    DriverProperty<pair_t<float> > *pressSensorCoeff() const;

    //TempTable;
     CU4TDM0V1_Temp_Table_Item_t* tempTable();
     bool sendTempTable();
     bool receiveTempTable();

signals:
    void dataUpdated(CU4TDM0V1_Data_t);
    void eepromConstUpdated(CU4TDM0V1_EEPROM_Const_t);

private slots:
    void updateData(CU4TDM0V1_Data_t data);
    void updateEEPROM(CU4TDM0V1_EEPROM_Const_t eeprom);

private:
    //DeviceData
    DriverPropertyReadOnly<CU4TDM0V1_Data_t> *mDeviceData;

    DriverPropertyReadOnly<float> *mTemperature;
    DriverPropertyReadOnly<float> *mPressure;
    DriverProperty<float> *mTempSensorCurrent;
    DriverPropertyReadOnly<float> *mTempSensorVoltage;
    DriverPropertyReadOnly<float> *mPressSensorVoltageP;
    DriverPropertyReadOnly<float> *mPressSensorVoltageN;
    DriverProperty<bool>  *mCommutatorOn;

    //Device EEPROM
    DriverProperty<CU4TDM0V1_EEPROM_Const_t> *mEepromConst;

    DriverProperty<pair_t<float> > *mTempSensorCurrentAdcCoeff;
    DriverProperty<pair_t<float> > *mTempSensorCurrentDacCoeff;
    DriverProperty<pair_t<float> > *mTempSensorVoltageCoeff;
    DriverProperty<pair_t<float> > *mPressSensorVoltagePCoeff;
    DriverProperty<pair_t<float> > *mPressSensorVoltageNCoeff;
    DriverProperty<pair_t<float> > *mPressSensorCoeff;

    CU4TDM0V1_Temp_Table_Item_t mTempTable[TEMP_TABLE_SIZE];
    // вспомогательное свойство для доступа к таблице температур
    DriverProperty_p *mTempTableProperty;

};

#endif // TEMPDRIVERM0_H
