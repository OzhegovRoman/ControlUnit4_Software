#ifndef CUTCPSOCKETIOINTERFACE_H
#define CUTCPSOCKETIOINTERFACE_H

#include <QObject>
#include "cuiointerfaceimpl.h"
#include <QTcpSocket>
#include <QHostAddress>

class cuTcpSocketIOInterface : public cuIOInterfaceImpl
{
    Q_OBJECT
public:
    explicit cuTcpSocketIOInterface(QObject *parent = nullptr);
    ~cuTcpSocketIOInterface();

    QHostAddress address() const;
    void setAddress(const QHostAddress &address);

    quint16 port() const;
    void setPort(const quint16 &port);

    QString tcpIpQuery(QString query, int TimeOut, bool *ok = nullptr);

protected:
    bool pSendMsg(quint8 address, quint8 command, quint8 dataLength, quint8 *data);
    bool pInitialize();

private slots:
    void dataReady();

private:
    QTcpSocket *mSocket;
    QHostAddress mAddress;
    quint16 mPort;
    QByteArray buffer;
    bool isSocketReady();

};

#endif // CUTCPSOCKETIOINTERFACE_H
