#ifndef SERVERRAWDATACOMMANDPARSER_H
#define SERVERRAWDATACOMMANDPARSER_H

#include <QObject>
#include "rawdatacommandparser.h"

class ServerRawDataCommandParser: public RawDataCommandParser
{
    Q_OBJECT
public:
    explicit ServerRawDataCommandParser(QObject *parent = nullptr);

    // cAbstractCommandParser interface
protected:
    bool doIt(const QByteArray &ba) override;
};

#endif // SERVERRAWDATACOMMANDPARSER_H
