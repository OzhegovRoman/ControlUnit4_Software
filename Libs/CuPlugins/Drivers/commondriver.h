#ifndef COMMONDRIVER_H
#define COMMONDRIVER_H

#include <QObject>
#include "abstractdriver.h"
#include "driverproperty.h"

class CommonDriver: public AbstractDriver
{
    Q_OBJECT
public:
    explicit CommonDriver(QObject * parent = nullptr);
    ~CommonDriver();

    DriverPropertyReadOnly<QString> * deviceType() const;
    DriverPropertyReadOnly<QString> * modificationVersion() const;
    DriverPropertyReadOnly<QString> * hardwareVersion() const;
    DriverPropertyReadOnly<QString> * firmwareVersion() const;
    DriverPropertyReadOnly<QString> * deviceDescription() const;
    DriverPropertyReadOnly<cUDID>   * UDID() const;

    DriverCommand                   * init() const;
    DriverCommand                   * shutDown() const;
    DriverCommand                   * silence() const;
    DriverCommand                   * reboot() const;
    DriverCommand                   * writeEeprom() const;

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
