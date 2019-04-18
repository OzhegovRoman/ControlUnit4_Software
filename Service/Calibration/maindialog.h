#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include "Interfaces/cuiointerfaceimpl.h"
#include "Drivers/ccu4sdm0driver.h"
#include "Drivers/ccu4tdm0driver.h"
#include "savedialog.h"
#include "opendialog.h"
#include "calibratedialog.h"

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
    tDriverType mDriverType;
    AbstractDriver *mDriver;
    cuIOInterfaceImpl *mInterface;
    int mDeviceAddress;
    SaveDialog *mSaveDialog;
    OpenDialog *mOpenDialog;
    CalibrateDialog *mCalibrate;
};

#endif // MAINDIALOG_H