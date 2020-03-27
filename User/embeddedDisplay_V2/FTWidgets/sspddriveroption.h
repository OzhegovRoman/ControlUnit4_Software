#ifndef SSPDDRIVEROPTION_H
#define SSPDDRIVEROPTION_H

#include <QObject>
#include "ftwidget.h"
#include "Drivers/sspddriverm0.h"

class SspdDriverOption
{
public:
    enum {
        BT_Back = 1,
        BT_ComparatorLevel = 2,
        BT_CounterTimeOut = 3,
        BT_AutoresetEnable = 4,
        BT_AutoresetThreshold = 5,
        BT_AutoresetTimeOut = 6
    };
    static void getOptions(FT801_SPI *ft801, SspdDriverM0* driver);
};

#endif // SSPDDRIVEROPTION_H
