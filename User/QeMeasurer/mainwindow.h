#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include "Drivers/ccu4sdm0driver.h"

#ifdef FAKE
#include "Interfaces/fiointerface.h"
#include "Devices/FakeDevices/fcusdm0.h"
#endif

#ifdef RASPBERRY_PI
#include "Interfaces/curasppirs485iointerface.h"
#endif

#ifdef RS485
#include "Interfaces/curs485iointerface.h"
#endif

#ifdef TCPIP_SOCKET_INTERFACE
#include "Interfaces/cutcpsocketiointerface.h"
#include "Server/servercommands.h"
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
    void on_pbInitialize_clicked();
    void on_stackedWidget_currentChanged(int arg1);
    void updateData();
    void updateCountsGraph();
    void setCurrentValue();
    void addPoint2MeasureGraphs();
    void deleteMeasureData();
    void saveData();
    void enableControlsAtMeasure(bool value);
    void contextMenuRequest(QPoint pos);
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

private:
    Ui::MainWindow *ui;
    cCu4SdM0Driver *mDriver;
    IOInterface_t *mInterface;
    QTimer *mTimer;
    int mMode{};
    double mCurrentValue{};
};

#endif // MAINWINDOW_H