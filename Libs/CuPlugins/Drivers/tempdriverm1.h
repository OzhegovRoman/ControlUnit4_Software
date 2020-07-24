#ifndef TEMPDRIVERM1_H
#define TEMPDRIVERM1_H

#include <QObject>
#include "commondriver.h"
#include "driverproperty.h"
#include "../StarProtocol/star_prc_structs.h"
#include "../StarProtocol/star_prc_commands.h"

struct DefaultParam {
    bool enable;
    float current;
};

struct TDM1_EepromCoeff {
    pair_t<float> current;
    pair_t<float> voltage;
};

class TempDriverM1 : public CommonDriver
{
    Q_OBJECT
public:
    explicit TempDriverM1(QObject *parent = nullptr);
    ~TempDriverM1();

    DriverProperty<cRelaysStatus> *relaysStatus() const;
    DriverProperty<cmd::enum_CU4TDM1_SwitcherMode> *switcherMode() const;

    bool updateVoltage();
    bool updateVoltage(uint8_t channel);
    float currentVoltage(uint8_t index);

    bool setCurrent(uint8_t channel, float current);
    bool setCurrent(float current);

    bool updateTemperature();
    bool updateTemperature(uint8_t channel);
    float currentTemperature(uint8_t index);

    bool readDefaultParams();
    DefaultParam defaultParam(uint8_t index);
    void setDefaultParam(int index, DefaultParam param);
    bool writeDefaultParams();

    bool readEepromCoeffs();
    bool writeEepromCoeffs();

    TDM1_EepromCoeff eepromCoeff(uint8_t index) const;
    void setEepromCoeff(uint8_t index, TDM1_EepromCoeff coeff);

    bool receiveTempTables();
    bool sendTempTables();

    CU4TDM0V1_Temp_Table_Item_t tempTableItem(uint8_t channel, uint8_t index);
    CU4TDM0V1_Temp_Table_Item_t* tempTable(uint8_t channel);
    void setTempTableItem(uint8_t channel, uint8_t index, CU4TDM0V1_Temp_Table_Item_t item);
    void setTempTable(uint8_t channel, CU4TDM0V1_Temp_Table_Item_t* table);

private:
    DriverProperty<cRelaysStatus> *mRelaysStatus;
    DriverProperty<cmd::enum_CU4TDM1_SwitcherMode> *mSwitcherMode;

    float mVoltages[4] = {0.0};
    // вспомогательное свойство для доступа к таблице температур
    DriverProperty_p *mVoltageProperty;
    DriverProperty_p *mCurrentProperty;
    float mTemperatures[4] = {0.0};
    DriverProperty_p *mTemperatureProperty;

    DefaultParam m_DefaultParams[4];
    DriverProperty_p *mDefaultParams;

    DriverProperty_p *mEepromCoeffs;
    TDM1_EepromCoeff m_EepromCoeffs[4];

    DriverProperty_p *mTempTableProperty;
    CU4TDM0V1_Temp_Table_Item_t mTempTables[4][TEMP_TABLE_SIZE];
};

#endif // TEMPDRIVERM1_H
