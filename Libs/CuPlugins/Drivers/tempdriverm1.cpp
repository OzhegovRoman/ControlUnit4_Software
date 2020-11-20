#include "tempdriverm1.h"
#include "star_prc_commands.h"
#include <QDebug>

TempDriverM1::TempDriverM1(QObject *parent)
    : CommonDriver(parent)
    , mRelaysStatus (new DriverProperty<cRelaysStatus>(this, cmd::TDM1_GetRelaysStatus, cmd::TDM1_SetRelaysStatus))
    , mSwitcherMode (new DriverProperty<cmd::enum_CU4TDM1_SwitcherMode>(this, cmd::TDM1_GetSwitcherMode, cmd::TDM1_SetSwitcherMode))
    , mVoltageProperty (new DriverProperty_p(this, cmd::TDM1_GetTempSensorVoltage))
    , mCurrentProperty (new DriverProperty_p(this, cmd::TDM1_SetTempSensorCurrent, cmd::TDM1_SetTempSensorCurrent))
    , mTemperatureProperty (new DriverProperty_p(this, cmd::TDM1_GetTemperature))
    , mDefaultParams(new DriverProperty_p(this, cmd::TDM1_GetDefaultParams, cmd::TDM1_SetDefaultParams))
    , mEepromCoeffs(new DriverProperty_p(this, cmd::TDM1_GetEepromConst, cmd::TDM1_SetEepromConst))
    , mTempTableProperty(new DriverProperty_p(this, cmd::TDM1_GetTempTableValues, cmd::TDM1_SetTempTableValues))
{

}

TempDriverM1::~TempDriverM1()
{

}

DriverProperty<cRelaysStatus> *TempDriverM1::relaysStatus() const
{
    return mRelaysStatus;
}

DriverProperty<cmd::enum_CU4TDM1_SwitcherMode> *TempDriverM1::switcherMode() const
{
    return mSwitcherMode;
}

bool TempDriverM1::updateVoltage()
{
    QByteArray ba;
    ba.append(0xFF);
    bool ok;
    mVoltageProperty->readWriteSync(mVoltageProperty->cmdGetter(), ba, &ok);
    if (ok){
        if (sizeof(float) * 4 != mVoltageProperty->data().size())
            return false;
        auto* tmpData = reinterpret_cast<float*>(mVoltageProperty->data().data());
        for (int i=0; i<4; ++i)
            mVoltages[i] = tmpData[i];
    }
    return ok;
}

bool TempDriverM1::updateVoltage(uint8_t channel)
{
    if (channel >= 4) return false;
    QByteArray ba;
    ba.append(channel);
    bool ok;
    mVoltageProperty->readWriteSync(mVoltageProperty->cmdGetter(), ba, &ok);
    if (ok){
        if (sizeof(float) != mVoltageProperty->data().size())
            return false;
        mVoltages[channel] = *(reinterpret_cast<float*>(mVoltageProperty->data().data()));
    }
    return ok;
}

float TempDriverM1::currentVoltage(uint8_t index)
{
    if (index<4)
        return (mVoltages[index]);
    return 0;
}

bool TempDriverM1::setCurrent(uint8_t channel, float current)
{
    bool ok = false;
    if (channel<4 || channel == 0xff){
        QByteArray ba;
        ba.append(channel);
        ba.append(reinterpret_cast<char*>(&current), sizeof(float));
        mCurrentProperty->readWriteSync(mCurrentProperty->cmdSetter(), ba, &ok);
    }
    return ok;
}

bool TempDriverM1::setCurrent(float current)
{
    return setCurrent(0xff, current);
}

bool TempDriverM1::updateTemperature()
{
    QByteArray ba;
    ba.append(0xFF);
    bool ok;
    mTemperatureProperty->readWriteSync(mTemperatureProperty->cmdGetter(), ba, &ok);
    if (ok){
        if (sizeof(float) * 4 != mTemperatureProperty->data().size())
            return false;
        auto* tmpData = reinterpret_cast<float*>(mTemperatureProperty->data().data());
        for (int i=0; i<4; ++i)
            mTemperatures[i] = tmpData[i];
    }
    return ok;
}

bool TempDriverM1::updateTemperature(uint8_t channel)
{
    if (channel >= 4) return false;
    QByteArray ba;
    ba.append(channel);
    bool ok;
    mTemperatureProperty->readWriteSync(mTemperatureProperty->cmdGetter(), ba, &ok);
    if (ok){
        if (sizeof(float) != mTemperatureProperty->data().size())
            return false;
        mTemperatures[channel] = *(reinterpret_cast<float*>(mTemperatureProperty->data().data()));
    }
    return ok;
}

float TempDriverM1::currentTemperature(uint8_t index)
{
    if (index<4)
        return (mTemperatures[index]);
    return 0.;
}

bool TempDriverM1::readDefaultParams()
{
    bool ok = false;
    QByteArray ba = mDefaultParams->readWriteSync(mDefaultParams->cmdGetter(), QByteArray(), &ok, 5);
    const int paramsize = sizeof(bool)+ sizeof(float);
    if (ok){
        if (ba.size() != 4 * paramsize)
            return false;
        for (int i = 0; i< 4; ++i){
            m_DefaultParams[i].enable = static_cast<bool>(ba.data()[i*paramsize]);
            m_DefaultParams[i].current = *(reinterpret_cast<float*>(ba.data() + i*paramsize+sizeof(bool)));
        }
    }
    return ok;
}

DefaultParam TempDriverM1::defaultParam(uint8_t index)
{
    if (index < 4) return m_DefaultParams[index];
    return DefaultParam{false, 0};
}

void TempDriverM1::setDefaultParam(int index, DefaultParam param)
{
    if (index < 4) m_DefaultParams[index]=param;
}

bool TempDriverM1::writeDefaultParams()
{
    bool ok = false;
    QByteArray ba;
    for (int i = 0; i< 4; ++i){
        ba.append(m_DefaultParams[i].enable);
        ba.append(reinterpret_cast<char*>(&m_DefaultParams[i].current), sizeof(float));
    }

    mDefaultParams->readWriteSync(mDefaultParams->cmdSetter(), ba, &ok, 5);
    return ok;
}

bool TempDriverM1::readEepromCoeffs()
{
    bool ok = false;
    QByteArray ba = mEepromCoeffs->readWriteSync(mEepromCoeffs->cmdGetter(), QByteArray(), &ok, 5);
    if (ok){
        if (ba.size() != 8 * sizeof(pair_t<float>))
            return false;
        auto * p = reinterpret_cast<pair_t<float>*>(ba.data());
        for (int i = 0; i< 4; ++i){
            m_EepromCoeffs[i].current = p[2 * i];
            m_EepromCoeffs[i].voltage = p[2 * i +1];
        }
    }
    return ok;
}

bool TempDriverM1::writeEepromCoeffs()
{
    bool ok = false;
    QByteArray ba;
    for (int i = 0; i< 4; ++i){
        ba.append(reinterpret_cast<char*>(&m_EepromCoeffs[i].current), sizeof(pair_t<float>));
        ba.append(reinterpret_cast<char*>(&m_EepromCoeffs[i].voltage), sizeof(pair_t<float>));
    }
    mEepromCoeffs->readWriteSync(mEepromCoeffs->cmdSetter(), ba, &ok, 5);
    return ok;
}

TDM1_EepromCoeff TempDriverM1::eepromCoeff(uint8_t index) const
{
    if (index<4) return m_EepromCoeffs[index];
    return TDM1_EepromCoeff();
}

void TempDriverM1::setEepromCoeff(uint8_t index, TDM1_EepromCoeff coeff)
{
    if (index<4) m_EepromCoeffs[index] = coeff;
}

bool TempDriverM1::receiveTempTables()
{
    QByteArray buffer;
    for (char channel = 0; channel < 4; ++channel){
        char offset = 0;
        while (offset < TEMP_TABLE_SIZE) {

            int count = qMin(10, TEMP_TABLE_SIZE - offset);

            buffer.clear();
            buffer.append(channel);
            buffer.append(offset);
            buffer.append(static_cast<char>(count));
            bool ok = true;
            mTempTableProperty->readWriteSync(mTempTableProperty->cmdGetter(), buffer, &ok, 10);
            if (!ok) return false;

            auto* tmpData = reinterpret_cast<CU4TDM0V1_Temp_Table_Item_t*>(mTempTableProperty->data().data());
            for(int i = 0; i < count; i++){
                mTempTables[static_cast<int>(channel)][offset+i] = tmpData[i];
            }
            offset += count;
        }
    }
    return true;
}

bool TempDriverM1::sendTempTables()
{
    QByteArray buffer;
    for (char channel = 0; channel < 4; ++channel){
        char offset = 0;
        while (offset < TEMP_TABLE_SIZE) {

            int count = qMin(10, TEMP_TABLE_SIZE - offset);

            buffer.clear();
            buffer.append(channel);
            buffer.append(offset);
            buffer.append(static_cast<char>(count));

            for (int i = 0; i < count; ++i){
                buffer.append(reinterpret_cast<char*>(&mTempTables[static_cast<int>(channel)][offset+i]), sizeof(CU4TDM0V1_Temp_Table_Item_t));
            }

            bool ok = true;
            mTempTableProperty->readWriteSync(mTempTableProperty->cmdSetter(), buffer, &ok, 5);
            if (!ok) return false;

            offset += count;
        }
    }
    return true;
}

CU4TDM0V1_Temp_Table_Item_t TempDriverM1::tempTableItem(uint8_t channel, uint8_t index)
{
    if ((channel>3) || (index > 99))
        return CU4TDM0V1_Temp_Table_Item_t{0,0};
    return mTempTables[channel][index];
}

CU4TDM0V1_Temp_Table_Item_t *TempDriverM1::tempTable(uint8_t channel)
{
    if (channel>3) return nullptr;
    return mTempTables[channel];
}

void TempDriverM1::setTempTableItem(uint8_t channel, uint8_t index, CU4TDM0V1_Temp_Table_Item_t item)
{
    if ((channel>3) || (index > 99))
        return;
    mTempTables[channel][index] = item;
}

void TempDriverM1::setTempTable( uint8_t channel, CU4TDM0V1_Temp_Table_Item_t *table)
{
    for (int i = 0; i< TEMP_TABLE_SIZE; ++i)
        setTempTableItem(channel, i, table[i]);
}
