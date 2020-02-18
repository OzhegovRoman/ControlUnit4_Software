#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "Drivers_V2/sspddriverm0.h"
#include "Drivers_V2/tempdriverm0.h"

class DeviceList;
class TemperatureData;
class SspdData;
class cuTcpSocketIOInterface;

class AppCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool reconnectEnable READ reconnectEnable)
    Q_PROPERTY(QString lastIpAddress READ lastIpAddress NOTIFY lastIpAddressChanged)
    Q_PROPERTY(DeviceList *devList READ devList WRITE setDevList)
    Q_PROPERTY(TemperatureData *mTempData READ getTempData WRITE setTempData)
    Q_PROPERTY(SspdData *mSspdData READ getSspdData WRITE setSspdData)
    Q_PROPERTY(int currentAddress WRITE setCurrentAddress)
    Q_PROPERTY(QStringList controlUnits READ controlUnits WRITE setControlUnits NOTIFY controlUnitsListChanged)

public:
    explicit AppCore(QObject *parent = nullptr);

    static bool reconnectEnable();
    static void setReconnectEnable(bool reconnectEnable);

    QString lastIpAddress() const;

    DeviceList *devList() const;
    void setDevList(DeviceList *devList);

    QStringList controlUnits() const;

    TemperatureData *getTempData() const;
    void setTempData(TemperatureData *tempData);

    SspdData *getSspdData() const;
    void setSspdData(SspdData *sspdData);

    int getCurrentAddress() const;
    void setCurrentAddress(int currentAddress);

    void setControlUnits(const QStringList &controlUnitsList);

signals:
    void reconnectEnableChanged();
    void lastIpAddressChanged();
    void connectionApply();
    void connectionReject();
    void controlUnitsListChanged();

public slots:
    Q_INVOKABLE void coreConnectToDefaultIpAddress();
    Q_INVOKABLE void coreConnectToIpAddress(const QString& ipAddress);
    Q_INVOKABLE void getTemperatureDriverData(quint8 address);
    Q_INVOKABLE void connectTemperatureSensor(quint8 address, bool state);
    Q_INVOKABLE void getSspdDriverData(quint8 address);
    Q_INVOKABLE void getSspdDriverParameters(quint8 address);
    Q_INVOKABLE void updateControlUnitsList();

    void setNewData(int dataListIndex, double value);

private:
    QString mLastIpAddress;
    DeviceList *mDevList;
    TemperatureData *mTempData;
    SspdData * mSspdData;
    int mCurrentAddress;
    cuTcpSocketIOInterface *mInterface;
    TempDriverM0 *mTempDriver;
    SspdDriverM0 *mSspdDriver;
    QStringList mControlUnitsList;
};

static QObject *appCoreProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto *singletonClass = new AppCore();
    return singletonClass;
}

#endif // APPCORE_H
