#include "allchannels.h"
#include "ui_allchannels.h"
#include <QDebug>
#include <Drivers/ccu4sdm0driver.h>
#include <Drivers/ccu4tdm0driver.h>

AllChannels::AllChannels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AllChannels),
    model(new AllChannelsDataModel(this)),
    delegate(new AllChannelsDataDelegate(this)),
    mInterface(nullptr)
{
    ui->setupUi(this);
}

AllChannels::~AllChannels()
{
    delete ui;
}

void AllChannels::openWidget()
{
    qDebug()<<"oopen";
    emit setTimeOut(static_cast<int>(ui->dsbTimeOut->value()*1000.0));
}

void AllChannels::closeWidget()
{
    qDebug()<<"cclose";
    emit setTimeOut(500);
}

void AllChannels::updateWidget()
{
    qDebug()<<"uupdate";
    // опрос всех устройств
    for (int idx = 0; idx < model->devices.count(); ++idx){
        if (model->devices[idx].isSspd){
            cCu4SdM0Driver driver;
            driver.setIOInterface(mInterface);
            driver.setDevAddress(model->devices[idx].devAddress);
            CU4SDM0V1_Data_t data = driver.deviceData()->getValueSequence(nullptr, 5);
            model->devices[idx].current = data.Current;
            model->devices[idx].voltage = data.Voltage;
            model->devices[idx].isShorted = data.Status.stShorted;
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

void AllChannels::initialize(QString deviceInfo)
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
