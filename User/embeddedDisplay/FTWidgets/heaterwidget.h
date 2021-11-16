#ifndef HEATERWIDGET_H
#define HEATERWIDGET_H

#include "ftwidget.h"
#include "riverdieve.h"

#include "Drivers/heaterdriverm0.h"

class HeaterWidget : public FTWidget
{
    Q_OBJECT
public:
    explicit HeaterWidget(Gpu_Hal_Context_t * host);
    void setDriver(HeaterDriverM0 *driver);

signals:
    void backClicked();
    void optionsClicked();
    void restart();

public slots:
    void update();

protected:
    void setup();
    void loop();

private:
    enum {
        BT_Dummy        = 254,
        BT_Back         = 1,
        BT_MaxCurrent   = 2,
        BT_FrontEdge    = 3,
        BT_HoldTime     = 4,
        BT_RearEdge     = 5,
        BT_Heat         = 6
    };
    quint16 dlOffset;
    bool mUpdateFlag;
    HeaterDriverM0 *mDriver;
    bool dataReady;
    bool errorFlag;
    QTimer *dataTimer;
};

#endif // HEATERWIDGET_H
