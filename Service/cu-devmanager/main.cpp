#include <QCoreApplication>
#include <QCommandLineParser>
#include <QLoggingCategory>
#include <iostream>
#include "cdevmngr.h"
#include <QElapsedTimer>

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

    qDebug()<<"Device manager (utility for addition, delete and saving preferences)";

    if (parser.isSet(tcpIpOption)) {
        devMngr.setTcpIpProtocolEnabled(true);
        devMngr.setTcpIpAddress(parser.value(tcpIpOption));
        qDebug()<<"TcpIp Protocol:"<<devMngr.tcpIpAddress();
    }
    else {
        if (parser.isSet(serialPortOption)){
            devMngr.setRs485ProtocolEnabled(true);
            devMngr.setPortName(parser.value(serialPortOption));
            qDebug()<<"Serial Protocol:"<<devMngr.portName();
        }
        else{
            devMngr.setTcpIpProtocolEnabled(true);
            devMngr.setTcpIpAddress("127.0.0.1");
            qDebug()<<"TcpIp Protocol:"<<devMngr.tcpIpAddress();
        }
    }

    if (!devMngr.isTcpIpProtocol())
        devMngr.initializeDeviceList();


    for (;;){ // Бесконечный цикл? выйдем из него потом
        std::cout<<"> ";
        std::string str;
        std::getline(std::cin, str);
        QString qstr = QString(str.c_str());
        cDevMngr::Commands cmd = devMngr.processCommand(qstr);
        switch (cmd) {
        case cDevMngr::cmd_SetInterface:
        case cDevMngr::cmd_SetComPort:
        case cDevMngr::cmd_SetTcpIpAddress:
        case cDevMngr::cmd_AddDevice:
            qDebug()<<"DONE!!!";
            break;

        case cDevMngr::cmd_DeleteDevice:
            if (devMngr.isTcpIpProtocol())
                qDebug()<<"ERROR: Server doesn't support this command. Available only in serial interface mode";
            else
                qDebug()<<"DONE!!!";
            break;
        case cDevMngr::cmd_GetInterface:
            if (devMngr.isTcpIpProtocol())
                qDebug()<<"tcpip";
            else
                qDebug()<<"serial";
            break;
        case cDevMngr::cmd_GetTcpIpAddress:
            qDebug()<<devMngr.tcpIpAddress();
            break;
        case cDevMngr::cmd_GetComPort:
            qDebug()<<devMngr.portName();
            break;
        case cDevMngr::cmd_GetDeviceList:
            qDebug()<<"Device count:"<<devMngr.devList().count();
            for (int i = 0; i < devMngr.devList().count(); ++i){
                qDebug()<<QString("Dev: %1; address: %2; type: %3")
                          .arg(i)
                          .arg(devMngr.devList()[i].devAddress)
                          .arg(devMngr.devList()[i].devType);
            }
            break;
        case cDevMngr::cmd_ClearAll:
            if (devMngr.isTcpIpProtocol()){
                qDebug()<<"ERROR: Server doesn't support this command. Available only in serial interface mode";
            }
            else{
                devMngr.clearDeviceList();
                qDebug()<<"DONE!!!";
            }
            break;
        case cDevMngr::cmd_SaveData:
            devMngr.saveDeviceList();
            qDebug()<<"Done!!!";
            break;
        case cDevMngr::cmd_Help:
            qDebug()<<"  Short description of usage commands:";
            // Временно удалю это, поскольку это приводит к более сложному поведению программы
            //            qDebug()<<"  interface ? - getting information about prefered interface. Possible output: [serial, tcpip];";
            //            qDebug()<<"  interface <type> - setting up prefered interface. Possible input: [serial, tcpip];";
            //            qDebug()<<"  port ? - getting information about prefered COM port;";
            //            qDebug()<<"  port <portName> - setting up information about prefered COM port;";
            //            qDebug()<<"  tcpip ? - getting prefered tcpip address;";
            //            qDebug()<<"  tcpip <address> - setting up prefered tcpip address;";
            qDebug()<<"  devlist ? - request to get connected device list;";
            qDebug()<<"  add <device address> - add device with address to device list;";
            qDebug()<<"  delete <device address> - delete device with address from device list (This command available only with serial interface mode);";
            qDebug()<<"  clear - delete all devices from device list (This command available only with serial interface mode);";
            qDebug()<<"  save - save information about connected devices to the system;";
            qDebug()<<"  exit - quit from this application;";
            qDebug()<<"  quit - quit from this application;";
            qDebug()<<"  help - this command.";
            break;
        case cDevMngr::cmd_Exit:
            return 0;
        case cDevMngr::Error:
            qDebug()<<"Command execution error!!!";
            break;
        default:
            qDebug()<<"Command UNDEFINED. Please use \"help\" command for getting detailing description or \"exit\", \"quit\" for exit.";
            break;
        }
    }
}
