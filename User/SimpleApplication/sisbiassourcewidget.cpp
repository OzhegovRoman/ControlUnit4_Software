#include "sisbiassourcewidget.h"
#include "ui_sisbiassourcewidget.h"

#include <QDebug>

SisBiasSourceWidget::SisBiasSourceWidget(QWidget *parent)
    : CommonWidget(parent)
    , ui(new Ui::SisBiasSourceWidget)
{
    ui->setupUi(this);
}

SisBiasSourceWidget::~SisBiasSourceWidget()
{
    delete ui;
}

SisBiasSourceDriverM0 *SisBiasSourceWidget::driver() const
{
    return mDriver;
}

void SisBiasSourceWidget::setDriver(SisBiasSourceDriverM0 *newDriver)
{
    mDriver = newDriver;
}

void SisBiasSourceWidget::updateData(CU4BSM0_Data_t data)
{
    ui->lbData->setText(QString(tr("U: %1 mV<br>I: %2 uA<br>"))
                        .arg(static_cast<double>(data.voltage) * 1e3, 6,'f', 2)
                        .arg(static_cast<double>(data.current) * 1e6, 6,'f', 1)
                        );
    ui->cbShort->setChecked(data.shortCircuit);
    ui->rbI->   setChecked(data.mode == CU4BSM0_IMODE);
    ui->pbSetI->setEnabled(data.mode == CU4BSM0_IMODE);
    ui->sbI->   setEnabled(data.mode == CU4BSM0_IMODE);
    ui->rbU->   setChecked(data.mode == CU4BSM0_UMODE);
    ui->pbSetU->setEnabled(data.mode == CU4BSM0_UMODE);
    ui->sbU->   setEnabled(data.mode == CU4BSM0_UMODE);
}

void SisBiasSourceWidget::closeWidget()
{
}

void SisBiasSourceWidget::openWidget()
{
    updateWidget();
}

void SisBiasSourceWidget::updateWidget()
{
    bool ok = false;
    auto data = mDriver->data()->getValueSync(&ok, 5);
    if (ok)
        updateData(data);
}

void SisBiasSourceWidget::on_cbShort_clicked(bool checked)
{
    mDriver->shortEnable()->setValueSync(checked, nullptr, 5);
}


void SisBiasSourceWidget::on_pbSetU_clicked()
{
mDriver->voltage()->setValueSync(ui->sbU->value()*1e-3, nullptr, 5);
}


void SisBiasSourceWidget::on_pbSetI_clicked()
{
    mDriver->current()->setValueSync(ui->sbI->value()*1e-6, nullptr, 5);
}


void SisBiasSourceWidget::on_rbU_toggled(bool checked)
{
    mDriver->mode()->setValueSync(checked ? CU4BSM0_UMODE : CU4BSM0_IMODE, nullptr, 5);
}

