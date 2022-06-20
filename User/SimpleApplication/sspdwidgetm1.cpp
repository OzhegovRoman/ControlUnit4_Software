#include "sspdwidgetm1.h"
#include "ui_sspdwidgetm1.h"
#include <QTime>
#include <QDebug>

SspdWidgetM1::SspdWidgetM1(QWidget *parent)
    : CommonWidget(parent)
    , ui(new Ui::SspdWidgetM1)
    
{
    ui->setupUi(this);
}

SspdWidgetM1::~SspdWidgetM1()
{
    delete ui;
}

void SspdWidgetM1::updateWidget()
{
    qDebug()<<"Update widget";
    bool ok = false;
    auto data = mDriver->data()->getValueSync(&ok, 5);
    if (ok)
        updateData(data);
}

void SspdWidgetM1::setDriver(SspdDriverM1 *driver)
{
    mDriver = driver;
}

void SspdWidgetM1::openWidget()
{
    qDebug()<<"Open widget";
    updateWidget();
    bool ok = false;
    qDebug()<<"try to update params";
    auto params = mDriver->params()->getValueSync(&ok, 5);
    if (ok)
        paramsUpdated(params);
}

void SspdWidgetM1::updateData(CU4SDM1_Data_t data)
{
    QString tmp = QString("%1")
            .arg(static_cast<double>(mDriver->counts()->currentValue())/mDriver->params()->currentValue().Time_Const, 6, 'g', 4);
    tmp.replace("e+0","e");

    ui->lbData->setText(QString(tr("I: %1 uA<br>U: %2 mV<br>cps: %3"))
                        .arg(static_cast<double>(data.Current) * 1e6, 6,'f', 1)
                        .arg(static_cast<double>(data.Voltage) * 1e3, 6, 'f', 2)
                        .arg(tmp));
    ui->cbAmplifier->setChecked(data.Status.stAmplifierOn);
    ui->cbShort->setChecked(data.Status.stShorted);
    ui->cbCounter->setChecked(data.Status.stComparatorOn & data.Status.stCounterOn);
    ui->cbAutoreset->setChecked(data.Status.stAutoResetOn);
    ui->cbHFMode->setChecked(data.Status.stHFModeOn);

}

void SspdWidgetM1::paramsUpdated(CU4SDM1_Param_t params)
{
    qDebug()<<"paramsUpdated();";
    ui->dsbTimeOut->setValue(static_cast<double>(params.AutoResetTimeOut));
    ui->dsbThreshold->setValue(static_cast<double>(params.AutoResetThreshold));
}

void SspdWidgetM1::on_cbShort_clicked(bool checked)
{
    mDriver->shortEnable()->setValueSync(checked, nullptr, 5);
}

void SspdWidgetM1::on_cbAmplifier_clicked(bool checked)
{
    mDriver->amplifierEnable()->setValueSync(checked, nullptr, 5);
}

void SspdWidgetM1::on_pbSetI_clicked()
{
    bool ok = false;
    mDriver->current()->setValueSync(static_cast<float>(ui->sbI->value() * 1E-6), &ok, 5);
    if (!ok) qDebug()<<"can't set Current";
}

void SspdWidgetM1::on_pbSetCmp_clicked()
{
    bool ok = false;
    mDriver->cmpReferenceLevel()->setValueSync(static_cast<float>(ui->sbCmp->value()), &ok, 5);
    if (!ok) qDebug()<<"can't set cmp level";
}

void SspdWidgetM1::on_cbAutoreset_clicked(bool checked)
{
    mDriver->autoResetEnable()->setValueSync(checked, nullptr, 5);
}

void SspdWidgetM1::on_pbSetParams_clicked()
{
    auto params = mDriver->params()->currentValue();
    params.AutoResetThreshold = static_cast<float>(ui->dsbThreshold->value());
    params.AutoResetTimeOut = static_cast<float>(ui->dsbTimeOut->value());
    mDriver->params()->setValueSync(params, nullptr, 5);
}

void SspdWidgetM1::on_cbHFMode_clicked(bool checked)
{
    mDriver->highFrequencyModeEnable()->setValueSync(checked, nullptr, 5);
}

void SspdWidgetM1::on_cbCounter_clicked(bool checked)
{
    bool ok;
    auto status = mDriver->status()->getValueSync(&ok, 5);
    if  (!ok){
        qDebug()<<"can't get Driver Status";
        return;
    }

    status.stCounterOn =    checked ? 1: 0;
    status.stComparatorOn = checked ? 1 :0;

    mDriver->status()->setValueSync(status, &ok, 5);
    if (!ok) qDebug()<<"can't set device status";
}
