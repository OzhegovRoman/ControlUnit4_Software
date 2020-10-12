#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "Drivers/commondriver.h"

class DeviceList;
class TemperatureData;
class UnitData;
class cuTcpSocketIOInterface;

class AppCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool reconnectEnable READ reconnectEnable)
    Q_PROPERTY(QString lastIpAddress READ lastIpAddress NOTIFY lastIpAddressChanged)
    Q_PROPERTY(DeviceList *devList READ devList WRITE setDevList)
    Q_PROPERTY(UnitData *mUnitData READ getUnitData WRITE setUnitData)
    Q_PROPERTY(QStringList controlUnits READ controlUnits WRITE setControlUnits NOTIFY controlUnitsListChanged)

public:
    explicit AppCore(QObject *parent = nullptr);

    static bool reconnectEnable();
    static void setReconnectEnable(bool reconnectEnable);

    QString lastIpAddress() const;

    DeviceList *devList() const;
    void setDevList(DeviceList *devList);

    QStringList controlUnits() const;

    UnitData *getUnitData() const;
    void setUnitData(UnitData *unitData);

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
    Q_INVOKABLE void updateDriverData(quint8 address, QString type);
    Q_INVOKABLE void updateDriverParameters(quint8 address, QString type);
    Q_INVOKABLE void updateControlUnitsList();
    Q_INVOKABLE void prepareUnitData(quint8 address, QString type);

    void setNewData(int dataListIndex, double value);

private:
    QString mLastIpAddress;
    DeviceList *mDevList;
    UnitData * mUnitData;
    cuTcpSocketIOInterface *mInterface;
    CommonDriver * mDriver;
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
