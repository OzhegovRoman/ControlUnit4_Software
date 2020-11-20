#ifndef CU4TDM1CALIBRATOR_H
#define CU4TDM1CALIBRATOR_H

#include <QObject>
#include "commoncalibrator.h"
#include "Drivers/tempdriverm1.h"
#include "star_prc_structs.h"

class CU4TDM1Calibrator : public CommonCalibrator
{
    Q_OBJECT
public:
    explicit CU4TDM1Calibrator(QObject* parent = nullptr);

private:
    TempDriverM1* mDriver;
    int mChannel;
    TDM1_EepromCoeff mLastEepromConst;

    // CommonCalibrator interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual QStringList modeList() override;
    virtual QString driverType() override;

    int channel() const;
    void setChannel(int channel);

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

    virtual void finish() override;
};

#endif // CU4TDM1CALIBRATOR_H
