#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "ftwidget.h"
#include "../DataHarvester/dataharvester.h"

class MainWidget: public FTWidget
{
    Q_OBJECT
public:
    explicit MainWidget(FT801_SPI * ft801, DataHarvester *harvester);

signals:
    void systemInfoClicked();
    void channelChoosen(int);

    // FTWidget interface
protected:
    void setup();
    void loop();

private:
    enum {
        BT_Info = 1,
        BT_SCROLLER = 2,
        BT_DriverButton = 0x80
    };

    const uint8_t maxDeviceListSize = 5;

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

    void update();

private slots:
    void dataHarvest();

};

#endif // MAINWIDGET_H
