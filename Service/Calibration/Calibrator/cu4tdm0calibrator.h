#ifndef CU4TDM0CALIBRATOR_H
#define CU4TDM0CALIBRATOR_H

#include <QObject>
#include "commoncalibrator.h"
#include "Drivers/tempdriverm0.h"

class CU4TDM0Calibrator: public CommonCalibrator
{
    Q_OBJECT
public:
    explicit CU4TDM0Calibrator(QObject* parent = nullptr);

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
    virtual double defaultValue() override;
    virtual void setDacValue(double value) override;
    virtual double readAdcValue() override;
    virtual void setNewDacEepromCoeffs(lineRegressionCoeff coeffs) override;
    virtual void setNewAdcEepromCoeffs(lineRegressionCoeff coeffs) override;

private:
    TempDriverM0* mDriver;
    CU4TDM0V1_EEPROM_Const_t mLastEeprom;
};

#endif // CU4TDM0CALIBRATOR_H
