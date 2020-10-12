#include "cu4sdm1calibrator.h"
#include "Interfaces/cutcpsocketiointerface.h"

CU4SDM1Calibrator::CU4SDM1Calibrator(QObject *parent)
    : CommonCalibrator(parent)
    , mDriver(nullptr)
{

}

void CU4SDM1Calibrator::setDriver(CommonDriver *driver)
{
    // придется оздавать повторный драйвер и интерфейс для всей этой ерунды
    if (mDriver){
        mDriver->iOInterface()->deleteLater();
        mDriver->deleteLater();
    }

    auto * old_interface = qobject_cast<cuTcpSocketIOInterface*>(driver->iOInterface());

    assert(old_interface != nullptr);
    mDriver = new SspdDriverM1(this);
    mDriver->setDevAddress(driver->devAddress());
    auto * interface = new cuTcpSocketIOInterface(this);
    interface->setPort(old_interface->port());
    interface->setAddress(old_interface->address());
    mDriver->setIOInterface(interface);
}

QStringList CU4SDM1Calibrator::modeList()
{
    return QStringList()<<"U mode"<<"I monitor mode"<<"I mode";
}

QString CU4SDM1Calibrator::driverType()
{
    return "CU4SDM1";
}

void CU4SDM1Calibrator::performAgilent()
{
    if (modeIndex() == 0)
        CommonCalibrator::performAgilent();
    else
        agilent()->setMode(cAgilent34401A::Mode_I_DC);
}

void CU4SDM1Calibrator::performDriver()
{
    assert(mDriver != nullptr);
    mDriver->PIDEnableStatus()->setValueSync(false, nullptr, 5);
    mDriver->shortEnable()->setValueSync(false, nullptr, 5);
}

void CU4SDM1Calibrator::setDacValue(double value)
{
    mDriver->current()->setValueSync(value, nullptr, 5);
}

void CU4SDM1Calibrator::setNewAdcEepromCoeffs(lineRegressionCoeff coeffs)
{
    switch (modeIndex()) {
    case 0:
        mLastEeprom.Voltage_ADC.first = coeffs.slope;
        mLastEeprom.Voltage_ADC.second = coeffs.intercept;
        break;
    case 1:
        mLastEeprom.CurrentMonitor_ADC.first = coeffs.slope;
        mLastEeprom.CurrentMonitor_ADC.second = coeffs.intercept;
        break;
    case 2:
        mLastEeprom.Current_ADC.first = coeffs.slope;
        mLastEeprom.Current_ADC.second = coeffs.intercept;
        break;
    }
}

void CU4SDM1Calibrator::saveEepromConsts()
{
    bool ok = false;
    mLastEeprom = mDriver->eepromConst()->getValueSync(&ok, 5);
    if (!ok) {
        emit message("ERROR: can't receive/set calibration data");
        terminate();
    }
}

void CU4SDM1Calibrator::restoreEepromConsts()
{
    mDriver->eepromConst()->setValueSync(mLastEeprom, nullptr, 5);
}

void CU4SDM1Calibrator::performAdcConsts()
{
    lineRegressionCoeff coeff;
    coeff.slope = 1;
    coeff.intercept = 0;
    setNewAdcEepromCoeffs(coeff);
}

void CU4SDM1Calibrator::performDacConsts()
{
    mLastEeprom.Current_DAC.first = 1;
    mLastEeprom.Current_DAC.second = 0;
}

void CU4SDM1Calibrator::finish()
{
    mDriver->PIDEnableStatus()->setValueSync(true, nullptr, 5);
}

double CU4SDM1Calibrator::readAdcValue()
{

    switch (modeIndex()) {
    case 0:
        return mDriver->voltage()->getValueSync(nullptr, 5);
        break;
    case 1:
        return mDriver->currentMonitor()->getValueSync(nullptr, 5);
        break;
    case 2:
        return mDriver->current()->getValueSync(nullptr, 5);
        break;
    }

    return 0;
}

void CU4SDM1Calibrator::setNewDacEepromCoeffs(lineRegressionCoeff coeffs)
{
    mLastEeprom.Current_DAC.first = coeffs.slope;
    mLastEeprom.Current_DAC.second = coeffs.intercept;
}
