#ifndef TEMPARATUREDATA_H
#define TEMPARATUREDATA_H

#include <QObject>

class TemperatureData : public QObject
{
    Q_OBJECT
    Q_PROPERTY (double temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY (double temperatureSensorVoltage READ temperatureSensorVoltage NOTIFY temperatureSensorVoltageChanged)
    Q_PROPERTY (double pressure READ pressure NOTIFY pressureChanged)
    Q_PROPERTY (double pressureSensorVoltage READ pressureSensorVoltage NOTIFY pressureSensorVoltageChanged)
    Q_PROPERTY (bool isConnected READ isConnected NOTIFY connectedChanged)
public:
    explicit TemperatureData(QObject *parent = nullptr);

    double temperature() const;
    void setTemperature(double temperature);

    double temperatureSensorVoltage() const;
    void setTemperatureSensorVoltage(double temperatureSensorVoltage);

    double pressure() const;
    void setPressure(double pressure);

    double pressureSensorVoltage() const;
    void setPressureSensorVoltage(double pressureSensorVoltage);

    bool isConnected() const;
    void setConnected(bool isConnected);

signals:
    void temperatureChanged();
    void temperatureSensorVoltageChanged();
    void pressureChanged();
    void pressureSensorVoltageChanged();
    void connectedChanged();

public slots:

private:
    double mTemperature;
    double mTemperatureSensorVoltage;
    double mPressure;
    double mPressureSensorVoltage;
    bool mConnected;
};

#endif // TEMPARATUREDATA_H
