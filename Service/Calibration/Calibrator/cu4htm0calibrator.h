#ifndef CU4HTM0CALIBRATOR_H
#define CU4HTM0CALIBRATOR_H

#include "commoncalibrator.h"
#include <QObject>
#include "Drivers/heaterdriverm0.h"

class CU4HTM0Calibrator : public CommonCalibrator
{
public:
    explicit CU4HTM0Calibrator(QObject *parent = nullptr);

    // CommonCalibrator interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual QStringList modeList() override;
    virtual QString driverType() override;

protected:
    virtual void performAgilent() override;
    virtual void performDriver() override;
    virtual void saveEepromConsts() override;
    virtual void restoreEepromConsts() override;
    virtual void performAdcConsts() override;
    virtual void performDacConsts() override;
    virtual void setDacValue(double value) override;
    virtual double readAdcValue() override;
    virtual void setNewDacEepromCoeffs(lineRegressionCoeff coeffs) override;
    virtual void setNewAdcEepromCoeffs(lineRegressionCoeff coeffs) override;
    virtual void finish() override;

private:
    HeaterDriverM0 *mDriver;
    CU4HT0V0_EEPROM_Const_t mLastEeprom;
    float coeff;
};



#endif // CU4HTM0CALIBRATOR_H
