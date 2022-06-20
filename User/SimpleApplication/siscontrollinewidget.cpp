#include "siscontrollinewidget.h"
#include "ui_siscontrollinewidget.h"

#include <QDebug>

SisControlLineWidget::SisControlLineWidget(QWidget *parent)
    : CommonWidget(parent)
    , ui(new Ui::SisControlLineWidget)
{
    ui->setupUi(this);

}

SisControlLineWidget::~SisControlLineWidget()
{
    delete ui;
}

SisControlLineDriverM0 *SisControlLineWidget::driver() const
{
    return mDriver;
}

void SisControlLineWidget::setDriver(SisControlLineDriverM0 *newDriver)
{
    mDriver = newDriver;
}


void SisControlLineWidget::closeWidget()
{
}

void SisControlLineWidget::openWidget()
{
    qDebug()<<"Open widget";
    updateWidget();
}

void SisControlLineWidget::updateWidget()
{
    qDebug()<<"Update widget";
    bool ok = false;
    auto data = mDriver->data()->getValueSync(&ok, 5);
    if (ok)
        updateData(data);
}

void SisControlLineWidget::updateData(CU4CLM0V0_Data_t data)
{
    ui->lbData->setText(QString(tr("I: %1 mA<br>"))
                        .arg(static_cast<double>(data.current) * 1e3, 6,'f', 2));

    ui->cbShort->setChecked(data.shortCircuit);
}

void SisControlLineWidget::on_pbSetI_clicked()
{
    bool ok = false;
    mDriver->current()->setValueSync(static_cast<float>(ui->sbI->value() * 1E-3), &ok, 5);
    if (!ok) qDebug()<<"can't set Current";
}


void SisControlLineWidget::on_cbShort_clicked(bool checked)
{
    mDriver->shortEnable()->setValueSync(checked, nullptr, 5);
}

