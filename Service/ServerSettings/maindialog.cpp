#include "maindialog.h"
#include "ui_maindialog.h"
#include "Server/servercommands.h"
#include <QInputDialog>
#include <QSettings>
#include "../qCustomLib/qCustomLib.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog),
    mIOInterface(new cuTcpSocketIOInterface(this)),
    mWaiting4Answer(false),
    mTimeOut(false),
    mTimer(new QTimer(this)),
    mDebugTimer(new QElapsedTimer())

{
    ui->setupUi(this);
    mIOInterface->setPort(SERVER_TCPIP_PORT);
    connect(mIOInterface, SIGNAL(msgReceived(quint8,quint8,quint8,quint8*)),
            this, SLOT(msgReceived(quint8,quint8,quint8,quint8*)));

    mTimer->setSingleShot(true);
    mTimer->setInterval(1000);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
    QSettings settings("Scontel","ServerSettings");
    ui->leTcpIpAddress->setText(settings.value("TcpIpAddress",QString()).toString());

    //костыль
    mIOInterface->initialize();
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::on_pbConnect_clicked()
{
    // Connect to server
    mDebugTimer->start();
    QString str = ui->leTcpIpAddress->text();

    QHostAddress host = convertToHostAddress(str);
    mIOInterface->setAddress(host);
    qDebug()<<"Setted Address, Time:"<<mDebugTimer->elapsed();

    on_pbGetServerInfo_clicked();

    QSettings settings("Scontel","ServerSettings");
    settings.setValue("TcpIpAddress",  ui->leTcpIpAddress->text());
}

void MainDialog::on_pbGetServerInfo_clicked()
{
    // get Server Info
    qDebug()<<"on_pbGetServerInfo_clicked, Time:"<<mDebugTimer->elapsed();

    startReading();
    bool ok;
    qDebug()<<"tcpIpQuery, Time:"<<mDebugTimer->elapsed();
    QString answer = mIOInterface->tcpIpQuery("SYST:DEVL?", 100, &ok);

    ui->teLog->appendHtml(QString("Receive DeviceList: %1 ")
                          .arg(answer));
    qDebug()<<"setControlsEnabled, Time:"<<mDebugTimer->elapsed();
    setControlsEnabled(ok);

}

void MainDialog::on_pbSearchNewDevices_clicked()
{
    //search new Devices
    mIOInterface->sendMsg(SERVER_ADDRESS, CMD_SERVER_SEARCH_DEVICES, 0, nullptr);
    ui->teLog->appendHtml("Please waiting... it was take several minutes");
}

void MainDialog::on_pbAppendNewDevice_clicked()
{
    // append new Device
    bool ok;
    quint8 address = QInputDialog::getInt(this,
                                          "Append new device",
                                          "Device address",
                                          0,
                                          0, 255, 1,
                                          &ok);
    if (!ok) {
        ui->teLog->appendHtml("Error: wrong device address");
        return;
    }

    startReading();
    mIOInterface->sendMsg(SERVER_ADDRESS, CMD_SERVER_ADD_DEVICE, 1, &address);
    ui->teLog->appendHtml(QString("Try to add device with address: %1.").arg(address));
    if (!waiting4Answer())
        ui->teLog->appendHtml("ERROR!!!");
}

void MainDialog::on_pbUpdateDeviceList_clicked()
{
    // updateDeviceList
    startReading();
    mIOInterface->sendMsg(SERVER_ADDRESS, CMD_SERVER_SAVE_DEVICE_LIST, 0, nullptr);
    ui->teLog->appendHtml("Try to save device configuration");
    if (!waiting4Answer())
        ui->teLog->appendHtml("ERROR!!!");
}

void MainDialog::msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    Q_UNUSED(address)
    mTimer->stop();
    mWaiting4Answer = false;
    switch (command) {
    case CMD_SERVER_GET_DEVICE_LIST:
        ui->teLog->appendHtml(QString("Receive DeviceList: %1 ")
                              .arg(QString(QByteArray((const char*)data, dataLength))));
        setControlsEnabled(true);
        break;
    case CMD_SERVER_SEARCH_DEVICES:
        ui->teLog->appendHtml(QString("Was founded: %1 devices").arg(*data));
        break;
    case CMD_SERVER_ADD_DEVICE:
        if (*data)
            ui->teLog->appendHtml("Success");
        else
            ui->teLog->appendHtml("Failure");
        break;
    case CMD_SERVER_SAVE_DEVICE_LIST:
        if (*data)
            ui->teLog->appendHtml("Success");
        else
            ui->teLog->appendHtml("Failure");
        break;
    default:
        break;
    }
}

void MainDialog::timeOut()
{
    mTimeOut = true;
    ui->teLog->appendHtml("Error: timeOut");
}

void MainDialog::setControlsEnabled(bool value)
{
    ui->pbAppendNewDevice->setEnabled(value);
    ui->pbGetServerInfo->setEnabled(value);
    ui->pbSearchNewDevices->setEnabled(value);
    ui->pbUpdateDeviceList->setEnabled(value);
}

bool MainDialog::waiting4Answer()
{
    mTimeOut = false;
    while (mWaiting4Answer && !isTimeOut())
        qApp->processEvents();
    return mWaiting4Answer;
}

void MainDialog::startReading()
{
    mWaiting4Answer = true;
    mTimeOut = false;
    mTimer->start();
}

bool MainDialog::isTimeOut() const
{
    return mTimeOut;
}
