#include "comportdialog.h"
#include "ui_comportdialog.h"
#include <QSerialPortInfo>

ComPortDialog::ComPortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComPortDialog)
{
    ui->setupUi(this);
}

ComPortDialog::~ComPortDialog()
{
    delete ui;
}

QString ComPortDialog::portName()
{
    return ui->cbPortName->currentText();
}

int ComPortDialog::exec()
{
    // подготовим список доступных портов
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    ui->cbPortName->clear();
    foreach (QSerialPortInfo info, list) {
        ui->cbPortName->addItem(info.portName());
    }

    return QDialog::exec();
}
