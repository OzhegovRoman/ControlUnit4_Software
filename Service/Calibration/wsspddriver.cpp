#include "wsspddriver.h"
#include "ui_wsspddriver.h"
#include <QDoubleValidator>

wSspdDriver::wSspdDriver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wSspdDriver)
{
    ui->setupUi(this);

    ui->leCmpRefDacIntercept->setValidator(new QDoubleValidator());
    ui->leCmpRefDacSlope->setValidator(new QDoubleValidator());
    ui->leCurrentAdcIntercept->setValidator(new QDoubleValidator());
    ui->leCurrentAdcSlope->setValidator(new QDoubleValidator());
    ui->leCurrentDacIntercept->setValidator(new QDoubleValidator());
    ui->leCurrentDacSlope->setValidator(new QDoubleValidator());
    ui->leVoltageAdcIntercept->setValidator(new QDoubleValidator());
    ui->leVoltageAdcSlope->setValidator(new QDoubleValidator());
}

wSspdDriver::~wSspdDriver()
{
    delete ui;
}

CU4SDM0V1_EEPROM_Const_t wSspdDriver::getEepromConst()
{
    CU4SDM0V1_EEPROM_Const_t eepromConst;
    eepromConst.Cmp_Ref_DAC.second = ui->leCmpRefDacIntercept->text().toFloat();
    eepromConst.Cmp_Ref_DAC.first = ui->leCmpRefDacSlope->text().toFloat();
    eepromConst.Current_ADC.second = ui->leCurrentAdcIntercept->text().toFloat();
    eepromConst.Current_ADC.first = ui->leCurrentAdcSlope->text().toFloat();
    eepromConst.Current_DAC.second = ui->leCurrentDacIntercept->text().toFloat();
    eepromConst.Current_DAC.first = ui->leCurrentDacSlope->text().toFloat();
    eepromConst.Voltage_ADC.second = ui->leVoltageAdcIntercept->text().toFloat();
    eepromConst.Voltage_ADC.first = ui->leVoltageAdcSlope->text().toFloat();
    return eepromConst;
}

void wSspdDriver::onEepromConstReceived(CU4SDM0V1_EEPROM_Const_t eepromConst)
{
    ui->leCmpRefDacIntercept->setText(QString("%1").arg(eepromConst.Cmp_Ref_DAC.second));
    ui->leCmpRefDacSlope->setText(QString("%1").arg(eepromConst.Cmp_Ref_DAC.first));
    ui->leCurrentAdcIntercept->setText(QString("%1").arg(eepromConst.Current_ADC.second));
    ui->leCurrentAdcSlope->setText(QString("%1").arg(eepromConst.Current_ADC.first));
    ui->leCurrentDacIntercept->setText(QString("%1").arg(eepromConst.Current_DAC.second));
    ui->leCurrentDacSlope->setText(QString("%1").arg(eepromConst.Current_DAC.first));
    ui->leVoltageAdcIntercept->setText(QString("%1").arg(eepromConst.Voltage_ADC.second));
    ui->leVoltageAdcSlope->setText(QString("%1").arg(eepromConst.Voltage_ADC.first));
}
