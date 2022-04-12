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

private slots:
    void msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data);


    // cAbstractCommandParser interface
public:
    void initializeParser();
};

#endif // SERVERRAWDATACOMMANDPARSER_H
