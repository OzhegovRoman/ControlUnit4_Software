#include "dialog.h"
#include "ui_dialog.h"
#include "QFileDialog"
#include "QFileInfo"
#include "comportdialog.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

#include <QSettings>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    mBootloader(new Bootloader(this))
{
    ui->setupUi(this);
    connect(mBootloader, SIGNAL(loaderProgressChanged(int)), ui->pbProgress, SLOT(setValue(int)));
    connect(mBootloader, SIGNAL(loaderFinished()), this, SLOT(controlEnable()));
    connect(mBootloader, SIGNAL(errorLoader(QString)), this, SLOT(error(QString)));
    connect(mBootloader, SIGNAL(infoLoader(QString)), this, SLOT(info(QString)));

    ComPortDialog dialog;
    if (dialog.exec() == QDialog::Rejected) return;
    mBootloader->setPortName(dialog.portName());

    QSettings settings("Scontel","BootloaderSettings");
    ui->leFileName->setText(settings.value("FileName",QString()).toString());
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_tbFileName_clicked()
{
    QString str = QFileDialog::getOpenFileName(this,
                                               "Open File...",
                                               ui->leFileName->text(),
                                               "Binary Files (*.bin)");
    if (str.isEmpty()) return;
    ui->leFileName->setText(str);
}

void Dialog::on_pbProgram_clicked()
{
    QString str = ui->leFileName->text();
    QFileInfo info(str);
    /// получается двойная проверка файла,
    /// но лучше уж так, чем запускать автозагрузчик на пустом файле.
    if (!info.exists()) return; // файла такого нет

    QSettings settings("Scontel","BootloaderSettings");
    settings.setValue("FileName", str);

    controlEnable(false);
    mBootloader->program(str);
}

void Dialog::controlEnable(bool value)
{
    ui->leFileName->setEnabled(value);
    ui->pbProgram->setEnabled(value);
    ui->pbClose->setEnabled(value);
}

void Dialog::error(QString value)
{
    qDebug()<<"error"<<value;
    ui->teInfo->insertHtml(QString("<font color=\"Red\"> Error: %1<br>").arg(value));
}

void Dialog::info(QString value)
{
     qDebug()<<"info"<<value;
    ui->teInfo->insertHtml(QString("<font color=\"Blue\"> %1<br>").arg(value));
}
