#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //qApp->addLibraryPath("c:/Qt/Static/5.7.0/plugins/");
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
