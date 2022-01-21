#include "editwidget.h"
#include "ui_editwidget.h"
#include "QDoubleValidator"
#include <QDebug>
#include <QObject>

EditWidget::EditWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditWidget)
    , mDriver(nullptr)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SIGNAL(clickShorted(bool)));
}

EditWidget::~EditWidget()
{
    delete ui;
}

void EditWidget::setCurrent(double value)
{
    ui->doubleSpinBox->setValue(value);
}

double EditWidget::getCurrent()
{
    return ui->doubleSpinBox->value();
}

void EditWidget::setChecked(bool value)
{
    ui->pushButton->setChecked(value);
}

bool EditWidget::getChecked()
{
    return ui->pushButton->isChecked();
}

void EditWidget::setDriver(CommonDriver *newDriver)
{
    mDriver = newDriver;
}

void EditWidget::on_pushButton_clicked(bool checked)
{
    {
        auto* tmpDriver = qobject_cast<SspdDriverM0*>(mDriver);
        if (tmpDriver){
            tmpDriver->shortEnable()->setValueSync(checked);
        }
    }
    {
        auto* tmpDriver = qobject_cast<SspdDriverM1*>(mDriver);
        if (tmpDriver){
            tmpDriver->shortEnable()->setValueSync(checked);
        }
    }
    {
        auto* tmpDriver = qobject_cast<SisControlLineDriverM0*>(mDriver);
        if (tmpDriver){
            tmpDriver->shortEnable()->setValueSync(checked);
        }
    }
}

void EditWidget::on_doubleSpinBox_editingFinished()
{
    {
        auto* tmpDriver = qobject_cast<SspdDriverM0*>(mDriver);
        if (tmpDriver){
            tmpDriver->current()->setValueSync(static_cast<float>(ui->doubleSpinBox->value() * 1E-6), nullptr, 5);
        }
    }
    {
        auto* tmpDriver = qobject_cast<SspdDriverM1*>(mDriver);
        if (tmpDriver){
            tmpDriver->current()->setValueSync(static_cast<float>(ui->doubleSpinBox->value() * 1E-6), nullptr, 5);
        }
    }
    {
        auto* tmpDriver = qobject_cast<SisControlLineDriverM0*>(mDriver);
        if (tmpDriver){
            tmpDriver->current()->setValueSync(static_cast<float>(ui->doubleSpinBox->value() * 1E-3), nullptr, 5);
        }
    }
}
