#include "cu4tdm0calibrator.h"
#include "Interfaces/cutcpsocketiointerface.h"

CU4TDM0Calibrator::CU4TDM0Calibrator(QObject *parent)
    : CommonCalibrator(parent)
    , mDriver (nullptr)
{

}

void CU4TDM0Calibrator::setDriver(CommonDriver *driver)
{
    // придется оздавать повторный драйвер и интерфейс для всей этой ерунды
    if (mDriver){
        mDriver->iOInterface()->deleteLater();
        mDriver->deleteLater();
    }

    auto * old_interface = qobject_cast<cuTcpSocketIOInterface*>(driver->iOInterface());

    assert(old_interface != nullptr);
    mDriver = new TempDriverM0(this);
    mDriver->setDevAddress(driver->devAddress());
    auto * interface = new cuTcpSocketIOInterface(this);
    interface->setPort(old_interface->port());
    interface->setAddress(old_interface->address());
    mDriver->setIOInterface(interface);
}

QStringList CU4TDM0Calibrator::modeList()
{
    return QStringList()<<"Pressure sensor Positive Voltage (Vp+)"
                       <<"Pressure sensor Negative Voltage (Vp-)"
                      <<"Temperature sensor Voltage (Vt)"
                     <<"Temperature sensor Current (It)";
}

QString CU4TDM0Calibrator::driverType()
{
    return "CU4TDM0";
}

void CU4TDM0Calibrator::performAgilent()
{
    if (modeIndex() == 3)
        agilent()->setMode(cAgilent34401A::Mode_I_DC);
    else
        CommonCalibrator::performAgilent();
}

void CU4TDM0Calibrator::performDriver()
{
    assert(mDriver != nullptr);
    mDriver->commutator()->setValueSync(true, nullptr, 5);
}

void CU4TDM0Calibrator::saveEepromConsts()
{
    bool ok = false;
    mLastEeprom = mDriver->eepromConst()->getValueSync(&ok, 5);
    if (!ok) {
        emit message("ERROR: can't receive/set calibration data");
        terminate();
    }
}

void CU4TDM0Calibrator::restoreEepromConsts()
{
    mDriver->eepromConst()->setValueSync(mLastEeprom, nullptr, 5);
}

void CU4TDM0Calibrator::performAdcConsts()
{
    switch (modeIndex()) {
    case 0:
        mLastEeprom.pressSensorVoltageP.first = 1;
        mLastEeprom.pressSensorVoltageP.second = 0;
        break;
    case 1:
        mLastEeprom.pressSensorVoltageN.first = 1;
        mLastEeprom.pressSensorVoltageN.second = 0;
        break;
    case 2:
        mLastEeprom.tempSensorVoltage.first = 1;
        mLastEeprom.tempSensorVoltage.second = 0;
        break;
    case 3:
        mLastEeprom.tempSensorCurrentAdc.first = 1;
        mLastEeprom.tempSensorCurrentAdc.second = 0;
        break;
    }
}

void CU4TDM0Calibrator::performDacConsts()
{
    mLastEeprom.tempSensorCurrentDac.first = 1;
    mLastEeprom.tempSensorCurrentDac.second = 0;
}

double CU4TDM0Calibrator::defaultValue()
{
    return 1e-5;
}

void CU4TDM0Calibrator::setDacValue(double value)
{
    mDriver->tempSensorCurrent()->setValueSync(value, nullptr, 5);
}

double CU4TDM0Calibrator::readAdcValue()
{
    switch (modeIndex()) {
    case 0:
        return mDriver->pressSensorVoltageP()->getValueSync(nullptr, 5);
    case 1:
        return mDriver->pressSensorVoltageN()->getValueSync(nullptr, 5);
    case 2:
        return mDriver->tempSensorVoltage()->getValueSync(nullptr, 5);
    case 3:
        return mDriver->tempSensorCurrent()->getValueSync(nullptr, 5);
    }
    return 0;
}

void CU4TDM0Calibrator::setNewDacEepromCoeffs(lineRegressionCoeff coeffs)
{
    mLastEeprom.tempSensorCurrentDac.first = coeffs.slope;
    mLastEeprom.tempSensorCurrentDac.second = coeffs.intercept;
}

void CU4TDM0Calibrator::setNewAdcEepromCoeffs(lineRegressionCoeff coeffs)
{
    switch (modeIndex()) {
    case 0:
        mLastEeprom.pressSensorVoltageP.first = coeffs.slope;
        mLastEeprom.pressSensorVoltageP.second = coeffs.intercept;
        break;
    case 1:
        mLastEeprom.pressSensorVoltageN.first = coeffs.slope;
        mLastEeprom.pressSensorVoltageN.second = coeffs.intercept;
        break;
    case 2:
        mLastEeprom.tempSensorVoltage.first = coeffs.slope;
        mLastEeprom.tempSensorVoltage.second = coeffs.intercept;
        break;
    case 3:
        mLastEeprom.tempSensorCurrentAdc.first = coeffs.slope;
        mLastEeprom.tempSensorCurrentAdc.second = coeffs.intercept;
        break;
    }
}
