#include "wtempm1supportwidget.h"
#include "ui_wtempm1supportwidget.h"
#include <QFileDialog>

wTempM1SupportWidget::wTempM1SupportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wTempM1SupportWidget)
{
    ui->setupUi(this);
    ui->leCurrentSlope      -> setValidator(new QDoubleValidator());
    ui->leVoltageSlope      -> setValidator(new QDoubleValidator());
    ui->leCurrentIntercept  -> setValidator(new QDoubleValidator());
    ui->leVoltageIntercept  -> setValidator(new QDoubleValidator());

    ui->twTempTable->setRowCount(TEMP_TABLE_SIZE);
    for (int i = 0; i< TEMP_TABLE_SIZE; ++i){
        ui->twTempTable->setItem(i,0, new QTableWidgetItem(""));
        ui->twTempTable->setItem(i,1, new QTableWidgetItem(""));
    }

}

wTempM1SupportWidget::~wTempM1SupportWidget()
{
    delete ui;
}

TDM1_EepromCoeff wTempM1SupportWidget::eepromCoeff()
{
    TDM1_EepromCoeff coeff;
    coeff.current.first  = ui->leCurrentSlope->text().toFloat();
    coeff.voltage.first  = ui->leVoltageSlope->text().toFloat();
    coeff.current.second = ui->leCurrentIntercept->text().toFloat();
    coeff.voltage.second = ui->leVoltageIntercept->text().toFloat();
    return coeff;
}

void wTempM1SupportWidget::setEepromCoeff(TDM1_EepromCoeff coeff)
{
    ui->leCurrentSlope      -> setText(QString::number(coeff.current.first));
    ui->leVoltageSlope      -> setText(QString::number(coeff.voltage.first));
    ui->leCurrentIntercept  -> setText(QString::number(coeff.current.second));
    ui->leVoltageIntercept  -> setText(QString::number(coeff.voltage.second));
}

DefaultParam wTempM1SupportWidget::getDefaultParam()
{
    DefaultParam param;
    param.enable = ui->cbEnable->isChecked();
    param.current = ui->sbDefaultCurrent->value() * 1e-6;
    return param;
}

void wTempM1SupportWidget::setDefaultParam(DefaultParam param)
{
    ui->sbDefaultCurrent->setValue(param.current * 1e6);
    ui->cbEnable->setChecked(param.enable);
}

void wTempM1SupportWidget::setTemperatureTableItem(CU4TDM0V1_Temp_Table_Item_t item, uint8_t index)
{
    if (index < TEMP_TABLE_SIZE){
        ui->twTempTable->item(index, 0)->setText(QString("%1").arg(item.Voltage));
        ui->twTempTable->item(index, 1)->setText(QString("%1").arg(item.Temperature));
    }
}

void wTempM1SupportWidget::setTemperatureTable(CU4TDM0V1_Temp_Table_Item_t *item)
{
    for (int i = 0; i < TEMP_TABLE_SIZE; ++i)
        setTemperatureTableItem(item[i],i);
}


CU4TDM0V1_Temp_Table_Item_t *wTempM1SupportWidget::temperatureTable()
{
    for (int i = 0; i < TEMP_TABLE_SIZE; i++){
        mTempTable[i].Voltage = ui->twTempTable->item(i, 0)->text().toFloat();
        mTempTable[i].Temperature = ui->twTempTable->item(i, 1)->text().toFloat();
    }
    return mTempTable;
}

void wTempM1SupportWidget::on_pbLoadTable_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open temp table...");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    char buf[1024];
    for (int i = 0; i< TEMP_TABLE_SIZE; ++i) {
        int length = file.readLine(buf, sizeof(buf));
        if (length == -1) break;
        QStringList strL = QString(QByteArray(buf, length)).split("\t");
        if (strL.count() == 1)
            strL = QString(QByteArray(buf, length)).split(" ");
        if (strL.count()>1){
            ui->twTempTable->item(i, 0)->setText(QString("%1").arg(strL[0].toDouble()));
            ui->twTempTable->item(i, 1)->setText(QString("%1").arg(strL[1].toDouble()));
        }
    }
    file.close();
}
