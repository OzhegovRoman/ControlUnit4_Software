#ifndef CU4SDM0CALIBRATOR_H
#define CU4SDM0CALIBRATOR_H

#include <QObject>
#include "commoncalibrator.h"
#include "Drivers/sspddriverm0.h"

class CU4SDM0Calibrator: public CommonCalibrator
{
    Q_OBJECT
public:
    explicit CU4SDM0Calibrator(QObject* parent = nullptr);

    // CommonCalibrator interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual QStringList modeList() override;
    virtual QString driverType() override;

private:
    SspdDriverM0 * mDriver;
    CU4SDM0V1_EEPROM_Const_t mLastEeprom;

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

#endif // CU4SDM0CALIBRATOR_H
