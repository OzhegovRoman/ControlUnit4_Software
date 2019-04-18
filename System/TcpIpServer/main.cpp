#include <QCoreApplication>
#include <QDebug>
#include "Server/cuserver.h"

#include "Interfaces/curs485iointerface.h"

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

    cuServer *server = new cuServer();
    qDebug()<<"Start Server";

    cuRs485IOInterface *mInterface = new cuRs485IOInterface();
    mInterface->setPortName("ttyS0");

    server->setIOInterface(mInterface);
    server->initialize();
    server->initializeDeviceList();

    return a.exec();
}
