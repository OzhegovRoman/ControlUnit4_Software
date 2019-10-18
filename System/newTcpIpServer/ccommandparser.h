#ifndef CCOMMANDPARSER_H
#define CCOMMANDPARSER_H

#include <QObject>

class cCommandParser : public QObject
{
    Q_OBJECT
public:
    explicit cCommandParser(QObject *parent = nullptr);

signals:

public slots:
    void parse(QObject* tcpIpProcess, QByteArray data);
private:
    bool isRawData(const QByteArray &ba);
};

#endif // CCOMMANDPARSER_H
