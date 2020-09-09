#ifndef SSPDWIDGETM1_H
#define SSPDWIDGETM1_H

#include <QObject>
#include "ftwidget.h"
#include "Drivers/sspddriverm1.h"
#include <QTimer>
#include "riverdieve.h"

class SspdWidgetM1: public FTWidget
{
    Q_OBJECT
public:
    explicit SspdWidgetM1(Gpu_Hal_Context_t * host);
    void setDriver(SspdDriverM1 *driver);

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
        BT_HFMode = 8,
        BT_SetCurrent = 9
    };
    SspdDriverM1 *mDriver;
    QTimer *dataTimer;
    quint16 dlOffset;
    bool mUpdateFlag;
    bool dataReady;
    bool errorFlag;

private slots:
    void readData();
    void update();

};

#endif // SSPDWIDGET_H
