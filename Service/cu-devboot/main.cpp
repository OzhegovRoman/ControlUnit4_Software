#include <QCoreApplication>
#include <QCommandLineParser>
#include <QMap>
#include <QStringList>
#include <QThread>
#include <QLoggingCategory>
#include <QFile>

#include <QObject>
#include "cdevboot.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("cu-devboot");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    QCommandLineParser parser;
    parser.setApplicationDescription("Bootloader for device update via internet or with local binary for Scontel\'s control unit (CU4)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption serialPortOption(QStringList()<<"s"<<"serial",
                                        QCoreApplication::translate("main", "specify serial port name. Default name is ttyS0"),
                                        QCoreApplication::translate("main", "serial port name"));
    parser.addOption(serialPortOption);

    QCommandLineOption typeOption(QStringList()<<"t"<<"type",
                                  QCoreApplication::translate("main", "Device type. Without this options was using autodefinition of device type"),
                                  QCoreApplication::translate("main","type"));
    parser.addOption(typeOption);

    QCommandLineOption indexOption(QStringList()<<"i"<<"index",
                                   QCoreApplication::translate("main", "Index of firmware file [firmware version]"),
                                   QCoreApplication::translate("main","version"));
    parser.addOption(indexOption);

    QCommandLineOption addressOption(QStringList()<<"a"<<"address",
                                     QCoreApplication::translate("main", "Device address. All other devices will be mute. Default adderss: 0."),
                                     QCoreApplication::translate("main","address"));
    parser.addOption(addressOption);

    QCommandLineOption forceOption(QStringList()<<"f"<<"force",
                                   QCoreApplication::translate("main", "Force option. Your device will be updated regardless of the current firmware version of the device."));
    parser.addOption(forceOption);

    QCommandLineOption binOption(QStringList()<<"b"<<"bin",
                                 QCoreApplication::translate("main", "Local file for updating device. This option excludes downloading via the Internet. "
                                                                     "Options -i, -t will be ignored."),
                                 QCoreApplication::translate("main","FileName"));
    parser.addOption(binOption);
    QCommandLineOption urlOption(QStringList()<<"u"<<"url",
                                 QCoreApplication::translate("main", "URL for repo. Default url: [http://rplab.ru/~ozhegov/ControlUnit4/Bin/Firmware/]"),
                                 QCoreApplication::translate("main","http://url"));
    parser.addOption(urlOption);

    QCommandLineOption hotPlugOption(QStringList()<<"H"<<"hot",
                                     QCoreApplication::translate("main", "Hot plug option. In this case device must be connected to ControlUnit when program will be processed. "
                                                                         "This option include force option (-f)"));
    parser.addOption(hotPlugOption);

    parser.process(a);

    cDevBoot devBoot;

    // serial port options
    if (parser.isSet(serialPortOption))
        devBoot.setPortName(parser.value(serialPortOption));

    // device address
    if (parser.isSet(addressOption))
        devBoot.setAddress(parser.value(addressOption).toInt());

    devBoot.setLoadFromURL(!parser.isSet(binOption));

    // local fileName
    if (!devBoot.isLoadFromURL())
        devBoot.setFileName(parser.value(binOption));


    if (parser.isSet(urlOption))
        devBoot.setUrl(parser.value(urlOption));

    devBoot.setForce(parser.isSet(forceOption) || parser.isSet(hotPlugOption));
    devBoot.setHotPlug(parser.isSet(hotPlugOption));

    if (parser.isSet(typeOption))
        devBoot.setDevType(parser.value(typeOption));

    if (parser.isSet(indexOption))
        devBoot.setFwVersion(parser.value(indexOption));

    devBoot.startProcess();

    return a.exec();
}
