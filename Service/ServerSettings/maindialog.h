#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QElapsedTimer>

#include "Interfaces/cutcpsocketiointerface.h"

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

    bool isTimeOut() const;

private slots:
    void on_pbConnect_clicked();
    void on_pbGetServerInfo_clicked();
    void on_pbSearchNewDevices_clicked();
    void on_pbAppendNewDevice_clicked();
    void on_pbUpdateDeviceList_clicked();

    void msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data);
    void timeOut();

    void setControlsEnabled(bool value);

private:
    Ui::MainDialog *ui;
    cuTcpSocketIOInterface *mIOInterface;
    bool mWaiting4Answer;
    bool mTimeOut;
    QTimer *mTimer;
    QElapsedTimer *mDebugTimer;

    bool waiting4Answer();
    void startReading();
};

#endif // MAINDIALOG_H
