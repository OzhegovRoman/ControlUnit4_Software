#include "tempdriverm0.h"
#include "star_prc_commands.h"

TempDriverM0::TempDriverM0(QObject *parent)
    : CommonDriver(parent)

    //DeviceData
    , mDeviceData (new DriverPropertyReadOnly<CU4TDM0V1_Data_t>(this, cmd::TD_GetData))
    //
    , mTemperature (new DriverPropertyReadOnly<float>(this, cmd::TD_GetTemperature))
    , mPressure (new DriverPropertyReadOnly<float>(this, cmd::TD_GetPressure))
    , mTempSensorCurrent (new DriverProperty<float>(this, cmd::TD_GetTempSensorCurrent, cmd::TD_SetTempSensorCurrent))
    , mTempSensorVoltage (new DriverPropertyReadOnly<float>(this, cmd::TD_GetTempSensorVoltage))
    , mPressSensorVoltageP (new DriverPropertyReadOnly<float>(this, cmd::TD_GetPressSensorVoltageP))
    , mPressSensorVoltageN (new DriverPropertyReadOnly<float>(this, cmd::TD_GetPressSensorVoltageN))
    , mCommutatorOn (new DriverProperty<bool>(this, cmd::TD_GetTurnOnStatus, cmd::TD_SetTurnOnStatus))

    //Device EEPROM
    , mEepromConst (new DriverProperty<CU4TDM0V1_EEPROM_Const_t> (this, cmd::TD_GetEepromConst, cmd::TD_SetEepromConst))
    //
    , mTempSensorCurrentAdcCoeff (new DriverProperty<pair_t<float> >(this, cmd::TD_GetTempSensorCurrentAdcCoeff, cmd::TD_SetTempSensorCurrentAdcCoeff))
    , mTempSensorCurrentDacCoeff (new DriverProperty<pair_t<float> >(this, cmd::TD_GetTempSensorCurrentDacCoeff, cmd::TD_SetTempSensorCurrentDacCoeff))
    , mTempSensorVoltageCoeff (new DriverProperty<pair_t<float> >(this, cmd::TD_GetTempSensorVoltageCoeff, cmd::TD_SetTempSensorVoltageCoeff))
    , mPressSensorVoltagePCoeff (new DriverProperty<pair_t<float> >(this, cmd::TD_GetPressSensorVoltagePCoeff, cmd::TD_SetPressSensorVoltagePCoeff))
    , mPressSensorVoltageNCoeff (new DriverProperty<pair_t<float> >(this, cmd::TD_GetPressSensorVoltageNCoeff, cmd::TD_SetPressSensorVoltageNCoeff))
    , mPressSensorCoeff (new DriverProperty<pair_t<float> >(this, cmd::TD_GetPressSensorCoeff, cmd::TD_SetPressSensorCoeff))

    , mTempTableProperty(new DriverProperty_p (this, cmd::TD_GetTempTableValues, cmd::TD_SetTempTableValues ))

{
    mDeviceData->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        mTemperature->setCurrentValue(data.Temperature);
        mPressure->setCurrentValue(data.Pressure);
        mTempSensorCurrent->setCurrentValue(data.TempSensorCurrent);
        mTempSensorVoltage->setCurrentValue(data.TempSensorVoltage);
        mPressSensorVoltageP->setCurrentValue(data.PressSensorVoltageP);
        mPressSensorVoltageN->setCurrentValue(data.PressSensorVoltageN);
        mCommutatorOn->setCurrentValue(data.CommutatorOn);
        updateData(data);
    });

    mTemperature->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.Temperature = mTemperature->currentValue();
        updateData(data);
    });
    mPressure->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.Pressure = mPressure->currentValue();
        updateData(data);
    });
    mTempSensorCurrent->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.TempSensorCurrent = mTempSensorCurrent->currentValue();
        updateData(data);
    });
    mTempSensorVoltage->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.TempSensorVoltage = mTempSensorVoltage->currentValue();
        updateData(data);
    });
    mPressSensorVoltageP->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.PressSensorVoltageP = mPressSensorVoltageP->currentValue();
        updateData(data);
    });
    mPressSensorVoltageN->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.PressSensorVoltageN = mPressSensorVoltageN->currentValue();
        updateData(data);
    });
    mCommutatorOn->gettedSignal()->connect([=](){
        auto data = mDeviceData->currentValue();
        data.CommutatorOn = mCommutatorOn->currentValue();
        updateData(data);
    });

    //EEPROM Const
    mEepromConst->gettedSignal()->connect([=](){
        auto eeprom = mEepromConst->currentValue();
        mTempSensorCurrentAdcCoeff->setCurrentValue(eeprom.tempSensorCurrentAdc);
        mTempSensorCurrentDacCoeff->setCurrentValue(eeprom.tempSensorCurrentDac);
        mTempSensorVoltageCoeff->setCurrentValue(eeprom.tempSensorVoltage);
        mPressSensorVoltagePCoeff->setCurrentValue(eeprom.pressSensorVoltageP);
        mPressSensorVoltageNCoeff->setCurrentValue(eeprom.pressSensorVoltageN);
        mPressSensorCoeff->setCurrentValue(eeprom.pressSensorCoeffs);
        updateEEPROM(eeprom);
    });

    mTempSensorCurrentAdcCoeff->gettedSignal()->connect([=](){
       auto eeprom = mEepromConst->currentValue();
       eeprom.tempSensorCurrentAdc = mTempSensorCurrentAdcCoeff->currentValue();
       updateEEPROM(eeprom);
    });
    mTempSensorCurrentDacCoeff->gettedSignal()->connect([=](){
       auto eeprom = mEepromConst->currentValue();
       eeprom.tempSensorCurrentDac = mTempSensorCurrentDacCoeff->currentValue();
       updateEEPROM(eeprom);
    });
    mTempSensorVoltageCoeff->gettedSignal()->connect([=](){
       auto eeprom = mEepromConst->currentValue();
       eeprom.tempSensorVoltage = mTempSensorVoltageCoeff->currentValue();
       updateEEPROM(eeprom);
    });
    mPressSensorVoltagePCoeff->gettedSignal()->connect([=](){
       auto eeprom = mEepromConst->currentValue();
       eeprom.pressSensorVoltageP = mPressSensorVoltagePCoeff->currentValue();
       updateEEPROM(eeprom);
    });
    mPressSensorVoltageNCoeff->gettedSignal()->connect([=](){
       auto eeprom = mEepromConst->currentValue();
       eeprom.pressSensorVoltageN = mPressSensorVoltageNCoeff->currentValue();
       updateEEPROM(eeprom);
    });
    mPressSensorCoeff->gettedSignal()->connect([=](){
       auto eeprom = mEepromConst->currentValue();
       eeprom.pressSensorCoeffs = mPressSensorCoeff->currentValue();
       updateEEPROM(eeprom);
    });
}

TempDriverM0::~TempDriverM0()
{
    delete mDeviceData;
    delete mTemperature;
    delete mPressure;
    delete mTempSensorCurrent;
    delete mTempSensorVoltage;
    delete mPressSensorVoltageP;
    delete mPressSensorVoltageN;
    delete mCommutatorOn;
    delete mEepromConst;
    delete mTempSensorCurrentAdcCoeff;
    delete mTempSensorCurrentDacCoeff;
    delete mTempSensorVoltageCoeff;
    delete mPressSensorVoltagePCoeff;
    delete mPressSensorVoltageNCoeff;
    delete mPressSensorCoeff;
    delete mTempTableProperty;
}

DriverPropertyReadOnly<CU4TDM0V1_Data_t> *TempDriverM0::data() const
{
    return mDeviceData;
}

void TempDriverM0::updateData(CU4TDM0V1_Data_t data)
{
    mDeviceData->setCurrentValue(data);
    emit dataUpdated(data);
}

void TempDriverM0::updateEEPROM(CU4TDM0V1_EEPROM_Const_t eeprom)
{
    mEepromConst->setCurrentValue(eeprom);
    emit eepromConstUpdated(eeprom);
}

DriverProperty<pair_t<float> > *TempDriverM0::pressSensorCoeff() const
{
    return mPressSensorCoeff;
}

CU4TDM0V1_Temp_Table_Item_t *TempDriverM0::tempTable()
{
    return reinterpret_cast<CU4TDM0V1_Temp_Table_Item_t*>(&mTempTable);
}

bool TempDriverM0::sendTempTable()
{
    // TODO: требуется провыерка данной функции
    QByteArray buffer;
    char offset = 0;
    while (offset < TEMP_TABLE_SIZE) {

        int count = qMin(10, TEMP_TABLE_SIZE - offset);

        buffer.clear();
        buffer.append(offset);
        buffer.append(static_cast<char>(count));
        for (int i = 0; i < count; i++)
            buffer.append(reinterpret_cast<char*>(&mTempTable[offset+i]), sizeof(CU4TDM0V1_Temp_Table_Item_t));

        bool ok = true;
        mTempTableProperty->readWriteSync(mTempTableProperty->cmdSetter(), buffer, &ok, 5);

        if (!ok) return false;
        offset += count;
    }
    return true;
}

bool TempDriverM0::receiveTempTable()
{
    // TODO: требуетс провыерка данной функции
    QByteArray buffer;
    char offset = 0;
    while (offset < TEMP_TABLE_SIZE) {

        int count = qMin(10, TEMP_TABLE_SIZE - offset);

        buffer.clear();
        buffer.append(offset);
        buffer.append(static_cast<char>(count));
        bool ok = true;
        mTempTableProperty->readWriteSync(mTempTableProperty->cmdGetter(), buffer, &ok, 5);
        if (!ok) return false;

        auto* tmpData = reinterpret_cast<CU4TDM0V1_Temp_Table_Item_t*>(mTempTableProperty->data().data());
        for(int i = 0; i < count; i++){
            mTempTable[offset+i] = tmpData[i];
        }

        offset += count;
    }
    return true;
}

DriverProperty<pair_t<float> > *TempDriverM0::pressSensorVoltageNCoeff() const
{
    return mPressSensorVoltageNCoeff;
}

DriverProperty<pair_t<float> > *TempDriverM0::pressSensorVoltagePCoeff() const
{
    return mPressSensorVoltagePCoeff;
}

DriverProperty<pair_t<float> > *TempDriverM0::tempSensorVoltageCoeff() const
{
    return mTempSensorVoltageCoeff;
}

DriverProperty<pair_t<float> > *TempDriverM0::tempSensorCurrentDacCoeff() const
{
    return mTempSensorCurrentDacCoeff;
}

DriverProperty<pair_t<float> > *TempDriverM0::tempSensorCurrentAdcCoeff() const
{
    return mTempSensorCurrentAdcCoeff;
}

DriverProperty<CU4TDM0V1_EEPROM_Const_t> *TempDriverM0::eepromConst() const
{
    return mEepromConst;
}

DriverProperty<bool> *TempDriverM0::commutator() const
{
    return mCommutatorOn;
}

DriverPropertyReadOnly<float> *TempDriverM0::pressSensorVoltageN() const
{
    return mPressSensorVoltageN;
}

DriverPropertyReadOnly<float> *TempDriverM0::pressSensorVoltageP() const
{
    return mPressSensorVoltageP;
}

DriverPropertyReadOnly<float> *TempDriverM0::tempSensorVoltage() const
{
    return mTempSensorVoltage;
}

DriverProperty<float> *TempDriverM0::tempSensorCurrent() const
{
    return mTempSensorCurrent;
}

DriverPropertyReadOnly<float> *TempDriverM0::pressure() const
{
    return mPressure;
}

DriverPropertyReadOnly<float> *TempDriverM0::temperature() const
{
    return mTemperature;
}
