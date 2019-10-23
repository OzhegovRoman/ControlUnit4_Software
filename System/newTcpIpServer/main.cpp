#include <QCoreApplication>
#include "Interfaces/curs485iointerface.h"
#include <QCommandLineParser>
#include <QDebug>

#include "ctcpipserver.h"

#ifdef FAKE_DEVICES
#include "../../../Libs/FakeDevicesLib/fakesspddevice.h"
#include "../../../Libs/FakeDevicesLib/fiointerface.h"
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
        FakeIOInterface *tmpInterface = new FakeIOInterface();
        mInterface = tmpInterface;
        FakeSspdDevice *device0 = new FakeSspdDevice();
        device0->installFakeInterface(tmpInterface);

        device0->setAddress(0);
        device0->setLightOn(true);
        DeviceInfo *info = device0->deviceInfo();


        info->setModificationVersion("0");
        info->setHardwareVersion("1");
        info->setFirmwareVersion("0.1.0");

        quint8 tmpUDID[12];
        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i;
        info->setDeviceUID(tmpUDID);

        FakeSspdDevice *device1 = new FakeSspdDevice();
        device1->installFakeInterface(tmpInterface);
        device1->setAddress(1);
        device1->setLightOn(false);
        info = device1->deviceInfo();

        info->setModificationVersion("1");
        info->setHardwareVersion("2");
        info->setFirmwareVersion("0.1.2");

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        info->setDeviceUID(tmpUDID);
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
    server->setInterface(mInterface);
    server->StartServer();

    return a.exec();
}
