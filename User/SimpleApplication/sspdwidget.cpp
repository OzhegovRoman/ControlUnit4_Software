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
    auto data = mDriver->data()->getValueSync(&ok, 5);
    if (ok)
        updateData(data);
}

void SspdWidget::setDriver(SspdDriverM0 *driver)
{
    mDriver = driver;
}

void SspdWidget::openWidget()
{
    qDebug()<<"Open widget";
    updateWidget();
    bool ok = false;
    qDebug()<<"try to update params";
    auto params = mDriver->params()->getValueSync(&ok, 5);
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
                        .arg(static_cast<double>(data.Current) * 1e6, 6,'f', 1)
                        .arg(static_cast<double>(data.Voltage) * 1e3, 6, 'f', 2));

    ui->cbAmplifier->setChecked(data.Status.stAmplifierOn);
    ui->cbShort->setChecked(data.Status.stShorted);
    ui->cbComparator->setChecked(data.Status.stComparatorOn);
    ui->cbAutoreset->setChecked(data.Status.stAutoResetOn);
}

void SspdWidget::paramsUpdated(CU4SDM0V1_Param_t params)
{
    qDebug()<<"paramsUpdated();";
    ui->dsbTimeOut->setValue(static_cast<double>(params.AutoResetTimeOut));
    ui->dsbThreshold->setValue(static_cast<double>(params.AutoResetThreshold));
}

void SspdWidget::on_cbShort_clicked(bool checked)
{
    mDriver->shortEnable()->setValueSync(checked, nullptr, 5);
}

void SspdWidget::on_cbAmplifier_clicked(bool checked)
{
    mDriver->amplifierEnable()->setValueSync(checked, nullptr, 5);
}

void SspdWidget::on_cbComparator_clicked(bool checked)
{
    bool ok;
    auto status = mDriver->status()->getValueSync(&ok, 5);
    if  (!ok){
        qDebug()<<"can't get Driver Status";
        return;
    }
    status.stComparatorOn = status.stRfKeyToCmp = status.stCounterOn = (checked) ? 1: 0;
    mDriver->status()->setValueSync(status, &ok, 5);
    if (!ok) qDebug()<<"can't set device status";
}

void SspdWidget::on_pbSetI_clicked()
{
    bool ok = false;
    mDriver->current()->setValueSync(static_cast<float>(ui->sbI->value() * 1E-6), &ok, 5);
    if (!ok) qDebug()<<"can't set Current";
}

void SspdWidget::on_pbSetCmp_clicked()
{
    bool ok = false;
    mDriver->cmpReferenceLevel()->setValueSync(static_cast<float>(ui->sbCmp->value()), &ok, 5);
    if (!ok) qDebug()<<"can't set cmp level";
}

void SspdWidget::on_cbAutoreset_clicked(bool checked)
{
    mDriver->autoResetEnable()->setValueSync(checked, nullptr, 5);
}

void SspdWidget::on_pbSetParams_clicked()
{
    auto params = mDriver->params()->currentValue();
    params.AutoResetThreshold = static_cast<float>(ui->dsbThreshold->value());
    params.AutoResetTimeOut = static_cast<float>(ui->dsbTimeOut->value());
    mDriver->params()->setValueSync(params, nullptr, 5);
}
