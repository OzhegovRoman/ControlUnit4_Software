#include <QCoreApplication>
#include <QCommandLineParser>

#include <QObject>
#include "Interfaces/cutcpsocketiointerface.h"
#include "Interfaces/curs485iointerface.h"
#include "../qCustomLib/qCustomLib.h"
#include "Server/servercommands.h"
#include "Drivers_V2/commondriver.h"

enum testType{
    TT_Type,
    TT_Description,
    TT_Firmware,
    TT_Hardware,
    TT_Modification
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("cu-devboot");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("Interface test utility for Scontel\'s control unit (CU4)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption serialPortOption(QStringList()<<"s"<<"serial",
                                        QCoreApplication::translate("main", "specify serial port name. Default name is ttyS0."),
                                        QCoreApplication::translate("main", "serial port name"));
    parser.addOption(serialPortOption);

    QCommandLineOption tcpIpOption(QStringList()<<"t"<<"tcpip",
                                   QCoreApplication::translate("main", "use TCPIP protocol like main protokol. "
                                                                       "This option is stronger than the -s option"),
                                   QCoreApplication::translate("main", "TCPIP address"));
    parser.addOption(tcpIpOption);


    QCommandLineOption countOption(QStringList()<<"n",
                                  QCoreApplication::translate("main", "sets the number of attempts. Default value: 10."),
                                  QCoreApplication::translate("main","attempts count"));
    parser.addOption(countOption);

    QCommandLineOption addressOption(QStringList()<<"a"<<"address",
                                     QCoreApplication::translate("main", "device address. Default addres: 0."),
                                     QCoreApplication::translate("main","address"));
    parser.addOption(addressOption);

    QCommandLineOption descriptionOption(QStringList()<<"d"<<"desc",
                                   QCoreApplication::translate("main", "Trying to get a module description. "
                                                               "Without specifying this option, the test is conducted on obtaining "
                                                               "the type of module. This option is stronger than the options -f, -h, -m. "
                                                               "The latter, if available, are ignored"));
    parser.addOption(descriptionOption);

    QCommandLineOption firmwareOption(QStringList()<<"f"<<"firm",
                                   QCoreApplication::translate("main", "Trying to get the module firmware version. "
                                                               "Without specifying this option, the test is conducted on obtaining "
                                                               "the type of module. This option is stronger than the options -h, -m. "
                                                               "The latter, if available, are ignored"));
    parser.addOption(firmwareOption);

    QCommandLineOption hardwareOption(QStringList()<<"h"<<"hard",
                                   QCoreApplication::translate("main", "Trying to get the module hardware version. "
                                                               "Without specifying this option, the test is conducted on obtaining "
                                                               "the type of module. This option is stronger than the options -m. "
                                                               "The latter, if available, are ignored"));
    parser.addOption(hardwareOption);

    QCommandLineOption modificationOption(QStringList()<<"m"<<"mod",
                                   QCoreApplication::translate("main", "Trying to get the module modification. "
                                                               "Without specifying this option, the test is conducted on obtaining "
                                                               "the type of module. "));
    parser.addOption(modificationOption);

    parser.process(a);

    testType mTestType = TT_Type;
    if (parser.isSet(descriptionOption))
        mTestType = TT_Description;
    if (parser.isSet(firmwareOption) && mTestType == TT_Type)
        mTestType = TT_Firmware;
    if (parser.isSet(hardwareOption) && mTestType == TT_Type)
        mTestType = TT_Hardware;
    if (parser.isSet(modificationOption) && mTestType == TT_Type)
        mTestType = TT_Modification;

    cuIOInterfaceImpl *mInterface = nullptr;
    if (parser.isSet(tcpIpOption) || (!parser.isSet(serialPortOption))){
        cuTcpSocketIOInterface *pInterface = new cuTcpSocketIOInterface();
        pInterface->setAddress(convertToHostAddress(parser.isSet(tcpIpOption)
                                                    ? parser.value(tcpIpOption)
                                                    : "127.0.0.1"));
        pInterface->setPort(SERVER_TCPIP_PORT);
        mInterface = pInterface;
    }
    else {
        cuRs485IOInterface *pInterface = new cuRs485IOInterface();
        pInterface->setPortName(parser.value(serialPortOption));
        mInterface = pInterface;
    }

    int n = 10;
    if (parser.isSet(countOption)){
        bool ok;
        n = parser.value(countOption).toInt(&ok);
        if (!ok) n = 10;
    }

    int address = 0;
    if (parser.isSet(addressOption)){
        bool ok;
        address = parser.value(addressOption).toInt(&ok);
        if (!ok) address = 0;
        if (address > 31 || address < 0) address = 0;
    }

    CommonDriver driver;
    if (!mInterface)
        return 1;

    driver.setIOInterface(mInterface);
    driver.setDevAddress(static_cast<quint8>(address));

    int successAtempts = 0;
    for (int i = 0; i < n; i++){
        fprintf(stderr, QString("\rProgress: %1/%2: errors: %3").arg(i).arg(n).arg(i-successAtempts).toLatin1().data());
        bool ok = false;
        switch (mTestType) {
        case TT_Type:
            driver.deviceType()->getValueSync(&ok);
            break;
        case TT_Description:
            driver.deviceDescription()->getValueSync(&ok);
            break;
        case TT_Firmware:
            driver.firmwareVersion()->getValueSync(&ok);
            break;
        case TT_Hardware:
            driver.hardwareVersion()->getValueSync(&ok);
            break;
        case TT_Modification:
            driver.modificationVersion()->getValueSync(&ok);
            break;
        }
        if (ok) successAtempts++;
    }

    fprintf(stderr, QString("\rProgress: %1/%1: errors: %2%%\r\n")
            .arg(n)
            .arg(static_cast<double>(n-successAtempts)*100.0/static_cast<double>(n),3,'f',2)
            .toLatin1().data());

    return 0;
}
