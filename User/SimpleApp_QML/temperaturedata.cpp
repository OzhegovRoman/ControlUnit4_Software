#include "temperaturedata.h"
#include <QDebug>

TemperatureData::TemperatureData(QObject *parent)
    : QObject(parent)
    , mTemperature(0)
    , mTemperatureSensorVoltage(0)
    , mPressure(0)
    , mPressureSensorVoltage(0)
    , mConnected(false)
{

}

double TemperatureData::temperature() const
{
    return mTemperature;
}

void TemperatureData::setTemperature(double temperature)
{
    mTemperature = temperature;
    emit temperatureChanged();
}

double TemperatureData::temperatureSensorVoltage() const
{
    return mTemperatureSensorVoltage;
}

void TemperatureData::setTemperatureSensorVoltage(double temeratureSensorVoltage)
{
    mTemperatureSensorVoltage = temeratureSensorVoltage;
    emit temperatureSensorVoltageChanged();
}

double TemperatureData::pressure() const
{
    return mPressure;
}

void TemperatureData::setPressure(double pressure)
{
    mPressure = pressure;
    emit pressureChanged();
}

double TemperatureData::pressureSensorVoltage() const
{
    return mPressureSensorVoltage;
}

void TemperatureData::setPressureSensorVoltage(double pressureSensorVoltage)
{
    mPressureSensorVoltage = pressureSensorVoltage;
    emit pressureSensorVoltageChanged();
}

bool TemperatureData::isConnected() const
{
    return mConnected;
}

void TemperatureData::setConnected(bool connected)
{
    mConnected = connected;
    if (connected)
        emit connectedChanged();
}
