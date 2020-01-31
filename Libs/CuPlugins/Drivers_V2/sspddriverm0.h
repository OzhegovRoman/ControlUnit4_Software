#ifndef SSPDDRIVERM0_H
#define SSPDDRIVERM0_H

#include <QObject>
#include "commondriver.h"
#include "driverproperty.h"
#include "star_prc_structs.h"

class SspdDriverM0 : public CommonDriver
{
    Q_OBJECT
public:
    explicit SspdDriverM0(QObject *parent = nullptr);

    DriverPropertyReadOnly<CU4SDM0V1_Data_t> *data() const;

signals:
    void dataUpdated(CU4SDM0V1_Data_t);
    void paramsUpdated(CU4SDM0V1_Param_t);
    void eepromConstUpdated(CU4SDM0V1_EEPROM_Const_t);
    void valueSetted();
    void paramsSetted();
    void eepromConstSetted();

private:
    //DeviceData
    DriverPropertyReadOnly<CU4SDM0V1_Data_t> *mDeviceData;
};

#endif // SSPDDRIVERM0_H
