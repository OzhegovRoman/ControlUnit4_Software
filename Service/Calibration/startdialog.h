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
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog() override;

    cuIOInterfaceImpl *interface() const;

    QString deviceType() const;
    int deviceAddress() const;

private slots:
    void on_buttonBox_accepted();
    void updateControlUnitList();
    void on_cbTcpIp_activated(const QString &arg1);

private:
    Ui::StartDialog *ui;
    cuIOInterfaceImpl *mInterface{nullptr};
    QString mDeviceType;
};

#endif // STARTDIALOG_H
