#include <QCoreApplication>
#include <QDebug>
#include "Server/cuserver.h"

#include "Interfaces/curs485iointerface.h"

#include <QCommandLineParser>

#ifdef FAKE_DEVICES
#include "../../../Libs/FakeDevicesLib/fakesspddevice.h"
#include "../../../Libs/FakeDevicesLib/fiointerface.h"
#endif

#ifdef DEBUG_LOG
#include <QFile>
#include <QTextStream>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString txt;
    switch (type) {
    case QtInfoMsg:
        txt = QString("Information: %1 (%2:%3, %4)\n")
                .arg(localMsg.constData())
                .arg(context.file)
                .arg(context.line)
                .arg(context.function);
        printf(QString("%1\n").arg(msg).toLocal8Bit());
        break;
    case QtDebugMsg:
        txt = QString("Debug: %1 (%2:%3, %4)\n")
                .arg(localMsg.constData())
                .arg(context.file)
                .arg(context.line)
                .arg(context.function);
        printf(QString("%1\n").arg(msg).toLocal8Bit());
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1 (%2:%3, %4)\n")
                .arg(localMsg.constData())
                .arg(context.file)
                .arg(context.line)
                .arg(context.function);
        printf(QString("%1\n").arg(msg).toLocal8Bit());
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1 (%2:%3, %4)\n")
                .arg(localMsg.constData())
                .arg(context.file)
                .arg(context.line)
                .arg(context.function);
        printf(QString("%1\n").arg(msg).toLocal8Bit());
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1 (%2:%3, %4)\n")
                .arg(localMsg.constData())
                .arg(context.file)
                .arg(context.line)
                .arg(context.function);
        printf(QString("%1\n").arg(msg).toLocal8Bit());
        break;
    }
    QFile outFile("log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}
#endif

int main(int argc, char *argv[])
{
#ifdef DEBUG_LOG
    qInstallMessageHandler(myMessageOutput);
#endif

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

    cuServer *server = new cuServer();
    qDebug()<<"Start Server";


#ifdef FAKE_DEVICES
    if (parser.isSet(fakeOptions)) {
        FakeIOInterface *mInterface = new FakeIOInterface();
        FakeSspdDevice *device0 = new FakeSspdDevice();
        device0->installFakeInterface(mInterface);

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
        device1->installFakeInterface(mInterface);
        device1->setAddress(1);
        device1->setLightOn(false);
        info = device1->deviceInfo();

        info->setModificationVersion("1");
        info->setHardwareVersion("2");
        info->setFirmwareVersion("0.1.2");

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        info->setDeviceUID(tmpUDID);

        FakeSspdDevice *device2 = new FakeSspdDevice();
        device2->installFakeInterface(mInterface);
        device2->setAddress(2);
        device2->setLightOn(false);
        info = device2->deviceInfo();

        info->setModificationVersion("1");
        info->setHardwareVersion("2");
        info->setFirmwareVersion("0.1.2");

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        info->setDeviceUID(tmpUDID);

        FakeSspdDevice *device3 = new FakeSspdDevice();
        device3->installFakeInterface(mInterface);
        device3->setAddress(3);
        device3->setLightOn(false);
        info = device3->deviceInfo();

        info->setModificationVersion("1");
        info->setHardwareVersion("2");
        info->setFirmwareVersion("0.1.2");

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        info->setDeviceUID(tmpUDID);

        FakeSspdDevice *device4 = new FakeSspdDevice();
        device4->installFakeInterface(mInterface);
        device4->setAddress(4);
        device4->setLightOn(false);
        info = device4->deviceInfo();

        info->setModificationVersion("1");
        info->setHardwareVersion("2");
        info->setFirmwareVersion("0.1.2");

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        info->setDeviceUID(tmpUDID);
        FakeSspdDevice *device5 = new FakeSspdDevice();
        device5->installFakeInterface(mInterface);
        device5->setAddress(5);
        device5->setLightOn(false);
        info = device5->deviceInfo();

        info->setModificationVersion("1");
        info->setHardwareVersion("2");
        info->setFirmwareVersion("0.1.2");

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        info->setDeviceUID(tmpUDID);

        FakeSspdDevice *device6 = new FakeSspdDevice();
        device6->installFakeInterface(mInterface);
        device6->setAddress(6);
        device6->setLightOn(false);
        info = device6->deviceInfo();

        info->setModificationVersion("1");
        info->setHardwareVersion("2");
        info->setFirmwareVersion("0.1.2");

        for (quint8 i = 0; i< 12; i++)
            tmpUDID[i] = 0x01*i+1;
        info->setDeviceUID(tmpUDID);

        server->setIOInterface(mInterface);
    }
    else {
#endif
        QString portName = "ttyS0";
        if (parser.isSet(serialPortOption)){
            portName = parser.value(serialPortOption);
        }
        cuRs485IOInterface *mInterface = new cuRs485IOInterface();
        mInterface->setPortName(portName);
        server->setIOInterface(mInterface);
#ifdef FAKE_DEVICES
    }
#endif

    server->initialize();
    server->initializeDeviceList();

    return a.exec();
}
