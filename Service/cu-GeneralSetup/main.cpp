#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>

#include "appcore.h"
#include "guiitems.h"


int main(int argc, char *argv[])
   {
   QGuiApplication app(argc, argv);

   QQmlApplicationEngine engine;
   const QUrl url(QStringLiteral("qrc:/main.qml"));
   QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                    &app, [url](QObject *obj, const QUrl &objUrl) {
       if (!obj && url == objUrl)
           QCoreApplication::exit(-1);
   }, Qt::QueuedConnection);

//   DeviceList devList;
   GuiItems guiItems;;
//   engine.rootContext()->setContextProperty(QStringLiteral("deviceList"), &devList);
   engine.rootContext()->setContextProperty(QStringLiteral("guiItems"), &guiItems);

   engine.load(url);

   return app.exec();
   }
