#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "Bootloader/bootloader.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_tbFileName_clicked();
    void on_pbProgram_clicked();
    void controlEnable(bool value = true);
    void error(QString value);
    void info(QString value);

private:
    Ui::Dialog *ui;
    Bootloader *mBootloader;
};



#endif // DIALOG_H
