#include "tcpipaddressdialog.h"
#include "ui_tcpipaddressdialog.h"
#include "../qCustomLib/qCustomLib.h"

#include <QSettings>
#include <QSerialPortInfo>

TcpIpAddressDialog::TcpIpAddressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpIpAddressDialog)
{
    ui->setupUi(this);
    QSettings settings("Scontel", "cu-simpleapp");
    bool isSerialPort = settings.value("isSerialProtocol",true).toBool();
    ui->cbType->setCurrentIndex(isSerialPort ? 0 : 1);
    ui->leTcpIpAddress->setText(settings.value("TcpIpAddress","127.000.000.001").toString());

    QStringList availableSerials;
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        availableSerials.append(info.portName());
    }
    ui->cbSerial->addItems(availableSerials);
    QString lastPortName = settings.value("SerialPortName","ttyS0").toString();

    int idx = availableSerials.indexOf(lastPortName);
    if (idx >= 0) ui->cbSerial->setCurrentIndex(idx);

    on_cbType_activated(ui->cbType->currentIndex());
}

TcpIpAddressDialog::~TcpIpAddressDialog()
{
    delete ui;
}

QHostAddress TcpIpAddressDialog::getAddress()
{
    return convertToHostAddress(ui->leTcpIpAddress->text());
}

QString TcpIpAddressDialog::getPortName()
{
    return ui->cbSerial->currentText();
}

bool TcpIpAddressDialog::isSerialProtocol()
{
    return ui->cbType->currentIndex() == 1;
}

void TcpIpAddressDialog::accept()
{
    QSettings settings("Scontel", "cu-simpleapp");
    settings.setValue("isSerialProtocol", ui->cbType->currentIndex() == 0);
    settings.setValue("TcpIpAddress", ui->leTcpIpAddress->text());
    settings.setValue("SerialPortName", ui->cbSerial->currentText());

    QDialog::accept();
}

void TcpIpAddressDialog::on_cbType_activated(int index)
{
    bool isSerialPort = index == 1;
    ui->stackedWidget->setCurrentIndex(ui->cbType->currentIndex());
    ui->lbProtocolspecs->setText(isSerialPort ? "Port name:" : "TcpIp address:");
}
