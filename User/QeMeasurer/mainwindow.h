#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include "Drivers/commondriver.h"

#ifdef FAKE
#include "Interfaces/fiointerface.h"
#include "Devices/FakeDevices/fcusdm0.h"
#endif

#ifdef RS485
#include "Interfaces/curs485iointerface.h"
#endif

#ifdef TCPIP_SOCKET_INTERFACE
#include "Interfaces/cutcpsocketiointerface.h"
#include "servercommands.h"
#endif


namespace Ui {
class MainWindow;
}

using IOInterface_t =
#ifdef TCPIP_SOCKET_INTERFACE
     cuTcpSocketIOInterface
#elif FAKE
    FakeIOInterface
#elif RASPBERRY_PI
    cuRs485IOInterface
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
    void updateCountsGraph();
    void setCurrentValue();
    void addPoint2MeasureGraphs();
    void deleteMeasureData();
    void saveData();
    void enableControlsAtMeasure(bool value);
    void measurerContextMenuRequest(QPoint pos);
    void counterContextMenuRequest(QPoint pos);
    void setAutoScaleCounterPlot(bool isAuto);
    void controlAutoScaleCounter();
    void setRangesVisible(bool isVisible);
    void on_pbSetI_clicked();
    void on_pbSetCmp_clicked();
    void on_cbShort_clicked(bool checked);
    void on_cbComparator_clicked(bool checked);
    void on_pbReadParams_clicked();
    void on_pbSetParams_clicked();
    void on_cbAmplifier_clicked(bool checked);
    void on_pbStart_clicked();
    void on_pbStop_clicked();

    void on_cbType_currentIndexChanged(int index);
    void on_pbGetSecretParams_clicked();
    void on_pbSetSecretParams_clicked();
    void on_pbReading_clicked();
    void on_pbRecording_clicked();

    void updateSecureData();
    void updateControlUnitList();

    void on_cbTcpIpAddress_activated(const QString &arg1);

    void on_cbHFMode_clicked(bool checked);

private:

    Ui::MainWindow *ui;
    enum {
        dtSspdM0,
        dtSspdM1,
        dtUnknown
    } deviceType;

    CommonDriver *mDriver;
    IOInterface_t *mInterface;
    QTimer *mTimer;
    QString mLastTcpIpAddress;
    QString mFileName;
    int mTimerCount_Interval;
    int mMode{};
    double mCurrentValue{};

    bool isEnabledCounterAutoscale = true;
    bool isRangesVisible = true;


};

#endif // MAINWINDOW_H
