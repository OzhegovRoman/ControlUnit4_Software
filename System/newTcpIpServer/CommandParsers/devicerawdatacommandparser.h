#ifndef DEVICERAWDATACOMMANDPARSER_H
#define DEVICERAWDATACOMMANDPARSER_H

#include <QObject>
#include "rawdatacommandparser.h"

class DeviceRawDataCommandParser : public RawDataCommandParser
{
    Q_OBJECT
public:
    explicit DeviceRawDataCommandParser(QObject *parent = nullptr);

    // cAbstractCommandParser interface
protected:
    bool doIt(const QByteArray &ba) override;

private slots:
    void msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data);
private:
    bool mStopFlag;

    // cAbstractCommandParser interface
public:
    void initializeParser() override;
};

#endif // DEVICERAWDATACOMMANDPARSER_H
