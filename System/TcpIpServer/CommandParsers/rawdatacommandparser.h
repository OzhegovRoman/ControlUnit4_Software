#ifndef RAWDATACOMMANDPARSER_H
#define RAWDATACOMMANDPARSER_H

#include <QObject>
#include "cabstractcommandparser.h"

class RawDataCommandParser : public cAbstractCommandParser
{
    Q_OBJECT
public:
    explicit RawDataCommandParser(QObject *parent = nullptr);

    // cAbstractCommandParser interface
protected:
    bool isCorrectCommand(const QByteArray &ba) override;
};

#endif // RAWDATACOMMANDPARSER_H
