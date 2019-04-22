#include "sspdwidget.h"
#include "ui_sspdwidget.h"
#include <QTime>
#include <QDebug>

SspdWidget::SspdWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SspdWidget)
    
{
    ui->setupUi(this);
}

SspdWidget::~SspdWidget()
{
    delete ui;
}

void SspdWidget::updateWidget()
{
    qDebug()<<"Update widget";
    bool ok = false;
    CU4SDM0V1_Data_t data;
    data = mDriver->deviceData()->getValueSequence(&ok, 5);
    if (ok)
        updateData(data);
}

void SspdWidget::setDriver(cCu4SdM0Driver *driver)
{
    mDriver = driver;
}

void SspdWidget::openWidget()
{
    qDebug()<<"Open widget";
    updateWidget();
    bool ok = false;
    CU4SDM0V1_Param_t params;
    qDebug()<<"try to update params";
        params = mDriver->deviceParams()->getValueSequence(&ok, 5);
    if (ok)
        paramsUpdated(params);
}

void SspdWidget::closeWidget()
{

}

void SspdWidget::updateData(CU4SDM0V1_Data_t data)
{
    qDebug()<<"data Updated";
    ui->lbData->setText(QString("I: %1 uA<br>U: %2 mV")
                        .arg(data.Current*1e6, 6,'f', 1)
                        .arg(data.Voltage*1e3, 6, 'f', 2));

    ui->cbAmplifier->setChecked(data.Status.stAmplifierOn);
    ui->cbShort->setChecked(data.Status.stShorted);
    ui->cbComparator->setChecked(data.Status.stComparatorOn);
    ui->cbAutoreset->setChecked(data.Status.stAutoResetOn);
}

void SspdWidget::paramsUpdated(CU4SDM0V1_Param_t params)
{
    qDebug()<<"paramsUpdated();";
    ui->dsbTimeOut->setValue(params.AutoResetTimeOut);
    ui->dsbThreshold->setValue(params.AutoResetThreshold);
}

void SspdWidget::on_cbShort_clicked(bool checked)
{
    mDriver->setShortEnable(checked);
    mDriver->waitingAnswer();
}

void SspdWidget::on_cbAmplifier_clicked(bool checked)
{
    mDriver->setAmpEnable(checked);
    mDriver->waitingAnswer();
}

void SspdWidget::on_cbComparator_clicked(bool checked)
{
    bool ok;
    CU4SDM0_Status_t status = mDriver->deviceStatus()->getValueSequence(&ok);
    if  (!ok){
        qDebug()<<"can't get Driver Status";
        return;
    }
    status.stComparatorOn = status.stRfKeyToCmp = status.stCounterOn = (checked) ? 1: 0;
    mDriver->deviceStatus()->setValue(status);
    if (!mDriver->waitingAnswer()) qDebug()<<"can't set device status";
}

void SspdWidget::on_pbSetI_clicked()
{
    mDriver->current()->setValue(ui->sbI->value()*1E-6);
    if (!mDriver->waitingAnswer()) qDebug()<<"can't set Current";
}

void SspdWidget::on_pbSetCmp_clicked()
{
    mDriver->cmpReferenceLevel()->setValue(ui->sbCmp->value());
    if (!mDriver->waitingAnswer()) qDebug()<<"can't set cmp level";
}

void SspdWidget::on_cbAutoreset_clicked(bool checked)
{
    mDriver->setAutoResetEnable(checked);
    mDriver->waitingAnswer();

}

void SspdWidget::on_pbSetParams_clicked()
{
    CU4SDM0V1_Param_t params = mDriver->deviceParams()->getCurrentValue();
    params.AutoResetThreshold = ui->dsbThreshold->value();
    params.AutoResetTimeOut = ui->dsbTimeOut->value();
    mDriver->deviceParams()->setValueSequence(params, nullptr, 5);
}
