#include "wsspddriver.h"
#include "ui_wsspddriver.h"
#include <QDoubleValidator>
#include <QMessageBox>
#include <QJsonArray>
#include "Drivers/sspddriverm1.h"
#include "Drivers/sspddriverm0.h"

wSspdDriver::wSspdDriver(QWidget *parent) :
    DriverWidget(parent),
    ui(new Ui::wSspdDriver)
{
    ui->setupUi(this);

    ui->leCmpRefDacIntercept    -> setValidator(new QDoubleValidator());
    ui->leCmpRefDacSlope        -> setValidator(new QDoubleValidator());
    ui->leCurrentAdcIntercept   -> setValidator(new QDoubleValidator());
    ui->leCurrentAdcSlope       -> setValidator(new QDoubleValidator());
    ui->leCurrentDacIntercept   -> setValidator(new QDoubleValidator());
    ui->leCurrentDacSlope       -> setValidator(new QDoubleValidator());
    ui->leVoltageAdcIntercept   -> setValidator(new QDoubleValidator());
    ui->leVoltageAdcSlope       -> setValidator(new QDoubleValidator());
    ui->lePulseWidth            -> setValidator(new QDoubleValidator());
}

wSspdDriver::~wSspdDriver()
{
    delete ui;
}

void wSspdDriver::setDriver(CommonDriver *driver)
{
    mDeviceType = dtUnknown;
    if (qobject_cast<SspdDriverM0*>(driver)){
        mDeviceType = dtSspdDriverM0;
        ui->lPulseWidth->setVisible(false);
        ui->lePulseWidth->setVisible(false);
    }
    if (qobject_cast<SspdDriverM1*>(driver)){
        mDeviceType = dtSspdDriverM1;
        ui->lPulseWidth->setVisible(true);
        ui->lePulseWidth->setVisible(true);
    }

    if (mDeviceType == dtUnknown)
        return;

    switch (mDeviceType) {
    case dtSspdDriverM0:{
        QObject::connect(qobject_cast<SspdDriverM0*>(driver),
                         &SspdDriverM0::eepromConstUpdated,
                         [=](const CU4SDM0V1_EEPROM_Const_t eepromConst){
            ui->leCmpRefDacIntercept    -> setText(QString::number(eepromConst.Cmp_Ref_DAC.second));
            ui->leCmpRefDacSlope        -> setText(QString::number(eepromConst.Cmp_Ref_DAC.first));
            ui->leCurrentAdcIntercept   -> setText(QString::number(eepromConst.Current_ADC.second));
            ui->leCurrentAdcSlope       -> setText(QString::number(eepromConst.Current_ADC.first));
            ui->leCurrentDacIntercept   -> setText(QString::number(eepromConst.Current_DAC.second));
            ui->leCurrentDacSlope       -> setText(QString::number(eepromConst.Current_DAC.first));
            ui->leVoltageAdcIntercept   -> setText(QString::number(eepromConst.Voltage_ADC.second));
            ui->leVoltageAdcSlope       -> setText(QString::number(eepromConst.Voltage_ADC.first));
        });
        break;
    }
    case dtSspdDriverM1:{
        QObject::connect(qobject_cast<SspdDriverM1*>(driver),
                         &SspdDriverM1::eepromConstUpdated,
                         [=](const CU4SDM1_EEPROM_Const_t eepromConst){
            ui->leCmpRefDacIntercept    -> setText(QString::number(eepromConst.Cmp_Ref_DAC.second));
            ui->leCmpRefDacSlope        -> setText(QString::number(eepromConst.Cmp_Ref_DAC.first));
            ui->leCurrentAdcIntercept   -> setText(QString::number(eepromConst.Current_ADC.second));
            ui->leCurrentAdcSlope       -> setText(QString::number(eepromConst.Current_ADC.first));
            ui->leCurrentDacIntercept   -> setText(QString::number(eepromConst.Current_DAC.second));
            ui->leCurrentDacSlope       -> setText(QString::number(eepromConst.Current_DAC.first));
            ui->leVoltageAdcIntercept   -> setText(QString::number(eepromConst.Voltage_ADC.second));
            ui->leVoltageAdcSlope       -> setText(QString::number(eepromConst.Voltage_ADC.first));
            ui->lePulseWidth            -> setText(QString::number(eepromConst.PulseWidth));
        });
        break;
    }
    default:
        return;
    }
    DriverWidget::setDriver(driver);
}

void wSspdDriver::enableGUI(bool enable)
{
    ui->leCmpRefDacIntercept    -> setEnabled(enable);
    ui->leCmpRefDacSlope        -> setEnabled(enable);
    ui->leCurrentAdcIntercept   -> setEnabled(enable);
    ui->leCurrentAdcSlope       -> setEnabled(enable);
    ui->leCurrentDacIntercept   -> setEnabled(enable);
    ui->leCurrentDacSlope       -> setEnabled(enable);
    ui->leVoltageAdcIntercept   -> setEnabled(enable);
    ui->leVoltageAdcSlope       -> setEnabled(enable);
    ui->lePulseWidth            -> setEnabled(enable);
}

void wSspdDriver::getEeprom()
{
    bool ok = false;
    switch (mDeviceType) {
    case dtSspdDriverM0:
        qobject_cast<SspdDriverM0*>(driver())->eepromConst()->getValueSync(&ok, 5);
        break;
    case dtSspdDriverM1:
        qobject_cast<SspdDriverM1*>(driver())->eepromConst()->getValueSync(&ok, 5);
        break;
    default:
        break;

    }
    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");
}

void wSspdDriver::setEeprom()
{
    bool ok = false;
    switch (mDeviceType) {
    case dtSspdDriverM0:{
        CU4SDM0V1_EEPROM_Const_t eepromConst;
        eepromConst.Cmp_Ref_DAC.second  = ui->leCmpRefDacIntercept->text().toFloat();
        eepromConst.Cmp_Ref_DAC.first   = ui->leCmpRefDacSlope->text().toFloat();
        eepromConst.Current_ADC.second  = ui->leCurrentAdcIntercept->text().toFloat();
        eepromConst.Current_ADC.first   = ui->leCurrentAdcSlope->text().toFloat();
        eepromConst.Current_DAC.second  = ui->leCurrentDacIntercept->text().toFloat();
        eepromConst.Current_DAC.first   = ui->leCurrentDacSlope->text().toFloat();
        eepromConst.Voltage_ADC.second  = ui->leVoltageAdcIntercept->text().toFloat();
        eepromConst.Voltage_ADC.first   = ui->leVoltageAdcSlope->text().toFloat();
        qobject_cast<SspdDriverM0*>(driver())->eepromConst()->setValueSync(eepromConst, &ok, 5);
        break;
    }
    case dtSspdDriverM1:{
        CU4SDM1_EEPROM_Const_t eepromConst;
        eepromConst.Cmp_Ref_DAC.second  = ui->leCmpRefDacIntercept->text().toFloat();
        eepromConst.Cmp_Ref_DAC.first   = ui->leCmpRefDacSlope->text().toFloat();
        eepromConst.Current_ADC.second  = ui->leCurrentAdcIntercept->text().toFloat();
        eepromConst.Current_ADC.first   = ui->leCurrentAdcSlope->text().toFloat();
        eepromConst.Current_DAC.second  = ui->leCurrentDacIntercept->text().toFloat();
        eepromConst.Current_DAC.first   = ui->leCurrentDacSlope->text().toFloat();
        eepromConst.Voltage_ADC.second  = ui->leVoltageAdcIntercept->text().toFloat();
        eepromConst.Voltage_ADC.first   = ui->leVoltageAdcSlope->text().toFloat();
        eepromConst.PulseWidth          = ui->lePulseWidth->text().toFloat();
        qobject_cast<SspdDriverM1*>(driver())->eepromConst()->setValueSync(eepromConst, &ok, 5);
        break;
    }
    default:
        break;
    }

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't set eeprom constants. Please try one more time!!!");
}

QJsonObject wSspdDriver::eepromToJson()
{
    QJsonObject calibrObject;
    QJsonArray value;
    value.append(ui->leVoltageAdcSlope->text().toDouble());
    value.append(ui->leVoltageAdcIntercept->text().toDouble());
    calibrObject["VoltageAdcCoeffs"] = value;

    value[0] = ui->leCurrentAdcSlope->text().toDouble();
    value[1] = ui->leCurrentAdcIntercept->text().toDouble();
    calibrObject["CurrentAdcCoeffs"] = value;

    value[0] = ui->leCurrentDacSlope->text().toDouble();
    value[1] = ui->leCurrentDacIntercept->text().toDouble();
    calibrObject["CurrentDacCoeffs"] = value;

    value[0] = ui->leCmpRefDacSlope->text().toDouble();
    value[1] = ui->leCmpRefDacIntercept->text().toDouble();
    calibrObject["leCmpRefDacCoeffs"] = value;

    if (mDeviceType == dtSspdDriverM1){
        calibrObject["PulseWidth"] = ui->lePulseWidth->text().toDouble();
    }
    return calibrObject;
}

void wSspdDriver::eepromFromJson(QJsonObject data)
{
    QJsonArray value = data["VoltageAdcCoeffs"].toArray();
    ui->leVoltageAdcSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leVoltageAdcIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["CurrentAdcCoeffs"].toArray();
    ui->leCurrentAdcSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentAdcIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["CurrentDacCoeffs"].toArray();
    ui->leCurrentDacSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentDacIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["leCmpRefDacCoeffs"].toArray();
    ui->leCmpRefDacSlope        -> setText(QString::number(value[0].toDouble()));
    ui->leCmpRefDacIntercept    -> setText(QString::number(value[1].toDouble()));

    if (mDeviceType == dtSspdDriverM1){
        ui->lePulseWidth->setText(QString::number(data["PulseWidth"].toDouble()));
    }
}
