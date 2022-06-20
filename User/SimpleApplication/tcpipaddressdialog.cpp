#include "tcpipaddressdialog.h"
#include "ui_tcpipaddressdialog.h"
#include "../qCustomLib/qCustomLib.h"

#include <QSettings>
#include <QSerialPortInfo>
#include <QtNetwork>
#include <QNetworkProxy>
#include <QNetworkInterface>
#include <QInputDialog>

#include "servercommands.h"

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

    updateControlUnitList();
    QString LastTcpIpAddress = settings.value("TcpIpAddress","127.000.000.001").toString();
    for (int i = 0; i < ui->cbTcpIp->count(); i++){
        if (ui->cbTcpIp->itemText(i) == LastTcpIpAddress){
            ui->cbTcpIp->setCurrentIndex(i);
            break;
        }
    }
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

void TcpIpAddressDialog::updateControlUnitList()
{
    setDisabled(true);
    qApp->processEvents();
    QStringList list = availableControlUnits();
    ui->cbTcpIp->clear();
    ui->cbTcpIp->addItems(list);
    ui->cbTcpIp->addItems(QStringList()<<tr("Update...")<<tr("Manual..."));
    setEnabled(true);
}

void TcpIpAddressDialog::on_cbType_activated(int index)
{
    bool isSerialPort = index == 1;
    ui->stackedWidget->setCurrentIndex(ui->cbType->currentIndex());
    ui->lbProtocolspecs->setText(isSerialPort ? tr("Port name:") : tr("TcpIp address:"));
}

void TcpIpAddressDialog::on_cbTcpIp_activated(const QString &arg1)
{
    if (arg1.contains(tr("Update"))){
        updateControlUnitList();
        ui->cbTcpIp->setCurrentIndex(0);
    }
    else if (arg1.contains(tr("Manual"))){

        QSettings settings("Scontel", "ControlUnit4_Calibration");
        QString LastTcpIpAddress = settings.value("TcpIpAddress","127.000.000.001").toString();

        bool ok;
        QString  str = QInputDialog::getText(this
                                             , "TcpIp Addresss..."
                                             , "Enter TcpIp Address manualy"
                                             , QLineEdit::Normal
                                             , LastTcpIpAddress
                                             , &ok);
        if (ok && !str.isEmpty()){
            updateControlUnitList();

            ok = false;
            for (int i = 0; i < ui->cbTcpIp->count(); i++){
                if (ui->cbTcpIp->itemText(i) == str){
                    ui->cbTcpIp->setCurrentIndex(i);
                    ok = true;
                    break;
                }
            }
            if (!ok){
                ui->cbTcpIp->insertItem(0, str);
                ui->cbTcpIp->setCurrentIndex(0);
            }
        }
    }
}
