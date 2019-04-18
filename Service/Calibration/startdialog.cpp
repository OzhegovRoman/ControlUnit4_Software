#include "startdialog.h"
#include "ui_startdialog.h"
#include <QSettings>
#include "Interfaces/cutcpsocketiointerface.h"
#include "Drivers/adriver.h"
#include "Server/servercommands.h"
#include "QDebug"
#include <QMessageBox>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QSerialPortInfo>
#include "Interfaces/curs485iointerface.h"
#include "../qCustomLib/qCustomLib.h"


StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog),
    mInterface(0)
{
    ui->setupUi(this);

    QSettings settings("Scontel", "ControlUnit4_Calibration");
    ui->cbInterface->setCurrentIndex(settings.value("Protocol", 0).toInt());
    ui->leTcpIpAddress->setText(settings.value("TcpIpAddress","127.000.000.001").toString());
    ui->sbDeviceAddress->setValue(settings.value("DeviceAddress",0).toInt());

    on_cbInterface_activated(ui->cbInterface->currentIndex());

    QStringList availableSerials;
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        availableSerials.append(info.portName());
    }
    ui->cbSerial->addItems(availableSerials);
    QString lastPortName = settings.value("SerialPortName","ttyS0").toString();

    int idx = availableSerials.indexOf(lastPortName);
    if (idx >= 0) ui->cbSerial->setCurrentIndex(idx);

}

StartDialog::~StartDialog()
{
    delete ui;
}

void StartDialog::on_buttonBox_accepted()
{
    // проверка связи
    setEnabled(false);

    qApp->processEvents();
    bool ok;

    // создаем правильный интерфейс
    switch (ui->cbInterface->currentIndex()) {
    case 0:
    {
        // TcpIp
        cuTcpSocketIOInterface *tmpInterface;
        tmpInterface = new cuTcpSocketIOInterface(this);
        tmpInterface->initialize();

        tmpInterface->setAddress(convertToHostAddress(ui->leTcpIpAddress->text()));
        tmpInterface->setPort(SERVER_TCPIP_PORT);

        mInterface = tmpInterface;

        QString answer = tmpInterface->tcpIpQuery("*IDN?", 100, &ok);
        if (!((ok)&&(answer.contains("Scontel ControlUnit")))){
            QMessageBox::warning(this,"Warning", "Can't find any device on this TcpIp address");
            setEnabled(true);
            return;
        }

        break;
    }
    case 1:
    {
        // Rs485
        cuRs485IOInterface *tmpInterface = new cuRs485IOInterface(this);
        tmpInterface->setPortName(ui->cbSerial->currentText());

        mInterface = tmpInterface;

        break;
    }
    default:
        // ой беда,
        reject();
        return;
        break;
    }

    // Далее определяем тип подключенного устройства
    AbstractDriver driver;
    driver.setIOInterface(mInterface);
    driver.setDevAddress(ui->sbDeviceAddress->value());
    mDeviceType = driver.getDeviceType()->getValueSequence(&ok);
    if (!ok || mDeviceType.isEmpty()){
        QMessageBox::warning(this,"Warning", "Wrong device address");
        setEnabled(true);
        return;
    }

    qDebug()<<mDeviceType;

    // если все в порядке, сохраняем текущие данные и закрываем окошко
    QSettings settings("Scontel", "ControlUnit4_Calibration");
    settings.setValue("Protocol",ui->cbInterface->currentIndex());
    settings.setValue("TcpIpAddress", ui->leTcpIpAddress->text());
    settings.setValue("DeviceAddress", ui->sbDeviceAddress->value());
    settings.setValue("SerialPortName", ui->cbSerial->currentText());
    accept();
}

QString StartDialog::deviceType() const
{
    return mDeviceType;
}

int StartDialog::deviceAddress() const
{
    return ui->sbDeviceAddress->value();
}

cuIOInterfaceImpl *StartDialog::interface() const
{
    return mInterface;
}

void StartDialog::on_cbInterface_activated(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    ui->lbProtocolspec->setText(index == 0 ? "Tcp/Ip address: " : "Port name:" );
}
