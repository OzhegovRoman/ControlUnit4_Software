#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include "ftwidget.h"
#include "riverdieve.h"

class SystemInfo : public FTWidget
{
    Q_OBJECT
public:
    explicit SystemInfo(Gpu_Hal_Context_t * host);

    // FTWidget interface

signals:
    void backClicked();

protected:
    void setup();
    void loop();
private:
    enum {
        BT_Back = 1
    };

    QString myTcpIpAddress();
    QString mySerialNumber();
};



#endif // SYSTEMINFO_H
