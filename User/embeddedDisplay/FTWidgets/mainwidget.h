#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "ftwidget.h"
#include "../DataHarvester/dataharvester.h"
#include "riverdieve.h"

class MainWidget: public FTWidget
{
    Q_OBJECT
public:
    explicit MainWidget(Gpu_Hal_Context_t * host, DataHarvester *harvester);

signals:
    void systemInfoClicked();
    void channelChoosen(int);

    // FTWidget interface
protected:
    void setup();
    void loop();

private:
    enum {
        BT_Info = 2,
        BT_SCROLLER = 3,
        BT_DriverButton = 0x80
    };

    const uint8_t maxDeviceListSize = 5;
    const int32_t animationPeriod = 500;

    struct DataInfoHelper{
        bool channelInited;
        bool channelUpdated;
        bool channelError;
    };

    DataHarvester* mHarvester;
    quint16 dlOffset;
    bool mUpdateFlag;
    quint8 mTopIndex;
    QVector<DataInfoHelper> dataInfo;
    QTimer *dataHarvesterTimer;

    int32_t animationProcess = 0;

    void update();

private slots:
    void dataHarvest();

};

#endif // MAINWIDGET_H
