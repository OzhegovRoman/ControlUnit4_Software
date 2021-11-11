#ifndef CALIBRATEDIALOG_H
#define CALIBRATEDIALOG_H

#include <QObject>
#include <QDialog>
#include "cagilent34401avisainterface.h"
#include "Calibrator/commoncalibrator.h"

typedef enum {
    dtSspdDriverM0,
    dtSspdDriverM1,
    dtTempDriverM0,
    dtTempDriverM1,
    dtHeaterDriver,
    dtUnknown
} CU4DriverType;

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

    int channel() const;
    void setChannel(int channel);

private slots:
    void on_pbStart_clicked();
    void messagePerform(QString str);
    void changeProgress(int progress);
    void appendPoints(int graphIndex, double x, double y, bool update);
    void plotPerform();
    void stopCalibration();

private:
    Ui::CalibrateDialog *ui;
    CU4DriverType mDriverType;
    CommonDriver *mDriver{nullptr};
    cAgilent34401aVisaInterface *agilent;
    CommonCalibrator *mCalibrator;
    int mChannel;
    void enableComponents(bool state);
};

#endif // CALIBRATEDIALOG_H
