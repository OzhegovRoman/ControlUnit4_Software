#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include "Drivers/sisbiassourcedriverm0.h"

#ifdef FAKE
#include "Interfaces/fiointerface.h"
#include "Devices/FakeDevices/fcusdm0.h"
#endif

#ifdef RS485
#include "Interfaces/curs485iointerface.h"
#endif

#ifdef TCPIP_SOCKET_INTERFACE
#include "Interfaces/cutcpsocketiointerface.h"
//#include "servercommands.h"
#endif

#include <QtCharts/QChartView>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

using IOInterface_t =
#ifdef TCPIP_SOCKET_INTERFACE
cuTcpSocketIOInterface
#elif FAKE
FakeIOInterface
#elif RASPBERRY_PI
CuRaspPiRs485IOInterface
#elif RS485
cuRs485IOInterface
#endif
;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void initialize();
    void on_stackedWidget_currentChanged(int arg1);
    void updateData();
    void setCurrentValue();
    void deleteMeasureData();
    void saveData();
    void enableControlsAtMeasure(bool value);
    void measurerContextMenuRequest(QPoint pos);
    void on_pbSetI_clicked();
    void on_cbShort_clicked(bool checked);
    void startSweep();
    void stopSweep();

    void updateControlUnitList();
    void on_cbTcpIpAddress_activated(const QString &arg1);
    void on_rbUmode_toggled(bool checked);

    void on_pbSetU_clicked();

private:

    Ui::MainWindow *ui;
    enum {
        dtSisBiasSource,
        dtUnknown
    } deviceType;

    SisBiasSourceDriverM0 *mDriver;
    IOInterface_t *mInterface;
    QTimer *mTimer;
    QString mLastTcpIpAddress;
    QString mFileName;
    int mTimerCount_Interval;
    int mMode{};
    double mCurrentValue{0};
    double mCurrentStep{0};
    double mCurrentMin{0};
    double mCurrentMax{0};
    bool mVoltageMode;
    bool mStopFlag;

    bool isEnabledCounterAutoscale = true;
    bool isRangesVisible = true;

    bool serverMessageReceived;
    bool sweepStoped;

    QRectF chartRange;

private slots:
    void serverMessageReceive(quint8 address, quint8 command, quint8 dataLength, quint8* data);
    void on_cbSweepType_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H
