#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "appcore.h"
#include <QDebug>
#include "tcpipvalidator.h"

#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setApplicationName("cu-simpleapp_qml");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple application standard monitor support for Scontel\'s control unit (CU4) NOW with QtQuick graphics interface");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption reconnectDisableOption(QStringList()<<"d"<<"disable",
                                              QCoreApplication::translate("main", "Disable reconnect button"));
    parser.addOption(reconnectDisableOption);


    QGuiApplication app(argc, argv);

    parser.process(app);

    qmlRegisterSingletonType<AppCore>("AppCore", 1, 0, "AppCore", appCoreProvider);
    qmlRegisterType<TcpIpValidator>("TcpIpValidator", 1, 0, "TcpIpValidator");

    AppCore * appcore = qobject_cast<AppCore*>(appCoreProvider(nullptr, nullptr));
    if (appcore){
        qDebug()<<!parser.isSet(reconnectDisableOption);
        appcore->setReconnectEnableFlag(!parser.isSet(reconnectDisableOption));
    }

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
