#include "cu4sdm0calibrator.h"
#include "Interfaces/cutcpsocketiointerface.h"

CU4SDM0Calibrator::CU4SDM0Calibrator(QObject *parent)
    : CommonCalibrator(parent)
    , mDriver(nullptr)
{

}

void CU4SDM0Calibrator::setDriver(CommonDriver *driver)
{
    // придется оздавать повторный драйвер и интерфейс для всей этой ерунды
    if (mDriver){
        mDriver->iOInterface()->deleteLater();
        mDriver->deleteLater();
    }

    auto * old_interface = qobject_cast<cuTcpSocketIOInterface*>(driver->iOInterface());

    assert(old_interface != nullptr);
    mDriver = new SspdDriverM0(this);
    mDriver->setDevAddress(driver->devAddress());
    auto * interface = new cuTcpSocketIOInterface(this);
    interface->setPort(old_interface->port());
    interface->setAddress(old_interface->address());
    mDriver->setIOInterface(interface);
}

QStringList CU4SDM0Calibrator::modeList()
{
    return QStringList()<<"U mode"<<"I mode";
}

QString CU4SDM0Calibrator::driverType()
{
    return "CU4SDM0";
}

void CU4SDM0Calibrator::performAgilent()
{
    if (modeIndex() == 0)
        CommonCalibrator::performAgilent();
    else
        agilent()->setMode(cAgilent34401A::Mode_I_DC);
}

void CU4SDM0Calibrator::performDriver()
{
    assert(mDriver != nullptr);
    mDriver->PIDEnableStatus()->setValueSync(false, nullptr, 5);
    mDriver->shortEnable()->setValueSync(false, nullptr, 5);
}

void CU4SDM0Calibrator::setDacValue(double value)
{
    mDriver->current()->setValueSync(value, nullptr, 5);
}

void CU4SDM0Calibrator::setNewAdcEepromCoeffs(lineRegressionCoeff coeffs)
{
    if (modeIndex() == 0){
        mLastEeprom.Voltage_ADC.first = coeffs.slope;
        mLastEeprom.Voltage_ADC.second = coeffs.intercept;
    }
    else{
        mLastEeprom.Current_ADC.first = coeffs.slope;
        mLastEeprom.Current_ADC.second = coeffs.intercept;
    }
}

void CU4SDM0Calibrator::saveEepromConsts()
{
    bool ok = false;
    mLastEeprom = mDriver->eepromConst()->getValueSync(&ok, 5);
    if (!ok) {
        emit message("ERROR: can't receive/set calibration data");
        terminate();
    }
}

void CU4SDM0Calibrator::restoreEepromConsts()
{
    mDriver->eepromConst()->setValueSync(mLastEeprom, nullptr, 5);
}

void CU4SDM0Calibrator::performAdcConsts()
{
    if (modeIndex() == 0){
        mLastEeprom.Voltage_ADC.first = 1;
        mLastEeprom.Voltage_ADC.second = 0;
    }
    else{
        mLastEeprom.Current_ADC.first = 1;
        mLastEeprom.Current_ADC.second = 0;
    }
}

void CU4SDM0Calibrator::performDacConsts()
{
    mLastEeprom.Current_DAC.first = 1;
    mLastEeprom.Current_DAC.second = 0;
}

void CU4SDM0Calibrator::finish()
{
    mDriver->PIDEnableStatus()->setValueSync(true, nullptr, 5);
}

double CU4SDM0Calibrator::readAdcValue()
{
    if (modeIndex())
        return mDriver->current()->getValueSync(nullptr, 5);
    return mDriver->voltage()->getValueSync(nullptr, 5);
}

void CU4SDM0Calibrator::setNewDacEepromCoeffs(lineRegressionCoeff coeffs)
{
    mLastEeprom.Current_DAC.first = coeffs.slope;
    mLastEeprom.Current_DAC.second = coeffs.intercept;
}
