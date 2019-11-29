#include "tcpipaddressdialog.h"
#include "ui_tcpipaddressdialog.h"
#include "../qCustomLib/qCustomLib.h"

#include <QSettings>
#include <QSerialPortInfo>
#include <QtNetwork>
#include <QNetworkProxy>
#include <QNetworkInterface>

#include "Server/servercommands.h"

TcpIpAddressDialog::TcpIpAddressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpIpAddressDialog)
{
    ui->setupUi(this);

    QSettings settings("Scontel", "cu-simpleapp");
    bool isSerialPort = settings.value("isSerialProtocol",true).toBool();
    ui->cbType->setCurrentIndex(isSerialPort ? 0 : 1);

    QStringList availableSerials;
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        availableSerials.append(info.portName());
    }
    ui->cbSerial->addItems(availableSerials);
    QString lastPortName = settings.value("SerialPortName","ttyS0").toString();

    int idx = availableSerials.indexOf(lastPortName);
    if (idx >= 0) ui->cbSerial->setCurrentIndex(idx);

    on_cbType_activated(ui->cbType->currentIndex());

    QStringList list = availableControlUnits();
    ui->cbTcpIp->clear();
    ui->cbTcpIp->addItems(list);
    QString LastTcpIpAddress = settings.value("TcpIpAddress","127.000.000.001").toString();
    int i = list.indexOf(LastTcpIpAddress);
    if (i>=0) ui->cbTcpIp->setCurrentIndex(i);
}

TcpIpAddressDialog::~TcpIpAddressDialog()
{
    delete ui;
}

QHostAddress TcpIpAddressDialog::getAddress()
{
    return convertToHostAddress(ui->cbTcpIp->currentText());
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
    settings.setValue("TcpIpAddress", ui->cbTcpIp->currentText());
    settings.setValue("SerialPortName", ui->cbSerial->currentText());

    QDialog::accept();
}

void TcpIpAddressDialog::on_cbType_activated(int index)
{
    bool isSerialPort = index == 1;
    ui->stackedWidget->setCurrentIndex(ui->cbType->currentIndex());
    ui->lbProtocolspecs->setText(isSerialPort ? "Port name:" : "TcpIp address:");
}
