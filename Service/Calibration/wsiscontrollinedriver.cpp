#include "wsiscontrollinedriver.h"
#include "ui_wsiscontrollinedriver.h"

#include <QDoubleValidator>
#include <QMessageBox>
#include <QJsonArray>
#include "Drivers/siscontrollinedriverm0.h"


wSisControlLineDriver::wSisControlLineDriver(QWidget *parent) :
    DriverWidget(parent),
    ui(new Ui::wSisControlLineDriver)
{
    ui->setupUi(this);

    ui->leCurrentAdcIntercept       -> setValidator(new QDoubleValidator());
    ui->leCurrentAdcSlope           -> setValidator(new QDoubleValidator());
    ui->leCurrentDacIntercept       -> setValidator(new QDoubleValidator());
    ui->leCurrentDacSlope           -> setValidator(new QDoubleValidator());
    ui->leLowerCurrentLimit         -> setValidator(new QDoubleValidator());
    ui->leUpperCurrentLimit         -> setValidator(new QDoubleValidator());
    ui->leCurrentStep               -> setValidator(new QDoubleValidator());
}

wSisControlLineDriver::~wSisControlLineDriver()
{
    delete ui;
}

void wSisControlLineDriver::enableGUI(bool enable)
{
    ui->leCurrentAdcIntercept       -> setEnabled(enable);
    ui->leCurrentAdcSlope           -> setEnabled(enable);
    ui->leCurrentDacIntercept       -> setEnabled(enable);
    ui->leCurrentDacSlope           -> setEnabled(enable);
    ui->leLowerCurrentLimit         -> setEnabled(enable);
    ui->leUpperCurrentLimit         -> setEnabled(enable);
    ui->leCurrentStep               -> setEnabled(enable);
}

void wSisControlLineDriver::getEeprom()
{
    auto* _driver = qobject_cast<SisControlLineDriverM0*>(driver());
    if (!_driver) return;

    bool ok = false;
    _driver->eepromConst()->getValueSync(&ok, 5);

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");

}

void wSisControlLineDriver::setEeprom()
{
    auto* _driver = qobject_cast<SisControlLineDriverM0*>(driver());
    if (!_driver) return;

    bool ok = false;
    CU4CLM0V0_EEPROM_Const_t eepromConst;

    eepromConst.currentAdc.second       = ui->leCurrentAdcIntercept   -> text().toFloat();
    eepromConst.currentAdc.first        = ui->leCurrentAdcSlope       -> text().toFloat();
    eepromConst.currentDac.second       = ui->leCurrentDacIntercept   -> text().toFloat();
    eepromConst.currentDac.first        = ui->leCurrentDacSlope       -> text().toFloat();
    eepromConst.currentLimits.first     = ui->leLowerCurrentLimit     -> text().toFloat();
    eepromConst.currentLimits.second    = ui->leUpperCurrentLimit     -> text().toFloat();
    eepromConst.currentStep             = ui->leCurrentStep           -> text().toFloat();

    _driver->eepromConst()->setValueSync(eepromConst, &ok, 5);

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't set eeprom constants. Please try one more time!!!");
}

QJsonObject wSisControlLineDriver::eepromToJson()
{
    QJsonObject calibrObject;
    QJsonArray value;

    value.append(ui->leCurrentAdcSlope->text().toDouble());
    value.append(ui->leCurrentAdcIntercept->text().toDouble());
    calibrObject["CurrentAdcCoeffs"] = value;

    value[0] = ui->leCurrentDacSlope->text().toDouble();
    value[1] = ui->leCurrentDacIntercept->text().toDouble();
    calibrObject["CurrentDacCoeffs"] = value;

    value[0] = ui->leLowerCurrentLimit->text().toDouble();
    value[1] = ui->leUpperCurrentLimit->text().toDouble();
    calibrObject["CurrentLimits"] = value;

    calibrObject["CurrentStep"] = ui->leCurrentStep -> text().toDouble();
    return calibrObject;
}

void wSisControlLineDriver::eepromFromJson(QJsonObject data)
{
    QJsonArray value = data["CurrentAdcCoeffs"].toArray();
    ui->leCurrentAdcSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentAdcIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["CurrentDacCoeffs"].toArray();
    ui->leCurrentDacSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentDacIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["CurrentLimits"].toArray();
    ui->leLowerCurrentLimit     -> setText(QString::number(value[0].toDouble()));
    ui->leUpperCurrentLimit     -> setText(QString::number(value[1].toDouble()));

    ui->leCurrentStep           -> setText(QString::number(data["CurrentStep"].toDouble()));
}

void wSisControlLineDriver::setDriver(CommonDriver *driver)
{
    auto* _driver = qobject_cast<SisControlLineDriverM0*>(driver);
    if (!_driver) return;

    QObject::connect(_driver,
                     &SisControlLineDriverM0::eepromConstUpdated,
                     [=](const CU4CLM0V0_EEPROM_Const_t eepromConst){
        ui->leCurrentAdcIntercept   -> setText(QString::number(eepromConst.currentAdc.second));
        ui->leCurrentAdcSlope       -> setText(QString::number(eepromConst.currentAdc.first));
        ui->leCurrentDacIntercept   -> setText(QString::number(eepromConst.currentDac.second));
        ui->leCurrentDacSlope       -> setText(QString::number(eepromConst.currentDac.first));
        ui->leLowerCurrentLimit     -> setText(QString::number(eepromConst.currentLimits.first));
        ui->leUpperCurrentLimit     -> setText(QString::number(eepromConst.currentLimits.second));
        ui->leCurrentStep           -> setText(QString::number(eepromConst.currentStep));
    });

    DriverWidget::setDriver(driver);
}
