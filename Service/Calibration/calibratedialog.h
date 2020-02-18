#ifndef CALIBRATEDIALOG_H
#define CALIBRATEDIALOG_H

#include <QObject>
#include <QDialog>
#include "Drivers/sspddriverm0.h"
#include "Drivers/tempdriverm0.h"
#include "cagilent34401avisainterface.h"

typedef enum {
    dtSspdDriverM0,
    dtTempDriverM0,
    dtUnknown
} CU4DriverType ;

namespace Ui {
class CalibrateDialog;
}

class CalibrateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrateDialog(QWidget *parent = nullptr);
    ~CalibrateDialog() override;

    int exec() override;

    CommonDriver *driver() const;
    void setDriver(CommonDriver *driver);

    CU4DriverType driverType() const;
    void setDriverType(const CU4DriverType &driverType);

private slots:
    void on_pbStart_clicked();
    void messagePerform(QString str);
    void agilentCheckZero();
    void stopCalibration();

private:
    Ui::CalibrateDialog *ui;
    CU4DriverType mDriverType;
    CommonDriver *mDriver{nullptr};
    CU4SDM0V1_EEPROM_Const_t lastCU4SDEepromConst;
    CU4TDM0V1_EEPROM_Const_t lastCU4TDEepromConst;
    cAgilent34401aVisaInterface *agilent;

    QVector<double> x[2], y[2];
    bool stopFlag;
    void enableComponents(bool state);
    void updateGraphData();
    void plotPerform();
    void agilentPerform();
    void eepromConstInitialize();
    void setCurrent(float value);
    void calibrationFirstStep();
    void deleteBadPoints();
    void sleep(int msec);

};

#endif // CALIBRATEDIALOG_H
