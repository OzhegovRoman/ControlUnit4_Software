#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QObject>
#include "ftwidget.h"
#include "riverdieve.h"

class InputWidget
{
public:
    enum {
        BT_Dummy = 254,
        BT_Back = 1,
        BT_Ok = 3
    };

    static double getDouble(Gpu_Hal_Context_t *host, QString title = QString());

};

#endif // INPUTWIDGET_H
