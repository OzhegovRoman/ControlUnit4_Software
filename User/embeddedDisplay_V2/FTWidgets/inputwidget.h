#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QObject>
#include "ftwidget.h"

class InputWidget
{
public:
    enum {
        BT_Back = 1,
        BT_Ok = 2
    };

    static double getDouble(FT801_SPI *ft801, QString title = QString());

};

#endif // INPUTWIDGET_H
