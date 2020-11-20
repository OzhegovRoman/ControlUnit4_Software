#include "cu4tdm1calibrator.h"
#include "Interfaces/cutcpsocketiointerface.h"

CU4TDM1Calibrator::CU4TDM1Calibrator(QObject *parent)
    : CommonCalibrator(parent)
    , mDriver(nullptr)
{

}

int CU4TDM1Calibrator::channel() const
{
    return mChannel;
}

void CU4TDM1Calibrator::setChannel(int channel)
{
    mChannel = channel;
}

void CU4TDM1Calibrator::setDriver(CommonDriver *driver)
{
    // придется создавать повторный драйвер и интерфейс для всей этой ерунды
    if (mDriver){
        mDriver->iOInterface()->deleteLater();
        mDriver->deleteLater();
    }

    auto * old_interface = qobject_cast<cuTcpSocketIOInterface*>(driver->iOInterface());

    assert(old_interface != nullptr);
    mDriver = new TempDriverM1(this);
    mDriver->setDevAddress(driver->devAddress());
    auto * interface = new cuTcpSocketIOInterface(this);
    interface->setPort(old_interface->port());
    interface->setAddress(old_interface->address());
    mDriver->setIOInterface(interface);
}

QStringList CU4TDM1Calibrator::modeList()
{
    return QStringList()<<"U mode"
                       <<"I mode";
}

QString CU4TDM1Calibrator::driverType()
{
    return "CU4TDM1";
}

void CU4TDM1Calibrator::performAgilent()
{
    if (modeIndex() == 0)
        CommonCalibrator::performAgilent();
    else
        agilent()->setMode(cAgilent34401A::Mode_I_DC);
}

void CU4TDM1Calibrator::performDriver()
{
    // включаем Direct connect mode
    assert(mDriver != nullptr);
    mDriver->switcherMode()->setValueSync(cmd::smConnect, nullptr, 5);
}

void CU4TDM1Calibrator::saveEepromConsts()
{
    bool ok = mDriver->readEepromCoeffs();
    if (!ok) {
        emit message("ERROR: can't receive/set calibration data");
        terminate();
    }
}

void CU4TDM1Calibrator::restoreEepromConsts()
{
    bool ok = mDriver->writeEepromCoeffs();
    if (!ok) {
        emit message("ERROR: can't receive/set calibration data");
        terminate();
    }
}

void CU4TDM1Calibrator::performAdcConsts()
{
    setNewAdcEepromCoeffs({1,0});
}

void CU4TDM1Calibrator::performDacConsts()
{
    setNewDacEepromCoeffs({1,0});
}

double CU4TDM1Calibrator::defaultValue()
{
    return mDriver->defaultParam(mChannel).current;
}

void CU4TDM1Calibrator::setDacValue(double value)
{
    assert(mDriver != nullptr);
    mDriver->setCurrent(mChannel, value);
}

double CU4TDM1Calibrator::readAdcValue()
{
    if ((modeIndex() == 0) && (!mDriver->updateVoltage(mChannel)))
        return mDriver->currentVoltage(mChannel);

    // ток мы не измеряем, поэтому выводим 0ж
    return 0;
}

void CU4TDM1Calibrator::setNewDacEepromCoeffs(lineRegressionCoeff coeffs)
{
    mLastEepromConst.current.first  = coeffs.slope;
    mLastEepromConst.current.second = coeffs.intercept;
}

void CU4TDM1Calibrator::setNewAdcEepromCoeffs(lineRegressionCoeff coeffs)
{
    mLastEepromConst.voltage.first  = coeffs.slope;
    mLastEepromConst.voltage.second = coeffs.intercept;
}

void CU4TDM1Calibrator::finish()
{
    mDriver->switcherMode()->setValueSync(cmd::smAC, nullptr, 5);
}
