#include "cutcpsocketiointerface.h"
#include <QElapsedTimer>
#include <QApplication>
#include  <QNetworkProxy>
#include <QThread>
#include "servercommands.h"

cuTcpSocketIOInterface::cuTcpSocketIOInterface(QObject *parent)
    : cuIOInterfaceImpl(parent)
    , mSocket(new QTcpSocket(this))
    , mAddress(QHostAddress::Null)
    , mPort(SERVER_TCPIP_PORT)
{
    buffer.clear();
    mSocket->setProxy(QNetworkProxy::NoProxy);
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(dataReady()));
}

cuTcpSocketIOInterface::~cuTcpSocketIOInterface()
{
    if (!mSocket->isOpen()){
        mSocket->close();
        mSocket->abort();
    }
    mSocket->deleteLater();
}

QHostAddress cuTcpSocketIOInterface::address() const
{
    return mAddress;
}

void cuTcpSocketIOInterface::setAddress(const QHostAddress &address)
{
    mAddress = address;
    if (mSocket->isOpen()) mSocket->close();
}

quint16 cuTcpSocketIOInterface::port() const
{
    return mPort;
}

void cuTcpSocketIOInterface::setPort(const quint16 &port)
{
    mPort = port;
    if (mSocket->isOpen()) mSocket->close();
}

QString cuTcpSocketIOInterface::tcpIpQuery(QString query, int TimeOut, bool *ok)
{
    QElapsedTimer debugTimer;
    debugTimer.start();
    if (ok)
        *ok = true;

    if (!initialize()) {
        if (ok)
            *ok = false;
        return QString();
    }


    disconnect(mSocket, nullptr, nullptr, nullptr);

    mSocket->write(query.toLatin1());
    mSocket->flush();
    buffer.clear();

    QElapsedTimer timer;
    timer.start();
    while ((timer.elapsed()<TimeOut) && (buffer.isEmpty())){
        qApp->processEvents();
        buffer.append(mSocket->readAll());
    }

    buffer.append(mSocket->readAll());
    connect(mSocket, &QTcpSocket::readyRead, this, &cuTcpSocketIOInterface::dataReady, Qt::QueuedConnection);
    return QString(buffer);
}

bool cuTcpSocketIOInterface::pSendMsg(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    if (!isSocketReady()) {
        initialize();
        if (!isSocketReady())
            return false;
    }

    buffer.clear();

    QByteArray ba;
    ba.append(address);
    ba.append(command);
    ba.append(dataLength);
    ba.append((char*) data, dataLength);
    mSocket->write(ba);
    return true;
}

bool cuTcpSocketIOInterface::pInitialize()
{
    if (!isSocketReady()){
        mSocket->connectToHost(address(), port());
    }
    mSocket->waitForConnected(1000);
    return mSocket->state() == QAbstractSocket::ConnectedState;
}

void cuTcpSocketIOInterface::dataReady()
{
    buffer.append(mSocket->readAll());
    //int index = 0;
    while ((buffer.size() >= 3) && (buffer.size() >= buffer[2] + 3)){
        emit msgReceived(buffer[0],
                buffer[1],
                buffer[2],
                ((quint8*) buffer.data()) + 3);
        //index += buffer[index + 2] + 3;
        buffer.remove(0, buffer[2] + 3);
    }
}

bool cuTcpSocketIOInterface::isSocketReady()
{
    return (mSocket->isOpen() && (mSocket->state() & QAbstractSocket::ConnectedState));
}
