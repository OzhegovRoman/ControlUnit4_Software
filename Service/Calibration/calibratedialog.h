#ifndef CALIBRATEDIALOG_H
#define CALIBRATEDIALOG_H

#include <QObject>
#include <QDialog>
#include "Drivers/ccu4sdm0driver.h"
#include "Drivers/ccu4tdm0driver.h"
#include "cagilent34401avisainterface.h"

typedef enum {
    CU4SD,
    CU4TD,
    UNKNOWN
} tDriverType ;

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

    AbstractDriver *driver() const;
    void setDriver(AbstractDriver *driver);

    tDriverType driverType() const;
    void setDriverType(const tDriverType &driverType);

private slots:
    void on_pbStart_clicked();
    void messagePerform(QString str);
    void agilentCheckZero();
    void stopCalibration();

private:
    Ui::CalibrateDialog *ui;
    tDriverType mDriverType;
    AbstractDriver *mDriver{nullptr};
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
