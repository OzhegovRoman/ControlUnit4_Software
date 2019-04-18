#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include "Interfaces/cuiointerfaceimpl.h"

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = 0);
    ~StartDialog();

    cuIOInterfaceImpl *interface() const;

    QString deviceType() const;
    int deviceAddress() const;

private slots:
    void on_buttonBox_accepted();
    void on_cbInterface_activated(int index);

private:
    Ui::StartDialog *ui;
    cuIOInterfaceImpl *mInterface;
    QString mDeviceType;
};

#endif // STARTDIALOG_H
