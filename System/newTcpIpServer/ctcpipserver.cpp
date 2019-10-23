#include "ctcpipserver.h"
#include "Interfaces/cuiointerface.h"
#include <QtNetwork>
#include <QNetworkProxy>
#include <QNetworkInterface>
#include <stdio.h>
#include <iostream>
#include <QString>
#include "ctcpipprocess.h"
#include "ccommandparser.h"
#include "ccommandexecutor.h"
#include <QThread>
#include <QCoreApplication>

cTcpIpServer::cTcpIpServer(QObject * parent)
    : QTcpServer(parent)
    , mInterface(nullptr)
    , mParser(new cCommandParser(this))
    , mExecutor(new cCommandExecutor())
{

}

cTcpIpServer::~cTcpIpServer()
{
   stop();
}

void cTcpIpServer::StartServer()
{
    consoleWriteDebug("Start command executor thread");

    QThread *thread = new QThread(this);
    mExecutor->moveToThread(thread);

//    qRegisterMetaType<>

    connect(thread, &QThread::started, mExecutor, &cCommandExecutor::doWork);
    connect(mExecutor, &cCommandExecutor::finished, thread, &QThread::quit);
    connect(mExecutor, &cCommandExecutor::finished, mExecutor, &cCommandExecutor::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    QThread::msleep(10);

    mParser->initialize();

    // ждем пока не проинитится. Надеюсь, что signal-slot при таком подходе работают, возможно придется переделать на qApp->processEvent();
    while (!mParser->isInited())
        QThread::msleep(10);

    consoleWriteDebug("Start TcpIp Server");

    setProxy(QNetworkProxy::NoProxy);
    if (listen(QHostAddress::Any, 9876)){
        consoleWrite("Server: started");
        consoleWrite("Available TcpIp addresses:");
        for (QHostAddress address: QNetworkInterface::allAddresses())
            if (address.protocol() == QAbstractSocket::IPv4Protocol)
                consoleWrite(address.toString());
        consoleWrite("Available TcpIp Port: 9876");

        //TODO: подготовить список установленных устройств

//        enumerate();
//        qDebug()<<"Enumerate devices. Available"<<mUnits.count()<<"Device(s)";

    }
    else
        consoleWrite("Server: not started!");
}

void cTcpIpServer::stop()
{
    if (!mExecutor)
        mExecutor->stop();
}

cuIOInterface *cTcpIpServer::interface() const
{
    return mInterface;
}

void cTcpIpServer::setInterface(cuIOInterface *interface)
{
    mInterface = interface;
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

    connect(process, &cTcpIpProcess::socketReaded, mParser, &cCommandParser::parse);
    //ToDo: make  signal-slot connection for receiving answer
}
