#ifndef FTWIDGET_H
#define FTWIDGET_H

#include <QObject>

#define FT_DISPLAYWIDTH		FT_DISPLAY_HSIZE_WQVGA
#define FT_DISPLAYHEIGHT	FT_DISPLAY_VSIZE_WQVGA
#define FT_DISPLAY_RESOLUTION	FT_DISPLAY_WQVGA_480x272
#define FT_DISPLAY_ROTATE       1

#include "../FTDI/hardware/FT801/FT801.h"
#include "../FTDI/libraries/FT_GC/FT_Transport_SPI/FT_Transport_SPI_RaspPI.h"
#include "../FTDI/libraries/FT_GC/FT_GC.h"
#include "../FTDI/libraries/FT_GC/FT801/FT801Impl.h"

#include <QTimer>

using FT801_SPI = FT801Impl<FT_Transport_SPI_RaspPi>;

class FTWidget : public QObject
{
    Q_OBJECT
public:
    explicit FTWidget(QObject *parent = nullptr);

    FT801_SPI *ft801() const;
    void setFt801(FT801_SPI *ft801);

    int sleepTimer() const;
    void setSleepTimer(int sleepTimer);

    bool isStoped() const;

public slots:
    void exec();
    void terminate();

signals:

protected:
    virtual void setup() = 0;
    virtual void loop();

private:
    FT801_SPI *mFt801;
    bool mTerminateFlag;
    int mSleepTimer;
    QTimer *mTimer;
};

#endif // FTWIDGET_H
