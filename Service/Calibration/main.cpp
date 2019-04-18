#include "startdialog.h"
#include "maindialog.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StartDialog w;
    MainDialog w2;
    if (w.exec()) {
        w2.setInterface(w.interface());
        w2.setDeviceType(w.deviceType());
        w2.setDeviceAddress(w.deviceAddress());
        w2.initializeUI();
        w2.show();
        return a.exec();
    }
    else
        return 0;
}