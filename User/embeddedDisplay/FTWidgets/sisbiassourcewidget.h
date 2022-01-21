#ifndef SISBIASSOURCEWIDGET_H
#define SISBIASSOURCEWIDGET_H

#include <QObject>
#include "ftwidget.h"
#include "Drivers/sisbiassourcedriverm0.h"
#include <QTimer>
#include "riverdieve.h"

class SisBiasSourceWidget : public FTWidget
{
    Q_OBJECT
public:
    explicit SisBiasSourceWidget(Gpu_Hal_Context_t * host);
    void setDriver(SisBiasSourceDriverM0 *driver);

    // FTWidget interface
protected:
    void setup();
    void loop();

signals:
    void backClicked();
    void optionsClicked();
    void restart();

private:
    enum {
        BT_Dummy = 254,
        BT_Back = 1,
        BT_Plus = 3,
        BT_Minus = 4,
        BT_Short = 5,
        BT_Counter = 7,
        BT_SetCurrent = 8,
        BT_Mode = 9,
    };
    quint16 dlOffset;
    bool mUpdateFlag;
    SisBiasSourceDriverM0 *mDriver;
    bool dataReady;
    bool errorFlag;
    QTimer *dataTimer;

private slots:
    void readData();
    void update();

};

#endif // SISBIASSOURCEWIDGET_H
