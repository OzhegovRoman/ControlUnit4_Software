#include "tempm1widget.h"
#include "ui_tempm1widget.h"
#include <QDebug>

TempM1Widget::TempM1Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TempM1Widget)
{
    ui->setupUi(this);
    mTimer.setSingleShot(true);
    connect(&mTimer, &QTimer::timeout, this, &TempM1Widget::onTimerTicker);
}

TempM1Widget::~TempM1Widget()
{
    delete ui;
}

void TempM1Widget::updateWidget()
{

}

void TempM1Widget::openWidget()
{
    mDriver->readDefaultParams();
    onTimerTicker();
}

void TempM1Widget::closeWidget()
{
    mTimer.stop();
}

void TempM1Widget::setDriver(TempDriverM1 *driver)
{
    mDriver = driver;
}

void TempM1Widget::onTimerTicker()
{
    QString strTemperature, strVoltage;
    mDriver->updateTemperature();
    mDriver->updateVoltage();

    for (int i = 0; i < 4; ++i){
        if (mDriver->defaultParam(i).enable){
            strTemperature += QString("T%1: %2K\n").arg(i) .arg(mDriver->currentTemperature(i), 4, 'f', 2);
            strVoltage += QString("U%1: %2V\n").arg(i) .arg(mDriver->currentVoltage(i), 0, 'g', 5);
        }
    }
    strTemperature.chop(1);
    strVoltage.chop(1);
    ui->lbTemperature->setText(strTemperature);
    ui->lbVoltage->setText(strVoltage);

    mDriver->relaysStatus()->getValueSync();
    ui->cb25V->setChecked(mDriver->relaysStatus()->currentValue()[0]);
    ui->cb5V->setChecked(mDriver->relaysStatus()->currentValue()[1]);

    mTimer.start(1000);
}

void TempM1Widget::on_cb5V_clicked(bool checked)
{
    cRelaysStatus status;
    status.setStatus(cRelaysStatus::ri5V, checked ? cRelaysStatus::ri5V : 0);
    mDriver->relaysStatus()->setValueSync(status, nullptr, 5);
}

void TempM1Widget::on_cb25V_clicked(bool checked)
{
    cRelaysStatus status;
    status.setStatus(cRelaysStatus::ri25V, checked ? cRelaysStatus::ri25V : 0);
    mDriver->relaysStatus()->setValueSync(status, nullptr, 5);
}
