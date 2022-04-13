#include "cu4htm0calibrator.h"
#include "Interfaces/cutcpsocketiointerface.h"

CU4HTM0Calibrator::CU4HTM0Calibrator(QObject *parent)
    : CommonCalibrator(parent)
    , mDriver (nullptr)
    , coeff(3)
{

}

void CU4HTM0Calibrator::setDriver(CommonDriver *driver)
{
    // придется оздавать повторный драйвер и интерфейс для всей этой ерунды
    if (mDriver){
        mDriver->iOInterface()->deleteLater();
        mDriver->deleteLater();
    }

    auto * old_interface = qobject_cast<cuTcpSocketIOInterface*>(driver->iOInterface());

    assert(old_interface != nullptr);
    mDriver = new HeaterDriverM0(this);
    mDriver->setDevAddress(driver->devAddress());
    auto * interface = new cuTcpSocketIOInterface(this);
    interface->setPort(old_interface->port());
    interface->setAddress(old_interface->address());
    mDriver->setIOInterface(interface);
}

QStringList CU4HTM0Calibrator::modeList()
{
    return QStringList()<<"I mode";
}

QString CU4HTM0Calibrator::driverType()
{
    return "CU4HTM0";
}

void CU4HTM0Calibrator::performAgilent()
{
    agilent()->setMode(cAgilent34401A::Mode_I_DC);
}

void CU4HTM0Calibrator::performDriver()
{
    assert(mDriver != nullptr);
    mDriver->mode()->setValueSync(CU4HTM0V0_MODE_MANUAL, nullptr, 5);
    mDriver->relayStatus()->setValueSync(CU4HTM0V0_RELAY_OPEN, nullptr, 5);
}

void CU4HTM0Calibrator::saveEepromConsts()
{
    bool ok = false;
    mLastEeprom = mDriver->eepromConst()->getValueSync(&ok, 5);
    if (!ok) {
        emit message("ERROR: can't receive/set calibration data");
        terminate();
    }
}

void CU4HTM0Calibrator::restoreEepromConsts()
{
    mDriver->eepromConst()->setValueSync(mLastEeprom, nullptr, 5);
}

void CU4HTM0Calibrator::performAdcConsts()
{
    mLastEeprom.currentAdc.first = 1;
    mLastEeprom.currentAdc.second = 0;
}

void CU4HTM0Calibrator::performDacConsts()
{
    mLastEeprom.currentDac.first = 1/coeff;
    mLastEeprom.currentDac.second = 65535/2*(coeff - 1)/coeff;
}

void CU4HTM0Calibrator::setDacValue(double value)
{
    mDriver->current()->setValueSync(value, nullptr, 5);
}

double CU4HTM0Calibrator::readAdcValue()
{
    return mDriver->current()->getValueSync(nullptr, 5);
}

void CU4HTM0Calibrator::setNewDacEepromCoeffs(lineRegressionCoeff coeffs)
{
    mLastEeprom.currentDac.first    = coeffs.slope/coeff;
    mLastEeprom.currentDac.second   = coeffs.intercept/coeff  + 65535/2*(coeff - 1)/coeff;
}

void CU4HTM0Calibrator::setNewAdcEepromCoeffs(lineRegressionCoeff coeffs)
{
    mLastEeprom.currentAdc.first    = coeffs.slope;
    mLastEeprom.currentAdc.second   = coeffs.intercept;
}

void CU4HTM0Calibrator::finish()
{
    mDriver->relayStatus()->setValueSync(CU4HTM0V0_RELAY_CLOSE, nullptr, 5);
    mDriver->mode()->setValueSync(CU4HTM0V0_MODE_AUTO, nullptr, 5);
}
