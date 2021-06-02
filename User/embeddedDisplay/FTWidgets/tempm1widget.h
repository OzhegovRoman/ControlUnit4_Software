#ifndef TEMPM1WIDGET_H
#define TEMPM1WIDGET_H

#include <QObject>
#include "ftwidget.h"
#include "Drivers/tempdriverm1.h"
#include <QTimer>
#include "riverdieve.h"

class TempM1Widget: public FTWidget
{
    Q_OBJECT
public:
    explicit TempM1Widget(Gpu_Hal_Context_t * host);
    void setDriver(TempDriverM1 *driver);

signals:
    void backClicked();
protected:
    void setup();
    void loop();
private:
    enum {
       BT_Dummy = 254,
        BT_Back = 1,
        BT_Relay5V = 2,
        BT_Relay25V = 3
    };
    quint16 dlOffset;
    bool mUpdateFlag;
    TempDriverM1 *mDriver;
    bool dataReady;
    bool errorFlag;
    QTimer *dataTimer;
private slots:
    void readData();
    void update();
};

#endif // TEMPM1WIDGET_H
