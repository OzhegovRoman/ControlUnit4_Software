#include "tempwidget.h"
#include "ui_tempwidget.h"

#include <QDebug>
#include <QTime>


TempWidget::TempWidget(QWidget *parent)
    : CommonWidget(parent)
    , ui(new Ui::TempWidget)
{
    ui->setupUi(this);

    TempTimer.start();
}

TempWidget::~TempWidget()
{
    delete ui;
}

void TempWidget::updateWidget()
{
    bool ok = false;
    auto data = mDriver->data()->getValueSync(&ok, 5);
    if (ok) dataUpDated(data);
}

void TempWidget::openWidget()
{
    qDebug()<<"Open Widget"<<QTime::currentTime();
    mDriver->commutator()->setValueSync(true, nullptr, 2);
    mClosed = false;
    updateWidget();
}

void TempWidget::closeWidget()
{
    mDriver->commutator()->setValueSync(false, nullptr, 2);
    mClosed = true;
}

void TempWidget::setDriver(TempDriverM0 *driver)
{
    mDriver = driver;
}

void TempWidget::dataUpDated(CU4TDM0V1_Data_t data)
{
    QString tmpstr;
    if (qAbs(static_cast<double>(data.Temperature)) < 1e-5) tmpstr = tr("Sensor not connected");
    else tmpstr = QString("%1 K").arg(static_cast<double>(data.Temperature), 4, 'f', 2);
    ui->lbData->setText(QString(tr("<h2>T: %1</h2><h3>P: %2 mbar<br>Temp sensor voltage: %3 V<br>Press sensor Voltage: %4 mV</h3>"))
                        .arg(tmpstr)
                        .arg(static_cast<double>(data.Pressure), 5, 'f', 1)
                        .arg(static_cast<double>(data.TempSensorVoltage), 4, 'f', 3)
                        .arg(static_cast<double>(data.PressSensorVoltageP-data.PressSensorVoltageN) * 1000.0, 5, 'f', 2));
    if (!(mClosed || data.CommutatorOn))
        mDriver->commutator()->setValueSync(true, nullptr, 2);
}
