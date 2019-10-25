#include "ctcpipserver.h"
#include "Interfaces/cuiointerface.h"
#include <QtNetwork>
#include <QNetworkProxy>
#include <QNetworkInterface>
#include <stdio.h>
#include <iostream>
#include <QString>
#include "ctcpipprocess.h"
#include "ccommandexecutor.h"
#include <QThread>
#include <QCoreApplication>

cTcpIpServer::cTcpIpServer(QObject * parent)
    : QTcpServer(parent)
    , mExecutor(nullptr)
{

}

cTcpIpServer::~cTcpIpServer()
{
    stop();
}

void cTcpIpServer::initialize()
{
    consoleWriteDebug("Start command executor thread");
    if (!mExecutor){
        consoleWriteError("CommandExecutor are not exist!!!");
        return;
    }

    QThread *thread = new QThread(this);
    mExecutor->moveToThread(thread);

    //    qRegisterMetaType<>

    connect(thread, &QThread::started, mExecutor, &cCommandExecutor::doWork);
    connect(mExecutor, &cCommandExecutor::finished, thread, &QThread::quit);
    connect(mExecutor, &cCommandExecutor::finished, mExecutor, &cCommandExecutor::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(mExecutor, &cCommandExecutor::inited, this, &cTcpIpServer::startServer);
    connect(mExecutor, &cCommandExecutor::sendAnswer, this, &cTcpIpServer::sendAnswer);

    thread->start();
}

void cTcpIpServer::stop()
{
    if (!mExecutor)
        mExecutor->stop();
}

void cTcpIpServer::consoleWrite(QString string)
{
    std::cout<<string.toLatin1().data()<<std::endl;
}

void cTcpIpServer::consoleWriteDebug(QString string)
{
#ifdef DEBUG_LOG
    std::cout<<"Debug: "<<string.toLatin1().data()<<std::endl;
#endif
}

void cTcpIpServer::consoleWriteError(QString string)
{
    std::cerr<<"Error: "<<string.toLatin1().data()<<std::endl;
}

void cTcpIpServer::incomingConnection(qintptr handle)
{
    cTcpIpProcess *process = new cTcpIpProcess(this);
    process->initializeTcpIpSocket(handle);

    connect(process, &cTcpIpProcess::socketReaded, mExecutor, &cCommandExecutor::executeCommand);
    //ToDo: make  signal-slot connection for receiving answer
}

cCommandExecutor *cTcpIpServer::executor() const
{
    return mExecutor;
}

void cTcpIpServer::setExecutor(cCommandExecutor *executor)
{
    mExecutor = executor;
}

void cTcpIpServer::startServer()
{
    consoleWriteDebug("Start TcpIp Server");
    setProxy(QNetworkProxy::NoProxy);
    if (listen(QHostAddress::Any, 9876)){
        consoleWrite("Server: started");
        consoleWrite("Available TcpIp addresses:");
        for (QHostAddress address: QNetworkInterface::allAddresses())
            if (address.protocol() == QAbstractSocket::IPv4Protocol)
                consoleWrite(address.toString());
        consoleWrite("Available TcpIp Port: 9876");
    }
    else
        consoleWrite("Server: not started!");
}

void cTcpIpServer::sendAnswer(QObject *process, QByteArray data)
{
    if (process){
        auto tmp = qobject_cast<cTcpIpProcess*>(process);
        if (tmp)
            tmp->writeToSocket(data);
    }
}
