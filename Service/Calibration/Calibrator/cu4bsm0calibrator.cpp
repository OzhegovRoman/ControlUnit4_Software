#include "cu4bsm0calibrator.h"
#include "Interfaces/cutcpsocketiointerface.h"

CU4BSM0Calibrator::CU4BSM0Calibrator(QObject *parent)
    : CommonCalibrator(parent)
    , mDriver (nullptr)
{

}


void CU4BSM0Calibrator::setDriver(CommonDriver *driver)
{
    // придется оздавать повторный драйвер и интерфейс для всей этой ерунды
    if (mDriver){
        mDriver->iOInterface()->deleteLater();
        mDriver->deleteLater();
    }

    auto * old_interface = qobject_cast<cuTcpSocketIOInterface*>(driver->iOInterface());

    assert(old_interface != nullptr);
    mDriver = new SisBiasSourceDriverM0(this);
    mDriver->setDevAddress(driver->devAddress());
    auto * interface = new cuTcpSocketIOInterface(this);
    interface->setPort(old_interface->port());
    interface->setAddress(old_interface->address());
    mDriver->setIOInterface(interface);
}

QStringList CU4BSM0Calibrator::modeList()
{
    return QStringList()<<"U mode"<<"I mode";
}

QString CU4BSM0Calibrator::driverType()
{
    return "CU4BSM0";
}

void CU4BSM0Calibrator::performDriver()
{
    assert(mDriver != nullptr);
    mDriver->pidEnable()->setValueSync(false, nullptr, 5);
    mDriver->shortEnable()->setValueSync(false, nullptr, 5);
}

void CU4BSM0Calibrator::saveEepromConsts()
{
    bool ok = false;
    mLastEeprom = mDriver->eepromConst()->getValueSync(&ok, 5);
    if (!ok) {
        emit message("ERROR: can't receive/set calibration data");
        terminate();
    }
}

void CU4BSM0Calibrator::restoreEepromConsts()
{
    mDriver->eepromConst()->setValueSync(mLastEeprom, nullptr, 5);
}

void CU4BSM0Calibrator::performAdcConsts()
{
    if (modeIndex() == 0){
        mLastEeprom.voltageAdc.first = 1;
        mLastEeprom.voltageAdc.second = 0;
    }
    else{
        mLastEeprom.currentAdc.first = 1;
        mLastEeprom.currentAdc.second = 0;
    }
}

void CU4BSM0Calibrator::performDacConsts()
{
    if (modeIndex() == 0){
        mLastEeprom.voltageDac.first = 1;
        mLastEeprom.voltageDac.second = 0;
    }
    else{
        mLastEeprom.currentDac.first = 1;
        mLastEeprom.currentDac.second = 0;
    }
}

void CU4BSM0Calibrator::setDacValue(double value)
{
    if (modeIndex() == 0){
        mDriver->voltage()->setValueSync(value, nullptr, 5);
    }
    else{
        mDriver->current()->setValueSync(value, nullptr, 5);
    }

}

double CU4BSM0Calibrator::readAdcValue()
{
    if (modeIndex() == 0){
        return mDriver->voltage()->getValueSync(nullptr, 5);
    }
    else {
        return mDriver->current()->getValueSync(nullptr, 5);
    }
}

void CU4BSM0Calibrator::setNewDacEepromCoeffs(lineRegressionCoeff coeffs)
{
    if (modeIndex() == 0){
        mLastEeprom.voltageDac.first = coeffs.slope;
        mLastEeprom.voltageDac.second = coeffs.intercept;
    }
    else {
        mLastEeprom.currentDac.first = coeffs.slope;
        mLastEeprom.currentDac.second = coeffs.intercept;
    }
}

void CU4BSM0Calibrator::setNewAdcEepromCoeffs(lineRegressionCoeff coeffs)
{
    if (modeIndex() == 0){
        mLastEeprom.voltageAdc.first = coeffs.slope;
        mLastEeprom.voltageAdc.second = coeffs.intercept;
    }
    else{
        mLastEeprom.currentAdc.first = coeffs.slope;
        mLastEeprom.currentAdc.second = coeffs.intercept;
    }
}

void CU4BSM0Calibrator::finish()
{
    mDriver->pidEnable()->setValueSync(true, nullptr, 5);
}
