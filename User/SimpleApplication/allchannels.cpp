#include "allchannels.h"
#include "ui_allchannels.h"
#include <QDebug>
#include <Drivers/ccu4sdm0driver.h>
#include <Drivers/ccu4tdm0driver.h>
#include <QSettings>
#include <QDir>
#include <QDateTime>

AllChannels::AllChannels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AllChannels),
    model(new AllChannelsDataModel(this)),
    delegate(new AllChannelsDataDelegate(this))
{
    ui->setupUi(this);

    QSettings settings("Scontel", "cu-simpleapp");
    ui->cbLogEnable->setChecked(settings.value("isLogEnable", false).toBool());
    QString tmpPath = settings.value("LogPath", QDir::currentPath()).toString();
    if (!QDir(tmpPath).exists()) tmpPath = QDir::currentPath();
    ui->leLogPath->setText(tmpPath);

}

AllChannels::~AllChannels()
{
    QSettings settings("Scontel", "cu-simpleapp");
    settings.setValue("isLogEnable", ui->cbLogEnable->isChecked());
    settings.setValue("LogPath", ui->leLogPath->text());
    delete ui;
}

void AllChannels::openWidget()
{
    qDebug()<<"oopen";
    emit setTimeOut(static_cast<int>(ui->dsbTimeOut->value()*1000.0));
}

void AllChannels::closeWidget()
{
    qDebug()<<"close";
    emit setTimeOut(500);
}

void AllChannels::updateWidget()
{
    // опрос всех устройств
    for (int idx = 0; idx < model->devices.count(); ++idx){
        if (model->devices[idx].isSspd){
            cCu4SdM0Driver driver;
            driver.setIOInterface(mInterface);
            driver.setDevAddress(model->devices[idx].devAddress);
            bool ok;
            CU4SDM0V1_Data_t data = driver.deviceData()->getValueSequence(&ok, 5);
            if (ok){
                model->devices[idx].current = data.Current;
                model->devices[idx].voltage = data.Voltage;
                model->devices[idx].isShorted = data.Status.stShorted;
                if (data.Status.stAutoResetOn && ui->cbLogEnable->isChecked()){
                    CU4SDM0V1_Param_t param = driver.deviceParams()->getValueSequence(&ok, 5);
                    if (ok && (param.AutoResetCounts != model->devices[idx].triggerCount)){
                        QFile m_File(QString("%1\\TriggerLog.txt").arg(ui->leLogPath->text()));
                        m_File.open(QIODevice::ReadWrite | QIODevice::Append);
                        QTextStream out(&m_File);
                        QString tmpStr = QString("[%1]: SSPD Unit Triggered. Adress: %2. Trigger counts: %3\r\n")
                                .arg(QDateTime::currentDateTime().toString("MM-dd-yyyy HH-mm-ss"))
                                .arg(model->devices[idx].devAddress)
                                .arg(param.AutoResetCounts);
                        qDebug()<<tmpStr;
                        out<<tmpStr;
                        m_File.close();
                        model->devices[idx].triggerCount = param.AutoResetCounts;
                    }
                }
            }

        }
        else{
            cCu4TdM0Driver driver;
            driver.setIOInterface(mInterface);
            driver.setDevAddress(model->devices[idx].devAddress);
            CU4TDM0V1_Data_t data = driver.deviceData()->getValueSequence(nullptr, 5);
            model->devices[idx].temperature = data.Temperature;
            if (!data.CommutatorOn) driver.commutatorOn()->setValueSequence(true, nullptr, 5);
        }
    }

    emit model->dataChanged(model->index(0), model->index(model->devices.count()));
}

void AllChannels::initialize(const QString& deviceInfo)
{
    model->initialize(deviceInfo);
    ui->tableView->setModel(model);
    delegate->setModel(model);
    ui->tableView->setItemDelegate(delegate);
}

void AllChannels::on_pbSetUpdateTime_clicked()
{
    openWidget();
}

void AllChannels::setInterface(cuIOInterface *interface)
{
    mInterface = interface;
    delegate->setInterface(mInterface);
}

void AllChannels::on_pushButton_clicked()
{

}
