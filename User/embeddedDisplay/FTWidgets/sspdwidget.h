#ifndef SSPDWIDGET_H
#define SSPDWIDGET_H

#include <QObject>
#include "ftwidget.h"
#include "Drivers/sspddriverm0.h"
#include <QTimer>
#include "riverdieve.h"

class SspdWidget: public FTWidget
{
    Q_OBJECT
public:
    explicit SspdWidget(Gpu_Hal_Context_t * host);
    void setDriver(SspdDriverM0 *driver);

signals:
    void backClicked();
    void optionsClicked();
    void restart();

protected:
    void setup();
    void loop();
private:
    enum {
        BT_Back = 1,
        BT_Option = 2,
        BT_Plus = 3,
        BT_Minus = 4,
        BT_Short = 5,
        BT_Amp = 6,
        BT_Counter = 7,
        BT_SetCurrent = 8
    };
    quint16 dlOffset;
    bool mUpdateFlag;
    SspdDriverM0 *mDriver;
    bool dataReady;
    bool errorFlag;
    QTimer *dataTimer;

private slots:
    void readData();
    void update();

};

#endif // SSPDWIDGET_H
