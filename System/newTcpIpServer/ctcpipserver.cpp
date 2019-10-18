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

cTcpIpServer::cTcpIpServer(QObject * parent)
    : QTcpServer(parent)
    , mInterface(nullptr)
    , mParser(new cCommandParser(this))
{

}

void cTcpIpServer::StartServer()
{
    consoleWriteDebug("Start server");
    setProxy(QNetworkProxy::NoProxy);
    if (listen(QHostAddress::Any, 9876)){
        consoleWrite("Server: started");
        consoleWrite("Available TcpIp addresses:");
        for (QHostAddress address: QNetworkInterface::allAddresses())
            if (address.protocol() == QAbstractSocket::IPv4Protocol)
                consoleWrite(address.toString());
        consoleWrite("Available TcpIp Port: 9876");

        //TODO: подготовить список учтановленных устройств

//        enumerate();
//        qDebug()<<"Enumerate devices. Available"<<mUnits.count()<<"Device(s)";

    }
    else
        consoleWrite("Server: not started!");
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
