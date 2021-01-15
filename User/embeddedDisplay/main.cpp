#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <initializer_list>
#include <signal.h>
#include <unistd.h>

#include "compactdraw.h"
#include "FTWidgets/widgets.h"
#include "DataHarvester/dataharvester.h"

#include "Interfaces/curs485iointerface.h"
#include "Interfaces/cutcpsocketiointerface.h"

#include "servercommands.h"
#include "../qCustomLib/qCustomLib.h"

#include "riverdieve.h"

static Gpu_Hal_Context_t host;

void ignoreUnixSignals(std::initializer_list<int> ignoreSignals) {
    for (int sig : ignoreSignals)
        signal(sig, SIG_IGN);
}

void catchUnixSignals(std::initializer_list<int> quitSignals) {
    auto handler = [](int sig) -> void {
        // blocking and not aysnc-signal-safe func are valid
        printf("\nquit the application by signal(%d).\n", sig);

        TurnOffWidget::turnOff(&host);
        QCoreApplication::quit();

    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig : quitSignals)
        sigaction(sig, &sa, nullptr);
}

cuIOInterfaceImpl *createInterface(const QCommandLineParser & parser,
                                   const QCommandLineOption & tcpIpOption,
                                   const QCommandLineOption & serialPortOption)
{
    qDebug()<<"create interface";
    bool tcpipProtocolEnabled = true;
    QString tcpipAddress = QString("127.0.0.1");
    QString rs485PortName;


    if (parser.isSet(tcpIpOption)) {
        tcpipAddress = parser.value(tcpIpOption);
    }
    else {
        if (parser.isSet(serialPortOption)){
            tcpipProtocolEnabled = false;
            rs485PortName = parser.value(serialPortOption);
        }
    }

    if (tcpipProtocolEnabled){
        cuTcpSocketIOInterface* interface = new cuTcpSocketIOInterface(nullptr);
        interface->setAddress(convertToHostAddress(tcpipAddress));
        interface->setPort(SERVER_TCPIP_PORT);

        return interface;
    }

    else {
        cuRs485IOInterface* interface = new cuRs485IOInterface(nullptr);
        interface->setPortName(rs485PortName);
        return  interface;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("cu-embeddeddisplay");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple application with embedded display support for Scontel\'s control unit (CU4)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption serialPortOption(QStringList()<<"s"<<"serial",
                                        QCoreApplication::translate("main", "use serial port like main protocol"),
                                        QCoreApplication::translate("main", "serial port name"));
    parser.addOption(serialPortOption);

    QCommandLineOption tcpIpOption(QStringList()<<"t"<<"tcpip",
                                   QCoreApplication::translate("main", "use TCPIP protocol like main protokol. "
                                                                       "This option is stronger than the -s option"),
                                   QCoreApplication::translate("main", "TCPIP address"));
    parser.addOption(tcpIpOption);

    parser.process(a);

    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});

    qDebug()<< "init interface";

    DataHarvester *_DataHarvester = new DataHarvester();
    _DataHarvester->setInterface(createInterface(parser, tcpIpOption, serialPortOption));

    // TODO: Установить все параметры программы

    DisplayInitializer _DisplayInitializer(&host);

    MainWidget _MainWidget(&host, _DataHarvester);
    SystemInfo _SystemInfo(&host);
    TempWidget _TempWidget(&host);
    TempM1Widget _TempM1Widget(&host);
    SspdWidget _SspdWidget(&host);
    SspdWidgetM1 _SspdWidgetM1(&host);
    WelcomePageWidget _WelcomePage(&host);
    CD::mHost = &host;

    QObject::connect(&_DisplayInitializer, &DisplayInitializer::initialized, &_WelcomePage, &WelcomePageWidget::exec);


    QObject::connect(&_WelcomePage, &WelcomePageWidget::done, [&_MainWidget, &_DataHarvester](){
        _DataHarvester->initialize();
        _MainWidget.exec();
    });

    QObject::connect(&_MainWidget, &MainWidget::systemInfoClicked, &_SystemInfo, &SystemInfo::exec);
    QObject::connect(&_MainWidget, &MainWidget::channelChoosen, [&_DataHarvester, &_TempWidget, &_TempM1Widget, &_SspdWidget, &_SspdWidgetM1](const int index){
        qDebug()<<"index"<<index;
        {
            auto tempDriver = qobject_cast<TempDriverM0*>(_DataHarvester->drivers()[index]);
            if (tempDriver){
                _TempWidget.setDriver(tempDriver);
                _TempWidget.exec();
            }
        }
        {
            auto tempDriver = qobject_cast<TempDriverM1*>(_DataHarvester->drivers()[index]);
            if (tempDriver){
                _TempM1Widget.setDriver(tempDriver);
                _TempM1Widget.exec();
            }
        }
        {
            auto sspdDriver = qobject_cast<SspdDriverM0*>(_DataHarvester->drivers()[index]);
            if (sspdDriver){
                _SspdWidget.setDriver(sspdDriver);
                _SspdWidget.exec();
            }
        }
        {
            auto sspdDriver = qobject_cast<SspdDriverM1*>(_DataHarvester->drivers()[index]);
            if (sspdDriver){
                _SspdWidgetM1.setDriver(sspdDriver);
                _SspdWidgetM1.exec();
            }
        }
    });

    QObject::connect(&_SystemInfo, &SystemInfo::backClicked, &_MainWidget, &MainWidget::exec);
    QObject::connect(&_TempWidget, &TempWidget::backClicked, &_MainWidget, &MainWidget::exec);
    QObject::connect(&_TempM1Widget, &TempM1Widget::backClicked, &_MainWidget, &MainWidget::exec);
    QObject::connect(&_SspdWidget, &SspdWidget::backClicked, &_MainWidget, &MainWidget::exec);
    QObject::connect(&_SspdWidgetM1, &SspdWidgetM1::backClicked, &_MainWidget, &MainWidget::exec);

    _DisplayInitializer.exec();

    return a.exec();
}

