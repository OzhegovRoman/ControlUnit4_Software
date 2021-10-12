#include "maindialog.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QThread>
#include <QStyle>
#include <QDesktopWidget>

#include "tcpipaddressdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setCursorFlashTime(1000);

    QCoreApplication::setApplicationName("cu-simpleapp");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple application standard monitor support for Scontel\'s control unit (CU4)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption serialPortOption(QStringList()<<"s"<<"serial",
                                        QCoreApplication::translate("main", "use serial interface like main"),
                                        QCoreApplication::translate("main", "serial port name"));
    parser.addOption(serialPortOption);

    QCommandLineOption tcpIpOption(QStringList()<<"t"<<"tcpip",
                                   QCoreApplication::translate("main", "use TCPIP interface like main. "
                                                                       "This option is stronger than the -s option"),
                                   QCoreApplication::translate("main", "TCPIP address"));
    parser.addOption(tcpIpOption);

    QCommandLineOption welcomeOption(QStringList()<<"w"<<"welcome",
                                     QCoreApplication::translate("main", "use Welcome screen at start up. "));
    parser.addOption(welcomeOption);

    parser.process(a);

    if (parser.isSet(welcomeOption)){
        QGraphicsScene scene;
        QGraphicsView view(&scene);
        QPixmap pixmap(":/images/splash.png");
        pixmap = pixmap.scaledToWidth(view.width());
        QGraphicsPixmapItem item(pixmap);
        scene.addItem(&item);
//        view.setWindowFlag(Qt::Window, true);
        view.setWindowFlag(Qt::FramelessWindowHint, true);
//        view.setFrameShape(QFrame::NoFrame);
        view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view.setGeometry(
                    QStyle::alignedRect(
                        Qt::LeftToRight,
                        Qt::AlignCenter,
                        pixmap.size(),
                        qApp->desktop()->availableGeometry()
                        )
                    );
        view.show();
        QElapsedTimer timer;
        timer.start();
        while (timer.elapsed()<2000)
            qApp->processEvents();
    }

    MainDialog w2;
    if (parser.isSet(tcpIpOption)) {
        w2.setTcpIpProtocolEnable(true);
        w2.setTcpIpAddress(parser.value(tcpIpOption));
    }
    else {
        if (parser.isSet(serialPortOption)){
            w2.setSerialProtocolEnable(true);
            w2.setSerialPortName(parser.value(serialPortOption));
        }
        else{
            // вызываем окно установки параметров
            TcpIpAddressDialog w;
            if (w.exec() == QDialog::Accepted) {
                w2.setSerialProtocolEnable(w.isSerialProtocol());
                w2.setSerialPortName(w.getPortName());
                w2.setTcpIpAddress(w.getAddress().toString());
            }
            else return 0;
        }
    }
    w2.show();
    if (!w2.initialize()) return 1;
    return a.exec();
}
