#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>

#include "displaythread.h"

#include <initializer_list>
#include <signal.h>
#include <unistd.h>

cDisplay *dThread;

void ignoreUnixSignals(std::initializer_list<int> ignoreSignals) {
    for (int sig : ignoreSignals)
        signal(sig, SIG_IGN);
}

void catchUnixSignals(std::initializer_list<int> quitSignals) {
    auto handler = [](int sig) -> void {
        // blocking and not aysnc-signal-safe func are valid
        printf("\nquit the application by signal(%d).\n", sig);
        emit dThread->quitSignal();
        while (!dThread->isFinished())
            qApp->processEvents();
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

    dThread = new cDisplay();

    if (parser.isSet(tcpIpOption)) {
        dThread->setTcpIpProtocolEnabled(true);
        dThread->setTcpIpAddress(parser.value(tcpIpOption));
    }
    else {
        if (parser.isSet(serialPortOption)){
            dThread->setRs485ProtocolEnabled(true);
            dThread->setRs485PortName(parser.value(serialPortOption));
        }
        else{
            dThread->setTcpIpProtocolEnabled(true);
            dThread->setTcpIpAddress("127.0.0.1");
        }
    }

    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});

    dThread->initialize();

    return a.exec();
}

