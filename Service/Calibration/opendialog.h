#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class OpenDialog;
}

class OpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenDialog(QWidget *parent = nullptr);
    ~OpenDialog();

    QJsonObject jsonData() const;

    void setDeviceName(const QString &deviceName);
    void setDeviceType(const QString &deviceType);

private slots:
    void on_pbOpenFile_clicked();
    void on_pbReceiveFromFtp_clicked();

private:
    Ui::OpenDialog *ui;
    QJsonObject mJsonData;
    QString mDeviceName;
    QString mDeviceType;
};

#endif // OPENDIALOG_H
