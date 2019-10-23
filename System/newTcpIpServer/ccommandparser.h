#ifndef CCOMMANDPARSER_H
#define CCOMMANDPARSER_H

#include <QObject>
#include "cdeviceinfo.h"

class cCommandParser : public QObject
{
    Q_OBJECT
public:
    explicit cCommandParser(QObject *parent = nullptr);
    void initialize();

    bool isInited() const;

signals:
    bool inited();

public slots:
    void parse(QObject* tcpIpProcess, QByteArray data);

private:
    QList<cDeviceInfo> mDeviceInfoList;
    bool mInited;
    bool isRawData(const QByteArray &ba);
    void prepareDeviceInfoList();
    void checkDevice(int address);
};

#endif // CCOMMANDPARSER_H
