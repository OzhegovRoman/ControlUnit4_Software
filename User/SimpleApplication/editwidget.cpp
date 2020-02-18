#include "editwidget.h"
#include "ui_editwidget.h"
#include "QDoubleValidator"
#include "Drivers/sspddriverm0.h"
#include <QDebug>

EditWidget::EditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditWidget)
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

void EditWidget::setInterface(cuIOInterface *value)
{
    qDebug()<<"setInterface"<<value;
    interface = value;
}

void EditWidget::on_pushButton_clicked(bool checked)
{
    qDebug()<<checked<<index;
    SspdDriverM0 driver;
    driver.setIOInterface(interface);
    driver.setDevAddress(static_cast<quint8>(index));
    driver.shortEnable()->setValueSync(checked);
}

void EditWidget::setIndex(int value)
{
    index = value;
}

void EditWidget::on_doubleSpinBox_editingFinished()
{
    SspdDriverM0 driver;
    driver.setIOInterface(interface);
    driver.setDevAddress(static_cast<quint8>(index));
    driver.current()->setValueSync(static_cast<float>(ui->doubleSpinBox->value() * 1E-6), nullptr, 5);
}
