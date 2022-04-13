#include "wsisbiassourcedriver.h"
#include "ui_wsisbiassourcedriver.h"

#include <QDoubleValidator>
#include <QMessageBox>
#include <QJsonArray>
#include "Drivers/sisbiassourcedriverm0.h"


wSisBiasSourceDriver::wSisBiasSourceDriver(QWidget *parent) :
    DriverWidget(parent),
    ui(new Ui::wSisBiasSourceDriver)
{
    ui->setupUi(this);

    ui->leCurrentAdcIntercept       -> setValidator(new QDoubleValidator());
    ui->leCurrentAdcSlope           -> setValidator(new QDoubleValidator());
    ui->leCurrentDacIntercept       -> setValidator(new QDoubleValidator());
    ui->leCurrentDacSlope           -> setValidator(new QDoubleValidator());
    ui->leCurrentMaxLimit           -> setValidator(new QDoubleValidator());
    ui->leCurrentMinLimit           -> setValidator(new QDoubleValidator());
    ui->leCurrentStep               -> setValidator(new QDoubleValidator());
    ui->leVoltageAdcIntercept       -> setValidator(new QDoubleValidator());
    ui->leVoltageAdcSlope           -> setValidator(new QDoubleValidator());
    ui->leVoltageDacIntercept       -> setValidator(new QDoubleValidator());
    ui->leVoltageDacSlope           -> setValidator(new QDoubleValidator());
    ui->leVoltageMaxLimit           -> setValidator(new QDoubleValidator());
    ui->leVoltageMinLimit           -> setValidator(new QDoubleValidator());
    ui->leVoltageStep               -> setValidator(new QDoubleValidator());
    ui->leCurrentMonitorResistance  -> setValidator(new QDoubleValidator());

}

wSisBiasSourceDriver::~wSisBiasSourceDriver()
{
    delete ui;
}


void wSisBiasSourceDriver::setDriver(CommonDriver *driver)
{
    auto* _driver = qobject_cast<SisBiasSourceDriverM0*>(driver);
    if (!_driver) return;

    QObject::connect(_driver,
                     &SisBiasSourceDriverM0::eepromConstUpdated,
                     [=](const CU4BSM0V0_EEPROM_Const_t eepromConst){
        ui->leCurrentAdcIntercept       -> setText(QString::number(eepromConst.currentAdc.second));
        ui->leCurrentAdcSlope           -> setText(QString::number(eepromConst.currentAdc.first));
        ui->leCurrentDacIntercept       -> setText(QString::number(eepromConst.currentDac.second));
        ui->leCurrentDacSlope           -> setText(QString::number(eepromConst.currentDac.first));
        ui->leCurrentMaxLimit           -> setText(QString::number(eepromConst.currentLimits.second));
        ui->leCurrentMinLimit           -> setText(QString::number(eepromConst.currentLimits.first));
        ui->leCurrentStep               -> setText(QString::number(eepromConst.currentStep));
        ui->leVoltageAdcIntercept       -> setText(QString::number(eepromConst.voltageAdc.second));
        ui->leVoltageAdcSlope           -> setText(QString::number(eepromConst.voltageAdc.first));
        ui->leVoltageDacIntercept       -> setText(QString::number(eepromConst.voltageDac.second));
        ui->leVoltageDacSlope           -> setText(QString::number(eepromConst.voltageDac.first));
        ui->leVoltageMaxLimit           -> setText(QString::number(eepromConst.voltageLimits.second));
        ui->leVoltageMinLimit           -> setText(QString::number(eepromConst.voltageLimits.first));
        ui->leVoltageStep               -> setText(QString::number(eepromConst.voltageStep));
        ui->leCurrentMonitorResistance  -> setText(QString::number(eepromConst.currentMonitorResistance));

    });

    DriverWidget::setDriver(driver);
}

void wSisBiasSourceDriver::enableGUI(bool enable)
{
    ui->leCurrentAdcIntercept       -> setEnabled(enable);
    ui->leCurrentAdcSlope           -> setEnabled(enable);
    ui->leCurrentDacIntercept       -> setEnabled(enable);
    ui->leCurrentDacSlope           -> setEnabled(enable);
    ui->leCurrentMaxLimit           -> setEnabled(enable);
    ui->leCurrentMinLimit           -> setEnabled(enable);
    ui->leCurrentStep               -> setEnabled(enable);
    ui->leVoltageAdcIntercept       -> setEnabled(enable);
    ui->leVoltageAdcSlope           -> setEnabled(enable);
    ui->leVoltageDacIntercept       -> setEnabled(enable);
    ui->leVoltageDacSlope           -> setEnabled(enable);
    ui->leVoltageMaxLimit           -> setEnabled(enable);
    ui->leVoltageMinLimit           -> setEnabled(enable);
    ui->leVoltageStep               -> setEnabled(enable);
    ui->leCurrentMonitorResistance  -> setEnabled(enable);
}

void wSisBiasSourceDriver::getEeprom()
{
    auto* _driver = qobject_cast<SisBiasSourceDriverM0*>(driver());

    bool ok = false;
    _driver->eepromConst()->getValueSync(&ok, 5);

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");
}

void wSisBiasSourceDriver::setEeprom()
{
    auto* _driver = qobject_cast<SisBiasSourceDriverM0*>(driver());
    if (!_driver) return;

    bool ok = false;
    CU4BSM0V0_EEPROM_Const_t eepromConst;

    eepromConst.currentAdc.second        = ui->leCurrentAdcIntercept      -> text().toFloat();
    eepromConst.currentAdc.first         = ui->leCurrentAdcSlope          -> text().toFloat();
    eepromConst.currentDac.second        = ui->leCurrentDacIntercept      -> text().toFloat();
    eepromConst.currentDac.first         = ui->leCurrentDacSlope          -> text().toFloat();
    eepromConst.currentLimits.second     = ui->leCurrentMaxLimit          -> text().toFloat();
    eepromConst.currentLimits.first      = ui->leCurrentMinLimit          -> text().toFloat();
    eepromConst.currentStep              = ui->leCurrentStep              -> text().toFloat();
    eepromConst.voltageAdc.second        = ui->leVoltageAdcIntercept      -> text().toFloat();
    eepromConst.voltageAdc.first         = ui->leVoltageAdcSlope          -> text().toFloat();
    eepromConst.voltageDac.second        = ui->leVoltageDacIntercept      -> text().toFloat();
    eepromConst.voltageDac.first         = ui->leVoltageDacSlope          -> text().toFloat();
    eepromConst.voltageLimits.second     = ui->leVoltageMaxLimit          -> text().toFloat();
    eepromConst.voltageLimits.first      = ui->leVoltageMinLimit          -> text().toFloat();
    eepromConst.voltageStep              = ui->leVoltageStep              -> text().toFloat();
    eepromConst.currentMonitorResistance = ui->leCurrentMonitorResistance -> text().toFloat();

    _driver->eepromConst()->setValueSync(eepromConst, &ok, 5);

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't set eeprom constants. Please try one more time!!!");

}

QJsonObject wSisBiasSourceDriver::eepromToJson()
{
    QJsonObject calibrObject;
    QJsonArray value;

    value.append(ui->leCurrentAdcSlope->text().toDouble());
    value.append(ui->leCurrentAdcIntercept->text().toDouble());
    calibrObject["CurrentAdcCoeffs"] = value;

    value[0] = ui->leCurrentDacSlope->text().toDouble();
    value[1] = ui->leCurrentDacIntercept->text().toDouble();
    calibrObject["CurrentDacCoeffs"] = value;

    value[0] = ui->leCurrentMinLimit->text().toDouble();
    value[1] = ui->leCurrentMaxLimit->text().toDouble();
    calibrObject["CurrentLimits"] = value;

    calibrObject["CurrentStep"] = ui->leCurrentStep -> text().toDouble();

    value[0] = ui->leVoltageAdcSlope->text().toDouble();
    value[1] = ui->leVoltageAdcIntercept->text().toDouble();
    calibrObject["VoltageAdcCoeffs"] = value;

    value[0] = ui->leVoltageDacSlope->text().toDouble();
    value[1] = ui->leVoltageDacIntercept->text().toDouble();
    calibrObject["VoltageDacCoeffs"] = value;

    value[0] = ui->leVoltageMinLimit->text().toDouble();
    value[1] = ui->leVoltageMaxLimit->text().toDouble();
    calibrObject["VoltageLimits"] = value;

    calibrObject["VoltageStep"] = ui->leVoltageStep -> text().toDouble();

    calibrObject["CurrentMonitorResistance"] = ui->leCurrentMonitorResistance -> text().toDouble();
    return calibrObject;
}

void wSisBiasSourceDriver::eepromFromJson(QJsonObject data)
{
    QJsonArray value = data["CurrentAdcCoeffs"].toArray();
    ui->leCurrentAdcSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentAdcIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["CurrentDacCoeffs"].toArray();
    ui->leCurrentDacSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentDacIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["CurrentLimits"].toArray();
    ui->leCurrentMinLimit       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentMaxLimit       -> setText(QString::number(value[1].toDouble()));

    ui->leCurrentStep           -> setText(QString::number(data["CurrentStep"].toDouble()));

    value = data["VoltageAdcCoeffs"].toArray();
    ui->leVoltageAdcSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leVoltageAdcIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["VoltageDacCoeffs"].toArray();
    ui->leVoltageDacSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leVoltageDacIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["VoltageLimits"].toArray();
    ui->leVoltageMinLimit       -> setText(QString::number(value[0].toDouble()));
    ui->leVoltageMaxLimit       -> setText(QString::number(value[1].toDouble()));

    ui->leVoltageStep           -> setText(QString::number(data["VoltageStep"].toDouble()));

    ui->leCurrentMonitorResistance -> setText(QString::number(data["CurrentMonitorResistance"].toDouble()));
}
