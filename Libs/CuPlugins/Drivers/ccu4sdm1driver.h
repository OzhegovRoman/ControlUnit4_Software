#ifndef CCU4SDM1DRIVER_H
#define CCU4SDM1DRIVER_H

#include <QObject>
#include "adriver.h"
#include "../StarProtocol/star_prc_structs.h"
#include "ccu4sdm0driver.h"

// TODO: неправильно это все, надо пересобирать
class cCu4SdM1Driver : public cCu4SdM0Driver
{
    Q_OBJECT
    using cCu4SdM0Driver::deviceData; // прячем от гре
    using cCu4SdM0Driver::eepromConst;

public:
    explicit cCu4SdM1Driver(QObject *parent = nullptr);

    // cuIODeviceImpl interface
    cuDeviceParam<CU4SDM1_Data_t> *deviceData() const;
    cuDeviceParam<float> *currentMonitor() const;

    cuDeviceParam_settable<CU4SDM1_EEPROM_Const_t> *eepromConst() const;
    cuDeviceParam_settable<pair_t<float> > *currentMonitorAdcCoeff() const;

protected:
    virtual bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data) override;
signals:
    void dataUpdated(CU4SDM1_Data_t);
    void eepromConstUpdated(CU4SDM1_EEPROM_Const_t);

private:
    //DeviceData
    cuDeviceParam<CU4SDM1_Data_t> *mDeviceData;
    cuDeviceParam<float> *mCurrentMonitor;

    //Device EEPROM
    cuDeviceParam_settable<CU4SDM1_EEPROM_Const_t> *mEepromConst;
    cuDeviceParam_settable<pair_t<float> > *mCurrentMonitorAdcCoeff;
};

#endif // CCU4SDM1DRIVER_H
