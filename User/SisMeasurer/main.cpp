#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // TODO: сделать возможность выбора интерфейса
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
