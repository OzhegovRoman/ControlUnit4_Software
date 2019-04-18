#include "qCustomLib.h"

QHostAddress convertToHostAddress(QString str)
{
    QStringList strL = str.split('.');
    /// Данная функция нужна, чтобы не путались восьмиричные числа
    /// в случае чего используем стандартную функцию.
    if (strL.size()!=4) return QHostAddress(str);
    quint32 result = 0;
    for (int i = 0; i <4 ; ++i){
        result = result << 8;
        bool ok;
        result += QString(strL[i]).toInt(&ok);
        if (!ok) return QHostAddress(str);
    }
    return QHostAddress(result);
}
