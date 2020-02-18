#include "commondriver.h"
#include "star_prc_commands.h"

CommonDriver::CommonDriver(QObject *parent)
    : AbstractDriver_V2(parent)
    , mDeviceType(new DriverPropertyReadOnly<QString>(this, cmd::G_GetDeviceType))
    , mModificationVersion(new DriverPropertyReadOnly<QString> (this, cmd::G_GetModVersion))
    , mHardwareVersion(new DriverPropertyReadOnly<QString> (this, cmd::G_GetHwVersion))
    , mFirmwareVersion(new DriverPropertyReadOnly<QString> (this, cmd::G_GetFwVersion))
    , mDeviceDescription(new DriverPropertyReadOnly<QString> (this, cmd::G_GetDeviceDescription))
    , mUDID(new DriverPropertyReadOnly<cUDID> (this, cmd::G_GetDeviceId))
    , mInit(new DriverCommand (this, cmd::G_Init))
    , mShutDown(new DriverCommand (this, cmd::G_GoToPark))
    , mSilence(new DriverCommand (this, cmd::G_ListeningOff))
    , mReboot(new DriverCommand (this, cmd::G_Reboot))
    , mWriteEeprom(new DriverCommand (this, cmd::G_WriteEeprom))
{
    mWriteEeprom->executedSignal()->connect([=](){emit eepromWrited();});
}

DriverPropertyReadOnly<QString> *CommonDriver::deviceType() const
{
    return mDeviceType;
}

DriverPropertyReadOnly<QString> *CommonDriver::modificationVersion() const
{
    return mModificationVersion;
}

DriverPropertyReadOnly<QString> *CommonDriver::hardwareVersion() const
{
    return mHardwareVersion;
}

DriverPropertyReadOnly<QString> *CommonDriver::firmwareVersion() const
{
    return mFirmwareVersion;
}

DriverPropertyReadOnly<QString> *CommonDriver::deviceDescription() const
{
    return mDeviceDescription;
}

DriverPropertyReadOnly<cUDID> *CommonDriver::UDID() const
{
    return mUDID;
}

DriverCommand *CommonDriver::init() const
{
    return mInit;
}

DriverCommand *CommonDriver::shutDown() const
{
    return mShutDown;
}

DriverCommand *CommonDriver::silence() const
{
    return mSilence;
}

DriverCommand *CommonDriver::reboot() const
{
    return mReboot;
}

DriverCommand *CommonDriver::writeEeprom() const
{
    return mWriteEeprom;
}

bool CommonDriver::getDeviceInfo()
{
    bool ok;
    mDeviceType->getValueSync(&ok);
    if (!ok) return false;
    mModificationVersion->getValueSync(&ok);
    if (!ok) return false;
    mHardwareVersion->getValueSync(&ok);
    if (!ok) return false;
    mFirmwareVersion->getValueSync(&ok);
    if (!ok) return false;
    mDeviceDescription->getValueSync(&ok);
    if (!ok) return false;
    mUDID->getValueSync(&ok);
    return ok;
}
