#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>

#include "appcore.h"
#include "tcpipvalidator.h"
#include "devicelist.h"
#include "devicemodel.h"
#include "unitdata.h"
#include "unitdatamodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setApplicationName("cu-simpleapp_qml");
    //TODO: изменить версию софта
    QCoreApplication::setApplicationVersion(QString("1.1.6"));

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple application standard monitor support for Scontel\'s control unit (CU4) NOW with QtQuick graphics interface");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption reconnectDisableOption(QStringList()<<"d"<<"disable",
                                              QCoreApplication::translate("main", "Disable reconnect button"));
    parser.addOption(reconnectDisableOption);

    QGuiApplication app(argc, argv);

    parser.process(app);

    qmlRegisterType<AppCore>("AppCore", 1, 0, "AppCore");
    //    qmlRegisterSingletonType<AppCore>("AppCore", 1, 0, "AppCore", appCoreProvider);
    qmlRegisterType<TcpIpValidator>("TcpIpValidator", 1, 0, "TcpIpValidator");
    qmlRegisterType<DeviceModel>("AppCore", 1, 0, "DeviceModel");
    qmlRegisterUncreatableType<DeviceList>("AppCore", 1, 0, "DeviceList",
                                           QStringLiteral("DeviceList should not be created in QML"));
    qmlRegisterType<UnitDataModel>("AppCore", 1, 0, "UnitDataModel");
    qmlRegisterUncreatableType<UnitData>("AppCore", 1, 0, "UnitData",
                                           QStringLiteral("UnitData should not be created in QML"));

    qDebug()<<"reconnectEnableFlag"<<!parser.isSet(reconnectDisableOption);
    AppCore::setReconnectEnable(!parser.isSet(reconnectDisableOption));

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    DeviceList devList;
    UnitData unitData;
    engine.rootContext()->setContextProperty(QStringLiteral("deviceList"), &devList);
    engine.rootContext()->setContextProperty(QStringLiteral("unitData"), &unitData);

    engine.load(url);

    return app.exec();
}
