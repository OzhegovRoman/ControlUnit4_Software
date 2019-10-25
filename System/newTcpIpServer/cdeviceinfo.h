#ifndef CDEVICEINFO_H
#define CDEVICEINFO_H

#include <QObject>
#include "cudid.h"

class cDeviceInfo
{
public:
    cDeviceInfo();

    quint8 address() const;
    void setAddress(const quint8 &address);

    QString type() const;
    void setType(const QString &type);

    cUDID UDID() const;
    void setUDID(const cUDID &UDID);

    QString modificationVersion() const;
    void setModificationVersion(const QString &modificationVersion);

    QString hardwareVersion() const;
    void setHardwareVersion(const QString &hardwareVersion);

    QString firmwareVersion() const;
    void setFirmwareVersion(const QString &firmwareVersion);

    QString description() const;
    void setDescription(const QString &description);

private:
    quint8 mAddress;
    QString mType;
    cUDID mUDID;
    QString mModificationVersion;
    QString mHardwareVersion;
    QString mFirmwareVersion;
    QString mDescription;
    bool mActive;
};

#endif // CDEVICEINFO_H
