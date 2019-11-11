#include <QCoreApplication>
#include <QCommandLineParser>
#include <QLoggingCategory>
#include <iostream>
#include "cdevmngr.h"
#include <QElapsedTimer>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("cu-devmngr");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    QCommandLineParser parser;
    parser.setApplicationDescription("Device manager (utility for addition, delete and saving preferences)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption serialPortOption(QStringList()<<"s"<<"serial",
                                        QCoreApplication::translate("main", "specify serial port name. Default name is ttyS0"),
                                        QCoreApplication::translate("main", "serial port name"));
    parser.addOption(serialPortOption);

    QCommandLineOption tcpIpOption(QStringList()<<"t"<<"tcpip",
                                   QCoreApplication::translate("main", "use TCPIP protocol like main protokol. "
                                                                       "This option is stronger than the -s option"),
                                   QCoreApplication::translate("main", "TCPIP address"));
    parser.addOption(tcpIpOption);

    parser.process(a);

    cDevMngr devMngr;

    if (parser.isSet(tcpIpOption)) {
        devMngr.setTcpIpProtocolEnabled(true);
        devMngr.setTcpIpAddress(parser.value(tcpIpOption));
        std::cout << "TcpIp Protocol: " <<devMngr.tcpIpAddress().toLocal8Bit().data()<<std::endl;
    }
    else {
        if (parser.isSet(serialPortOption)){
            devMngr.setRs485ProtocolEnabled(true);
            devMngr.setPortName(parser.value(serialPortOption));
            std::cout << "Serial Protocol: " <<devMngr.portName().toLocal8Bit().data()<<std::endl;
        }
        else{
            devMngr.setTcpIpProtocolEnabled(true);
            devMngr.setTcpIpAddress("127.0.0.1");
            std::cout << "TcpIp Protocol: " <<devMngr.tcpIpAddress().toLocal8Bit().data()<<std::endl;
        }
    }

    if (!devMngr.isTcpIpProtocol())
        devMngr.initializeDeviceList();

    devMngr.run();

    return a.exec();
}
