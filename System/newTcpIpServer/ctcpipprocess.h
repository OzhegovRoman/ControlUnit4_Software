#ifndef CTCPIPPROCESS_H
#define CTCPIPPROCESS_H

#include <QObject>

class QTcpSocket;

class cTcpIpProcess : public QObject
{
    Q_OBJECT
public:
    explicit cTcpIpProcess(QObject *parent = nullptr);

    void initializeTcpIpSocket(qintptr handle);

signals:
    void socketReaded(QObject* process, QByteArray data);

public slots:
    void killProcess(void);
    void read();
    void writeToSocket(QByteArray ba);

private:
    QTcpSocket *mSocket;
};

#endif // CTCPIPPROCESS_H
