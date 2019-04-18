#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "appcore.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType<AppCore>("AppCore", 1, 0, "AppCore", appCoreProvider);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
           return -1;
    return app.exec();
}
