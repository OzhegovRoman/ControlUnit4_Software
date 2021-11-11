#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QTimer>
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
#include "Drivers/heaterdriverm0.h"

#include "Interfaces/cutcpsocketiointerface.h"
#include "servercommands.h"

using cInterface = cuTcpSocketIOInterface;

#include "tempwidget.h"
#include "tempm1widget.h"
#include "sspdwidget.h"
#include "sspdwidgetm1.h"
#include "allchannels.h"

#include <QElapsedTimer>

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = nullptr);
    ~MainDialog() override;

    void setAddress(const QHostAddress &address);

    void setTcpIpProtocolEnable(bool tcpIpProtocol);
    void setSerialProtocolEnable(bool serial){setTcpIpProtocolEnable( ! serial );}

    void setTcpIpAddress(const QString &tcpIpAddress);
    void setSerialPortName(const QString &serialPortName);

public slots:
    bool initialize();

private slots:
    void on_stackedWidget_currentChanged(int arg1);
    void changeTimeOut(int msecs);
    void timerTimeOut();
    void msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data);

private:
    Ui::MainDialog *ui;
    QVector<CommonDriver*> mDrivers;
    cuIOInterface *mInterface;
    bool mWaiting{};
    int lastWidgetIndex{-1};
    int mTimerTimeOut{500};
    QTimer *mTimer;
    QHostAddress mAddress;
    QElapsedTimer mElapsedTimer;
    bool mInited{false};

    bool mTcpIpProtocol{};
    QString mTcpIpAddress;
    QString mSerialPortName;
    AllChannels* aWidget{};

    bool waitingAnswer();
    bool createUI(const QString& deviceList);
};

#endif // MAINDIALOG_H
