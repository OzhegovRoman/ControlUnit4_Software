#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>

#include "temperaturedata.h"
#include "appcore.h"
#include "tcpipvalidator.h"
#include "devicelist.h"
#include "devicemodel.h"
#include "sspddata.h"
#include "sspddatamodel.h"

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
    qmlRegisterUncreatableType<TemperatureData>("AppCore", 1, 0, "TemperatureData",
                                           QStringLiteral("TemperatureData should not be created in QML"));
    qmlRegisterType<SspdDataModel>("AppCore", 1, 0, "SspdDataModel");
    qmlRegisterUncreatableType<UnitData>("AppCore", 1, 0, "SspdData",
                                           QStringLiteral("SspdData should not be created in QML"));

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
    TemperatureData tempData;
    UnitData sspdData;
    engine.rootContext()->setContextProperty(QStringLiteral("deviceList"), &devList);
    engine.rootContext()->setContextProperty(QStringLiteral("temperatureData"), &tempData);
    engine.rootContext()->setContextProperty(QStringLiteral("sspdData"), &sspdData);

    engine.load(url);

    return app.exec();
}
