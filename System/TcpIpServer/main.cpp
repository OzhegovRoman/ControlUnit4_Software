#include <QCoreApplication>
#include "Interfaces/curs485iointerface.h"
#include <QCommandLineParser>
#include <QDebug>

#include "ctcpipserver.h"
#include "ccommandexecutor.h"

#ifdef FAKE_DEVICES
#include "fakesspddevice.h"
#include "faketempdevice.h"
#include "fiointerface.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("cu-tcpipserver");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple Tcp/Ip server for operating device via internet for Scontel\'s control unit (CU4)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption serialPortOption(QStringList()<<"s"<<"serial",
                                        QCoreApplication::translate("main", "specify serial port name. Default name is ttyS0"),
                                        QCoreApplication::translate("main", "serial port name"));
    parser.addOption(serialPortOption);

    QCommandLineOption debugInfoOption(QStringList()<<"d"<<"debug",
                                        QCoreApplication::translate("main", "Debug Info Enable"));
    parser.addOption(debugInfoOption);

    QCommandLineOption commonInfoOption(QStringList()<<"i"<<"info",
                                        QCoreApplication::translate("main", "Common Info Enable"));
    parser.addOption(commonInfoOption);

    QCommandLineOption errorOption(QStringList()<<"e"<<"error",
                                        QCoreApplication::translate("main", "Error messages Enable"));
    parser.addOption(errorOption);

#ifdef FAKE_DEVICES
    QCommandLineOption fakeOptions(QStringList()<<"f"<<"fake",
                                   QCoreApplication::translate("main", "Fake option. Server will be operate in simulation mode with 2 SSRD Devices(address 0 and 1). "
                                                                       "If this option is setted options \"-s\" will be ignored"));
    parser.addOption(fakeOptions);
#endif

    parser.process(a);
    cuIOInterface *mInterface = nullptr;


#ifdef FAKE_DEVICES
    if (parser.isSet(fakeOptions)) {       
       QString ts = QTime::currentTime().toString("hh:mm:ss:zzz");
        qDebug() << ts.toLatin1().data() <<"emulation mode";
        FakeIOInterface *tmpInterface = new FakeIOInterface();

        mInterface = tmpInterface;

        FakeTempDevice *tempDevice = new FakeTempDevice();
        tempDevice->installFakeInterface(tmpInterface);
        tempDevice->setAddress(0);
        tempDevice->deviceInfo()->setModificationVersion("0");
        tempDevice->deviceInfo()->setHardwareVersion("1");
        tempDevice->deviceInfo()->setFirmwareVersion("0.1.0");
        tempDevice->deviceInfo()->setDeviceType(DeviceInfo::dtTemperatureDevice);

        quint8 tmpUDID[12];
        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i;
        tempDevice->deviceInfo()->setDeviceUID(tmpUDID);


        FakeSspdDevice * device0 = new FakeSspdDevice();
        device0->installFakeInterface(tmpInterface);

        device0->setAddress(1);
        device0->setLightOn(true);

        device0->deviceInfo()->setModificationVersion("0");
        device0->deviceInfo()->setHardwareVersion("1");
        device0->deviceInfo()->setFirmwareVersion("0.1.0");
        device0->deviceInfo()->setDeviceType(DeviceInfo::dtSspdDevice);

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        device0->deviceInfo()->setDeviceUID(tmpUDID);

        FakeSspdDevice *device1 = new FakeSspdDevice();
        device1->installFakeInterface(tmpInterface);
        device1->setAddress(2);
        device1->setLightOn(false);

        device1->deviceInfo()->setModificationVersion("0");
        device1->deviceInfo()->setHardwareVersion("2");
        device1->deviceInfo()->setFirmwareVersion("0.1.2");
        device1->deviceInfo()->setDeviceType(DeviceInfo::dtSspdDevice);

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+2;
        device1->deviceInfo()->setDeviceUID(tmpUDID);
    }
    else {
#endif
        QString portName = "ttyS0";
        if (parser.isSet(serialPortOption)){
            portName = parser.value(serialPortOption);
        }
        cuRs485IOInterface *tmpInterface = new cuRs485IOInterface();
        tmpInterface->setPortName(portName);
        mInterface = tmpInterface;
#ifdef FAKE_DEVICES
    }
#endif

    cTcpIpServer *server = new cTcpIpServer();
    cCommandExecutor *executor = new cCommandExecutor();
    executor->setInterface(mInterface);
    server->setExecutor(executor);
    server->setInfoEnable(parser.isSet(commonInfoOption));
    server->setDebugInfoEnable(parser.isSet(debugInfoOption));
    server->setErrorInfoEnable(parser.isSet(errorOption));

    server->initialize();
    return a.exec();
}
