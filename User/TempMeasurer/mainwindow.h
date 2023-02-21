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

#include <QtCharts/QChartView>
QT_CHARTS_USE_NAMESPACE

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
    void updateTemperatureGraph();
    void deleteMeasureData();
    void saveData();
    void chartContextMenuRequest(QPoint pos);

    void updateControlUnitList();

    void on_cbTcpIpAddress_activated(const QString &arg1);
    void showActions(bool visible = true);
    void setConnectedState(bool connected);

private:

    Ui::MainWindow *ui;
    enum {
        dtTempM0,
        dtUnknown
    } deviceType;

    CommonDriver *mDriver;
    IOInterface_t *mInterface;
    QTimer *mTimer;
    QString mLastTcpIpAddress;
    QString mFileName;
    int mMode{};
    double mCurrentValue{};

    bool isEnabledCounterAutoscale = true;
    bool isRangesVisible = true;

    QRectF chartRange;

};

#endif // MAINWINDOW_H
