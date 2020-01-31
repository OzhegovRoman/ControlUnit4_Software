#include "sspddriverm0.h"
#include "star_prc_commands.h"

SspdDriverM0::SspdDriverM0(QObject *parent)
    : CommonDriver(parent)
    , mDeviceData(new DriverPropertyReadOnly<CU4SDM0V1_Data_t>(this, cmd::SD_GetData))
{
//    connect(mDeviceData, &DriverPropertyReadOnly<CU4SDM0V1_Data_t>::propertyGetted, [=]()
//    {

//    });
}

DriverPropertyReadOnly<CU4SDM0V1_Data_t> *SspdDriverM0::data() const
{
    return mDeviceData;
}
