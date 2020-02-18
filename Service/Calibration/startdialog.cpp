#include "startdialog.h"
#include "ui_startdialog.h"
#include <QSettings>
#include "Interfaces/cutcpsocketiointerface.h"
#include "Drivers_V2/commondriver.h"
#include "Server/servercommands.h"
#include "QDebug"
#include <QMessageBox>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QInputDialog>
#include "../qCustomLib/qCustomLib.h"

StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog)

{
    ui->setupUi(this);

    QSettings settings("Scontel", "ControlUnit4_Calibration");
    ui->sbDeviceAddress->setValue(settings.value("DeviceAddress",0).toInt());

    updateControlUnitList();
    QString LastTcpIpAddress = settings.value("TcpIpAddress","127.000.000.001").toString();
    for (int i = 0; i < ui->cbTcpIp->count(); i++){
        if (ui->cbTcpIp->itemText(i) == LastTcpIpAddress){
            ui->cbTcpIp->setCurrentIndex(i);
            break;
        }
    }

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
    // TcpIp
    cuTcpSocketIOInterface *tmpInterface;
    tmpInterface = new cuTcpSocketIOInterface(this);
    tmpInterface->initialize();

    tmpInterface->setAddress(convertToHostAddress(ui->cbTcpIp->currentText()));
    tmpInterface->setPort(SERVER_TCPIP_PORT);

    mInterface = tmpInterface;

    QString answer = tmpInterface->tcpIpQuery("*IDN?", 100, &ok);
    if (!((ok)&&(answer.contains("Scontel ControlUnit")))){
        QMessageBox::warning(this,"Warning", "Can't find any device on this TcpIp address");
        setEnabled(true);
        return;
    }

    // Далее определяем тип подключенного устройства
    CommonDriver driver;
    driver.setIOInterface(mInterface);
    driver.setDevAddress(static_cast<quint8>(ui->sbDeviceAddress->value()));
    mDeviceType = driver.deviceType()->getValueSync(&ok, 5);
    if (!ok || mDeviceType.isEmpty()){
        QMessageBox::warning(this,"Warning", "Wrong device address");
        setEnabled(true);
        return;
    }

    qDebug()<<mDeviceType;

    // если все в порядке, сохраняем текущие данные и закрываем окошко
    QSettings settings("Scontel", "ControlUnit4_Calibration");
    settings.setValue("TcpIpAddress", ui->cbTcpIp->currentText());
    settings.setValue("DeviceAddress", ui->sbDeviceAddress->value());
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

void StartDialog::updateControlUnitList()
{
    setDisabled(true);
    qApp->processEvents();
    QStringList list = availableControlUnits();
    ui->cbTcpIp->clear();
    ui->cbTcpIp->addItems(list);
    ui->cbTcpIp->addItems(QStringList()<<"Update..."<<"Manual...");
    setEnabled(true);
}

void StartDialog::on_cbTcpIp_activated(const QString &arg1)
{
    if (arg1.contains("Update")){
        updateControlUnitList();
        ui->cbTcpIp->setCurrentIndex(0);
    }
    else if (arg1.contains("Manual")){

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
