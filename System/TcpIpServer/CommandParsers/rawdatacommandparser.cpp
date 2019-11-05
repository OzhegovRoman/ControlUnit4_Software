#include "rawdatacommandparser.h"

RawDataCommandParser::RawDataCommandParser(QObject *parent)
    : cAbstractCommandParser(parent)
{

}

bool RawDataCommandParser::isCorrectCommand(const QByteArray &ba)
{
    /// проверка на несоответствие протоколу SCPI
    /// основная тонкость заключается в том, что в SCPI используется только текстовая информация
    /// а в обычном пакете данных первый символ - не соответствует текстовому символу, на этом и производим сверку протоколов
    /// делаем расшифровку команд
    if ((ba.size() > 2) &&
            (ba.size() > ba[2] + 2))
        return true;
    return false;
}
