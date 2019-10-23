#include "cdeviceinfo.h"

quint8 cDeviceInfo::address() const
{
    return mAddress;
}

void cDeviceInfo::setAddress(const quint8 &address)
{
    mAddress = address;
}

QString cDeviceInfo::type() const
{
    return mType;
}

void cDeviceInfo::setType(const QString &type)
{
    mType = type;
}

cUDID cDeviceInfo::UDID() const
{
    return mUDID;
}

void cDeviceInfo::setUDID(const cUDID &uDID)
{
    mUDID = uDID;
}

QString cDeviceInfo::modificationVersion() const
{
    return mModificationVersion;
}

void cDeviceInfo::setModificationVersion(const QString &modificationVersion)
{
    mModificationVersion = modificationVersion;
}

QString cDeviceInfo::hardwareVersion() const
{
    return mHardwareVersion;
}

void cDeviceInfo::setHardwareVersion(const QString &hardwareVersion)
{
    mHardwareVersion = hardwareVersion;
}

QString cDeviceInfo::firmwareVersion() const
{
    return mFirmwareVersion;
}

void cDeviceInfo::setFirmwareVersion(const QString &firmwareVersion)
{
    mFirmwareVersion = firmwareVersion;
}

QString cDeviceInfo::description() const
{
    return mDescription;
}

void cDeviceInfo::setDescription(const QString &description)
{
    mDescription = description;
}
