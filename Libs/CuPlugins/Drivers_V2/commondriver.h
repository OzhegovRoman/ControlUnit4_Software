#ifndef COMMONDRIVER_H
#define COMMONDRIVER_H

#include <QObject>
#include "abstractdriver_v2.h"
#include "driverproperty.h"

class CommonDriver: public AbstractDriver_V2
{
    Q_OBJECT
public:
    explicit CommonDriver(QObject * parent = nullptr);

    DriverPropertyReadOnly<QString> *deviceType() const;
    DriverPropertyReadOnly<QString> *modificationVersion() const;
    DriverPropertyReadOnly<QString> *hardwareVersion() const;
    DriverPropertyReadOnly<QString> *firmwareVersion() const;
    DriverPropertyReadOnly<QString> *deviceDescription() const;
    DriverPropertyReadOnly<cUDID> *UDID() const;

    DriverCommand *init() const;
    DriverCommand *shutDown() const;
    DriverCommand *silence() const;
    DriverCommand *reboot() const;
    DriverCommand *writeEeprom() const;

    bool getDeviceInfo();

signals:
    void eepromWrited();

private:
    DriverPropertyReadOnly<QString> *mDeviceType;
    DriverPropertyReadOnly<QString> *mModificationVersion;
    DriverPropertyReadOnly<QString> *mHardwareVersion;
    DriverPropertyReadOnly<QString> *mFirmwareVersion;
    DriverPropertyReadOnly<QString> *mDeviceDescription;
    DriverPropertyReadOnly<cUDID> *mUDID;

    DriverCommand *mInit;
    DriverCommand *mShutDown;
    DriverCommand *mSilence;
    DriverCommand *mReboot;
    DriverCommand *mWriteEeprom;
};

#endif // COMMONDRIVER_H
