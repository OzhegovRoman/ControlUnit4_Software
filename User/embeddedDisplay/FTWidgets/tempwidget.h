#ifndef TEMPWIDGET_H
#define TEMPWIDGET_H

#include <QObject>
#include "ftwidget.h"
#include "Drivers/tempdriverm0.h"
#include <QTimer>
#include "riverdieve.h"

class TempWidget: public FTWidget
{
    Q_OBJECT
public:
    explicit TempWidget(Gpu_Hal_Context_t * host);
    // FTWidget interface
    void setDriver(TempDriverM0 *driver);

signals:
    void backClicked();

protected:
    void setup();
    void loop();
private:
    enum {
       BT_Dummy = 254,
        BT_Back = 1
    };
    quint16 dlOffset;
    bool mUpdateFlag;
    TempDriverM0 *mDriver;
    bool dataReady;
    bool errorFlag;
    QTimer *dataTimer;

private slots:
    void readData();
    void update();

};

#endif // TEMPWIDGET_H
