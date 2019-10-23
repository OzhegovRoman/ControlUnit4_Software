#include "ctcpipprocess.h"
#include <QThreadPool>
#include <QTcpSocket>
#include "ctcpipserver.h"

cTcpIpProcess::cTcpIpProcess(QObject *parent)
    : QObject(parent)
    , mSocket(nullptr)
{
}

void cTcpIpProcess::initializeTcpIpSocket(qintptr handle)
{
    mSocket = new QTcpSocket(this);
    cTcpIpServer::consoleWriteDebug("A new socket created!");

    connect(mSocket, &QTcpSocket::disconnected, this, &cTcpIpProcess::killProcess);
    connect(mSocket, &QTcpSocket::readyRead, this, &cTcpIpProcess::read);

    mSocket->setSocketDescriptor(handle);
    cTcpIpServer::consoleWriteDebug(QString("Client connected at %1").arg(handle));
}

void cTcpIpProcess::killProcess()
{
    cTcpIpServer::consoleWriteDebug("Client disconnected");
    deleteLater();
}

void cTcpIpProcess::read()
{
    cTcpIpServer::consoleWriteDebug("cTcpIpProcess::read function");
    if (!mSocket){
        cTcpIpServer::consoleWriteError("socket is empty");
        return;
    }

    cTcpIpServer::consoleWriteDebug(QString("Socket %1 is ready to read").arg(mSocket->socketDescriptor()));
    QByteArray ba = mSocket->readAll();
    cTcpIpServer::consoleWriteDebug(QString("Socket %1 readed %2").arg(mSocket->socketDescriptor()).arg(ba.data()));
    emit socketReaded(this, ba);
}

void cTcpIpProcess::writeToSocket(QByteArray ba)
{
    cTcpIpServer::consoleWriteDebug("cTcpIpProcess::writeToSocket function");
    if (!mSocket){
        cTcpIpServer::consoleWriteError("socket is empty");
        return;
    }
    cTcpIpServer::consoleWriteDebug(QString("Socket %1 write: %2").arg(mSocket->socketDescriptor()).arg(ba.data()));
}
