#ifndef CU4SDM1CALIBRATOR_H
#define CU4SDM1CALIBRATOR_H

#include <QObject>
#include "commoncalibrator.h"
#include "Drivers/sspddriverm1.h"

class CU4SDM1Calibrator: public CommonCalibrator
{
    Q_OBJECT
public:
    explicit CU4SDM1Calibrator(QObject* parent = nullptr);

    // CommonCalibrator interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual QStringList modeList() override;
    virtual QString driverType() override;

private:
    SspdDriverM1 * mDriver;
    CU4SDM1_EEPROM_Const_t mLastEeprom;

    // CommonCalibrator interface
protected:
    virtual void performAgilent() override;
    virtual void performDriver() override;
    virtual double readAdcValue() override;
    virtual void setNewDacEepromCoeffs(lineRegressionCoeff coeffs) override;
    virtual void setDacValue(double value) override;
    virtual void setNewAdcEepromCoeffs(lineRegressionCoeff coeffs) override;
    virtual void saveEepromConsts() override;
    virtual void restoreEepromConsts() override;
    virtual void performAdcConsts() override;
    virtual void performDacConsts() override;
    virtual void finish() override;
};

#endif // CU4SDM1CALIBRATOR_H
