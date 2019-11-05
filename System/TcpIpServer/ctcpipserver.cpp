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

bool cTcpIpServer::mDebugInfoEnable = false;
bool cTcpIpServer::mErrorInfoEnable = false;
bool cTcpIpServer::mInfoEnable = false;

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
    if (mInfoEnable)
        std::cout<<string.toLatin1().data()<<std::endl;
}

void cTcpIpServer::consoleWriteDebug(QString string)
{
    if (mInfoEnable && mDebugInfoEnable)
        std::cout<<"Debug: "<<string.toLatin1().data()<<std::endl;
}

void cTcpIpServer::consoleWriteError(QString string)
{
    if (mInfoEnable && mErrorInfoEnable)
        std::cerr<<"Error: "<<string.toLatin1().data()<<std::endl;
}

void cTcpIpServer::incomingConnection(qintptr handle)
{
    cTcpIpProcess *process = new cTcpIpProcess(this);
    process->initializeTcpIpSocket(handle);

    connect(process, &cTcpIpProcess::socketReaded, mExecutor, &cCommandExecutor::executeCommand);
}

bool cTcpIpServer::isInfoEnabled() const
{
    return mInfoEnable;
}

void cTcpIpServer::setInfoEnable(bool infoEnable)
{
    mInfoEnable = infoEnable;
}

bool cTcpIpServer::isErrorInfoEnabled() const
{
    return mErrorInfoEnable;
}

void cTcpIpServer::setErrorInfoEnable(bool errorInfoEnable)
{
    mErrorInfoEnable = errorInfoEnable;
}

bool cTcpIpServer::isDebugInfoEnabled() const
{
    return mDebugInfoEnable;
}

void cTcpIpServer::setDebugInfoEnable(bool debugInfoEnable)
{
    mDebugInfoEnable = debugInfoEnable;
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
