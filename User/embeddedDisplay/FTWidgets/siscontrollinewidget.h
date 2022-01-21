#ifndef SISCONTROLLINEWIDGET_H
#define SISCONTROLLINEWIDGET_H

#include <QObject>
#include "ftwidget.h"
#include "Drivers/siscontrollinedriverm0.h"
#include <QTimer>
#include "riverdieve.h"

class SisControlLineWidget : public FTWidget
{
    Q_OBJECT
public:
    explicit SisControlLineWidget(Gpu_Hal_Context_t * host);

    void setDriver(SisControlLineDriverM0 *driver);

signals:
    void backClicked();
    void optionsClicked();
    void restart();

    // FTWidget interface
protected:
    void setup();
    void loop();
private slots:
    void readData();
    void update();

private:
    enum {
        BT_Dummy = 254,
        BT_Back = 1,
        BT_Plus = 3,
        BT_Minus = 4,
        BT_Short = 5,
        BT_SetCurrent = 8
    };
    SisControlLineDriverM0 *mDriver;
    quint16 dlOffset;
    bool mUpdateFlag;
    bool dataReady;
    bool errorFlag;
    QTimer *dataTimer;
};

#endif // SISCONTROLLINEWIDGET_H
