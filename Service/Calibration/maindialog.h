#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include "Interfaces/cuiointerfaceimpl.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
#include "Drivers/heaterdriverm0.h"
#include "Drivers/siscontrollinedriverm0.h"
#include "Drivers/sisbiassourcedriverm0.h"

#include "savedialog.h"
#include "opendialog.h"
#include "calibratedialog.h"

#include "driverwidget.h"
#include "wsspddriver.h"
#include "wtempcalibr.h"
#include "wtempm1calibr.h"
#include "wheaterdriver.h"
#include "wsiscontrollinedriver.h"
#include "wsisbiassourcedriver.h"

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

    void setInterface(cuIOInterfaceImpl *interface);
    void setDeviceType(const QString &deviceType);

    void initializeUI();
    void setDeviceAddress(int deviceAddress);

    void enableGUI(bool enable = true);
    void disableGUI(){enableGUI(false);}

private slots:
    void on_pbGetEeprom_clicked();
    void on_pbSetEeprom_clicked();
    void on_pbWriteEeprom_clicked();

    void on_pbCalibrate_clicked();
    void on_pbSave_clicked();
    void on_pbLoad_clicked();

private:
    Ui::MainDialog *ui;
    DriverWidget *mDriverWidget;
    CU4DriverType mDriverType;
    CommonDriver *mDriver;
    cuIOInterfaceImpl *mInterface;
    int mDeviceAddress;
    SaveDialog *mSaveDialog;
    OpenDialog *mOpenDialog;
    CalibrateDialog *mCalibrate;
};

#endif // MAINDIALOG_H
