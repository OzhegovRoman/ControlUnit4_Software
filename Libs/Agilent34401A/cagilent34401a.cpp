#include "cagilent34401a.h"
#include <QDebug>

cAgilent34401A::cAgilent34401A() :
    mMode{Mode_U_DC},
    mRange{DefRange},
    mResolution{DefResolution},
    mInited{false}
{

}

quint8 cAgilent34401A::mode() const
{
    return mMode;
}

void cAgilent34401A::setMode(const quint8 &mode)
{
    mMode = mode;
}

qreal cAgilent34401A::range() const
{
    return mRange;
}

void cAgilent34401A::setRange(const qreal &range)
{
    mRange = range;
}

qreal cAgilent34401A::resolution() const
{
    return mResolution;
}

void cAgilent34401A::setResolution(const qreal &resolution)
{
    mResolution = resolution;
}


bool cAgilent34401A::isInited() const
{
    return mInited;
}

qreal cAgilent34401A::singleRead()
{
    qDebug()<<"agilent initialize";
    if (!isInited()) initialize();
    QString str;
    switch (mMode) {
    case Mode_I_DC:
        str.append("MEAS:CURR:DC? ");
        break;
    case Mode_I_AC:
        str.append("MEAS:CURR:AC? ");
        break;
    case Mode_U_DC:
        str.append("MEAS:VOLT:DC? ");
        break;
    case Mode_U_AC:
        str.append("MEAS:VOLT:AC? ");
        break;
    case Mode_Res:
        str.append("MEAS:RES? ");
        break;
    case Mode_Res_4P:
        str.append("MEAS:FRES? ");
        break;
    case Mode_Freq:
        str.append("MEAS:FREQ? ");
        break;
    case Mode_Period:
        str.append("MEAS:PER? ");
        break;
    default:
        return 0;
    }

    if (mRange == DefRange) str.append("DEF,");
    else str.append(QString("%1,").arg(mRange));

    if (mResolution == DefResolution) str.append("DEF");
    else str.append(QString("%1").arg(mResolution));

    str.append("\r\n");

    qDebug()<<"set mode:"<<str;
    writeAgilent(str);

    qDebug()<<"read data";
    qreal data = readAgilent();
    qDebug()<<"done"<<data;
    return data;
}
