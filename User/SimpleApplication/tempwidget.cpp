#include "tempwidget.h"
#include "ui_tempwidget.h"

TempWidget::TempWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TempWidget)
{
    ui->setupUi(this);
}

TempWidget::~TempWidget()
{
    delete ui;
}

void TempWidget::updateWidget()
{
    bool ok;
    CU4TDM0V1_Data_t data = mDriver->deviceData()->getValueSequence(&ok);
    if (ok) dataUpDated(data);
}

void TempWidget::openWidget()
{
    mDriver->commutatorTurnOn(true);
    mClosed = false;
    updateWidget();
}

void TempWidget::closeWidget()
{
    mDriver->commutatorTurnOn(false);
    mClosed = true;
}

void TempWidget::setDriver(cCu4TdM0Driver *driver)
{
    mDriver = driver;
}

void TempWidget::dataUpDated(CU4TDM0V1_Data_t data)
{
    QString tmpstr;
    if (data.Temperature == 0) tmpstr = "Sensor not connected";
    else tmpstr = QString("%1 K").arg(data.Temperature, 4, 'f', 2);
    ui->lbData->setText(QString("<h2>T: %1</h2><h3>P: %2 mbar<br>Temp sensor voltage: %3 V<br>Press sensor Voltage: %4 mV</h3>")
                        .arg(tmpstr)
                        .arg(data.Pressure,5,'f',1)
                        .arg(data.TempSensorVoltage,4,'f',3)
                        .arg((data.PressSensorVoltageP-data.PressSensorVoltageN)*1000.0, 5, 'f', 2));
    if (!(mClosed || data.CommutatorOn))
        mDriver->commutatorTurnOn(true);
}

