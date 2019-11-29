#include "qCustomLib.h"
#include <QtNetwork>
#include "../CuPlugins/Server/servercommands.h"

#include <QObject>
#include <QElapsedTimer>
#include <QDebug>

QHostAddress convertToHostAddress(QString str)
{
    QStringList strL = str.split('.');
    /// Данная функция нужна, чтобы не путались восьмиричные числа
    /// в случае чего используем стандартную функцию.
    if (strL.size()!=4) return QHostAddress(str);
    quint32 result = 0;
    for (int i = 0; i <4 ; ++i){
        result = result << 8;
        bool ok;
        result += QString(strL[i]).toInt(&ok);
        if (!ok) return QHostAddress(str);
    }
    return QHostAddress(result);
}

QStringList availableControlUnits()
{
    // рассылаем Udp запросы
    QStringList answer;
    QTcpServer* server = new QTcpServer();

    server->setProxy(QNetworkProxy::NoProxy);
    if(!server->listen(QHostAddress::Any, SERVER_TCPIP_PORT + 2)){
        return answer;
    }

    QObject::connect(server, &QTcpServer::newConnection, [=, &server, &answer](){
        qDebug()<<"new connection";
        QTcpSocket* socket = server->nextPendingConnection();
        qDebug()<<"connection done";

        socket->setProxy(QNetworkProxy::NoProxy);
        bool ok = socket->waitForReadyRead(100);
        qDebug()<<"ok"<<ok;
        if (ok){
            QByteArray ba = socket->readAll();
            qDebug()<<ba;
            answer.append(QString(ba).split(";"));
        }
        socket->deleteLater();
    });

    qDebug()<<"before udpSocket create";
    QThread::msleep(1000);
    QUdpSocket udpSocket;
    udpSocket.setProxy(QNetworkProxy::NoProxy);
    QString tcpipAddresses;
    for (QHostAddress address: QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            if (address != QHostAddress::LocalHost)
                tcpipAddresses.append(address.toString()+";");

    QByteArray datagram = QByteArray(tcpipAddresses.toLocal8Bit());
    udpSocket.writeDatagram(datagram, QHostAddress::Broadcast, SERVER_TCPIP_PORT + 1);
    qDebug()<<udpSocket.bytesToWrite();
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed()<500)
        qApp->processEvents();

    qDebug()<<udpSocket.bytesToWrite();

    server->deleteLater();
    // подчищаем все лишнее
    // заменяем собственные адреса на localhost

    for (QStringList::iterator i = answer.begin(); i < answer.end();){
        if ((*i).isEmpty() || (QHostAddress(*i) == QHostAddress::LocalHost))
            i = answer.erase(i);
        else ++i;
    }

    for (QHostAddress address: QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol){
            //проверяем есть ли он в списке И меняем его на Localhost
            int i = answer.indexOf(address.toString());
            while (i>=0){
                answer[i] = "localhost";
                i = answer.indexOf(address.toString(), i+1);
            }
        }
    // удаляем повторы
    for (QStringList::iterator i = answer.begin(); i < answer.end(); ++i){
        for (QStringList::iterator j = i+1; j < answer.end();)
            if (*i == *j)
                j = answer.erase(j);
            else ++j;
    }
    // LocalHost меняем на 127.0.0.1
    int i = answer.indexOf("localhost");
    if (i>=0) answer[i] = "127.0.0.1";
    return answer;
}

