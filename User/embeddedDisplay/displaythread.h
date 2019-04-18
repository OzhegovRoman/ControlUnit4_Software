#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H

#include <QThread>

#define FT_DISPLAYWIDTH		FT_DISPLAY_HSIZE_WQVGA
#define FT_DISPLAYHEIGHT	FT_DISPLAY_VSIZE_WQVGA
#define FT_DISPLAY_RESOLUTION	FT_DISPLAY_WQVGA_480x272
#define FT_DISPLAY_ROTATE       1

#include "../FTDI/hardware/FT801/FT801.h"
#include "../FTDI/libraries/FT_GC/FT_Transport_SPI/FT_Transport_SPI_RaspPI.h"
#include "../FTDI/libraries/FT_GC/FT_GC.h"
#include "../FTDI/libraries/FT_GC/FT801/FT801Impl.h"

#include "Drivers/ccu4sdm0driver.h"
#include "Drivers/ccu4tdm0driver.h"
#include "Interfaces/cutcpsocketiointerface.h"
#include "Server/servercommands.h"
#include <QElapsedTimer>

#define WelcomeScreenTime       6000 //6000
#define WatchDogTimeOut         20*60*1000 //20 минут
#define WorkDisplayTimeOut      50
#define SleepModeDisplayTimeOut 500 // как часто опрашивается экран в спящем режиме
#define WorkerTimeOut           500 // как часто опрашиваются устройства в рабочем режиме/

typedef FT801Impl<FT_Transport_SPI_RaspPi> FT801IMPL_SPI;

typedef enum Buttons
{
    BT_Next = 1,
    BT_Prev = 2,
    BT_SspdData = 3,
    BT_SspdCmp = 4,
    BT_Short = 5,
    BT_Amp = 6,
    BT_Cmp = 7,
    BT_Plus = 8,
    BT_Minus = 9,
    BT_0_1 = 10,
    BT_1 =11,
    BT_10 = 12,
    BT_100 = 13
} Buttons;

typedef enum WidgetTypes
{
    Temperature = 0,
    SSPD,

} WidgetTypes;


class cDisplay : public QObject
{
    Q_OBJECT
public:
    explicit cDisplay(QObject *parent = nullptr);

    void initialize();

    bool isTcpIpProtocol() const;
    void setTcpIpProtocolEnabled(bool value);
    void setRs485ProtocolEnabled(bool value){setTcpIpProtocolEnabled(!value);}

    QString tcpIpAddress() const;
    void setTcpIpAddress(const QString &value);

    QString rs485PortName() const;
    void setRs485PortName(const QString &rs485PortName);

    bool isFinished() const;

private:
    enum PowerMode {
        pmNormal,
        pmPreSleep,
        pmSleep
    } mPowerMode;

    FT801IMPL_SPI FTImpl;
    int mButtonTag, mLastButtonTag;
    int mCurrentIndex;

    QVector<AbstractDriver*> mDrivers;
    cuIOInterface *mInterface;

    QTimer *mWorkerTimer;
    QTimer *mDisplayTimer;
    bool mInited;
    bool mFinishAll;
    bool mFinished;

    bool mTcpIpProtocol;
    QString mTcpIpAddress;
    QString mRs485PortName;

    int mSspdButtonIndex;
    bool mSspdData;
    QTimer *mWDGTimer;

    bool isFT801founded();
    void displaySetUp();
    void welcomeScreen();
    void uploadArrows();
    void showSpinner();
    void initializeInterface();

    void createUi(QString deviceList);
    void show_TempWidget();
    void show_SspdWidget();
    void doFinish();

    void nextPrevModule(bool next);

    void showPreviouseButton(bool pressed);
    void showNextButton(bool pressed);
    void showTitle(QString title, int address);

    void buttonProcess();
    void sspdButtonProcess();

    void displayUpdate();

    void wakeUp();

signals:
    void quitSignal();

public slots:
private slots:
    void workerTimeOut();
    void displayTimeOut();
    void watchDogTimeOut();
    void finish();
};

#endif // DISPLAYTHREAD_H
