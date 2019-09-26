#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include "Interfaces/cutcpsocketiointerface.h"
#include "Drivers/ccu4tdm0driver.h"
#include "Drivers/ccu4sdm0driver.h"

#include <QQmlEngine>
#include <QJSEngine>

class AppCore: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString coreMessage READ coreMessage)
    Q_PROPERTY(QString lastIpAddress READ getLastIpAddress)
    Q_PROPERTY(bool reconnectEnable READ getReconnectEnableFlag)
    Q_PROPERTY(float temperature READ getTemperature)
    Q_PROPERTY(float pressure READ getPressure)
    Q_PROPERTY(float tempSensorVoltage READ getTempSensorVoltage)
    Q_PROPERTY(float pressSensorVoltageP READ getPressSensorVoltageP)
    Q_PROPERTY(float pressSensorVoltageN READ getPressSensorVoltageN)
    Q_PROPERTY(float commutatorOn READ getTempCommutatorStatus)

    Q_PROPERTY(float sspdCurrent READ getSspdCurrent WRITE setSspdCurrent)
    Q_PROPERTY(float sspdVoltage READ getSspdVoltage)
    Q_PROPERTY(float sspdCounts READ getSspdCounts)

    Q_PROPERTY(bool sspdShorted READ getSspdShorted WRITE setSspdShorted)
    Q_PROPERTY(bool sspdAmplifierTurnedOn READ getSspdAmplifierTurnedOn WRITE setSspdAmplifierTurnedOn)
    Q_PROPERTY(bool sspdComparatorTurnedOn READ getSspdComparatorTurnedOn WRITE setSspdComparatorTurnedOn)
//    Q_PROPERTY(bool sspdCounterTurnedOn READ getSspdCounterTurnedOn) // WRITE
    Q_PROPERTY(bool sspdAutoResetTurnedOn READ getSspdAutoResetTurnedOn WRITE setSspdAutoResetTurnedOn)

    Q_PROPERTY(float sspdCmpRefLevel READ getSspdCmpRefLevel WRITE setSspdCmpRefLevel)
    Q_PROPERTY(float sspdCounterTimeConst READ getSspdCounterTimeConst WRITE setSspdCounterTimeConst)
    Q_PROPERTY(float sspdAutoResetThreshold READ getSspdAutoResetThreshold WRITE setSspdAutoResetThreshold)
    Q_PROPERTY(float sspdAutoResetTimeOut READ getSspdAutoResetTimeOut WRITE setSspdAutoResetTimeOut)

public:
    explicit AppCore(QObject *parent = nullptr);

    QString coreMessage() const
    {
        return mCoreMessage;
    }
    float getTemperature()
    {
        return last_TempData.Temperature;
    }

    float getPressure() const
    {
        return last_TempData.Pressure;
    }
    float getTempSensorVoltage() const
    {
        return last_TempData.TempSensorVoltage;
    }
    float getPressSensorVoltageP() const
    {
        return last_TempData.PressSensorVoltageP;
    }
    float getPressSensorVoltageN() const
    {
        return last_TempData.PressSensorVoltageN;
    }
    bool getTempCommutatorStatus() const
    {
        return last_TempData.CommutatorOn;
    }
    float getSspdCurrent() const
    {
        return last_SspdData.Current;
    }
    void setSspdCurrent(const float &value);

    float getSspdVoltage() const
    {
        return last_SspdData.Voltage;
    }
    float getSspdCounts() const
    {
        return last_SspdData.Counts;
    }
    bool getSspdShorted() const
    {
        return last_SspdData.Status.stShorted;
    }
    void setSspdShorted(const bool &value);

    bool getSspdAmplifierTurnedOn() const
    {
        return last_SspdData.Status.stAmplifierOn;
    }
    void setSspdAmplifierTurnedOn(const bool &value);

    bool getSspdComparatorTurnedOn() const
    {
        return last_SspdData.Status.stComparatorOn;
    }
    void setSspdComparatorTurnedOn(const bool &value);

//    bool getSspdCounterTurnedOn() const
//    {
//        return last_SspdData.Status.stCounterOn;
//    }

    bool getSspdAutoResetTurnedOn() const
    {
        return last_SspdData.Status.stAutoResetOn;
    }
    void setSspdAutoResetTurnedOn(const bool &value);

    float getSspdCmpRefLevel() const
    {
        return last_SspdParams.Cmp_Ref_Level;
    }
    void setSspdCmpRefLevel(const float &value);

    float getSspdCounterTimeConst() const
    {
        return last_SspdParams.Time_Const;
    }
    void setSspdCounterTimeConst(const float &value);

    float getSspdAutoResetThreshold() const
    {
        return last_SspdParams.AutoResetThreshold;
    }
    void setSspdAutoResetThreshold(const float &value);

    float getSspdAutoResetTimeOut() const
    {
        return last_SspdParams.AutoResetTimeOut;
    }
    void setSspdAutoResetTimeOut(const float &value);

    QString getLastIpAddress() const;

    bool getReconnectEnableFlag() const;
    void setReconnectEnableFlag(bool reconnectDisableFlag);

private:
    cuTcpSocketIOInterface *mInterface;
    CU4TDM0V1_Data_t last_TempData{};
    CU4SDM0V1_Data_t last_SspdData{};
    CU4SDM0V1_Param_t last_SspdParams{};
    cCu4TdM0Driver *mTempDriver;
    cCu4SdM0Driver *mSspdDriver;

public slots:
    Q_INVOKABLE void coreConnectToDefaultIpAddress();
    Q_INVOKABLE void coreConnectToIpAddress(const QString& ipAddress);
    Q_INVOKABLE void getTemperatureDriverData(quint8 address);
    Q_INVOKABLE void connectTemperatureSensor(quint8 address);
    Q_INVOKABLE void disConnectTemperatureSensor(quint8 address);
    Q_INVOKABLE void initializeSspdDriver(quint8 address);
    Q_INVOKABLE void getSspdData(quint8 address);

private slots:
    void connectToDevice();

signals:
    void connectionApply();
    void connectionReject();
    void temperatureDataDone();
    void sspdDriverInitialized();
    void sspdDataUpdated();
private:
    QString mCoreMessage;
    QString mLastIpAddress;
};

static QObject *appCoreProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto *singletonClass = new AppCore();
    return singletonClass;
}

#endif // APPCORE_H
