#include "wheaterdriver.h"
#include "ui_wheaterdriver.h"
#include <QDoubleValidator>
#include <QMessageBox>
#include <QJsonArray>
#include "Drivers/heaterdriverm0.h"

wHeaterDriver::wHeaterDriver(QWidget *parent) :
    DriverWidget(parent),
    ui(new Ui::wHeaterDriver)
{
    ui->setupUi(this);

    ui->leCurrentAdcIntercept       -> setValidator(new QDoubleValidator());
    ui->leCurrentAdcSlope           -> setValidator(new QDoubleValidator());
    ui->leCurrentDacIntercept       -> setValidator(new QDoubleValidator());
    ui->leCurrentDacSlope           -> setValidator(new QDoubleValidator());
    ui->leMaximumCurrent            -> setValidator(new QDoubleValidator());
    ui->leFrontEdge                 -> setValidator(new QDoubleValidator());
    ui->leHoldTime                  -> setValidator(new QDoubleValidator());
    ui->leRearEdge                  -> setValidator(new QDoubleValidator());
}

wHeaterDriver::~wHeaterDriver()
{
    delete ui;
}

void wHeaterDriver::enableGUI(bool enable)
{
    ui->leCurrentAdcIntercept       -> setEnabled(enable);
    ui->leCurrentAdcSlope           -> setEnabled(enable);
    ui->leCurrentDacIntercept       -> setEnabled(enable);
    ui->leCurrentDacSlope           -> setEnabled(enable);
    ui->leMaximumCurrent            -> setEnabled(enable);
    ui->leFrontEdge                 -> setEnabled(enable);
    ui->leHoldTime                  -> setEnabled(enable);
    ui->leRearEdge                  -> setEnabled(enable);
}

void wHeaterDriver::getEeprom()
{
    auto* _driver = qobject_cast<HeaterDriverM0*>(driver());
    if (!_driver) return;

    bool ok = false;
    _driver->eepromConst()->getValueSync(&ok, 5);

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");
}

void wHeaterDriver::setEeprom()
{
    auto* _driver = qobject_cast<HeaterDriverM0*>(driver());
    if (!_driver) return;

    bool ok = false;
    CU4HT0V0_EEPROM_Const_t eepromConst;

    eepromConst.currentAdc.second   = ui->leCurrentAdcIntercept   -> text().toFloat();
    eepromConst.currentAdc.first    = ui->leCurrentAdcSlope       -> text().toFloat();
    eepromConst.currentDac.second   = ui->leCurrentDacIntercept   -> text().toFloat();
    eepromConst.currentDac.first    = ui->leCurrentDacSlope       -> text().toFloat();
    eepromConst.maximumCurrent      = ui->leMaximumCurrent        -> text().toFloat();
    eepromConst.frontEdgeTime       = ui->leFrontEdge             -> text().toFloat();
    eepromConst.holdTime            = ui->leHoldTime              -> text().toFloat();
    eepromConst.rearEdgeTime        = ui->leRearEdge              -> text().toFloat();

    _driver->eepromConst()->setValueSync(eepromConst, &ok, 5);

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't set eeprom constants. Please try one more time!!!");


}

QJsonObject wHeaterDriver::eepromToJson()
{
    QJsonObject calibrObject;
    QJsonArray value;

    value.append(ui->leCurrentAdcSlope->text().toDouble());
    value.append(ui->leCurrentAdcIntercept->text().toDouble());
    calibrObject["CurrentAdcCoeffs"] = value;

    value[0] = ui->leCurrentDacSlope->text().toDouble();
    value[1] = ui->leCurrentDacIntercept->text().toDouble();
    calibrObject["CurrentDacCoeffs"] = value;

    calibrObject["MaximumCurrent"] = ui->leMaximumCurrent        -> text().toDouble();
    calibrObject["FrontEdgeTime "] = ui->leFrontEdge             -> text().toDouble();
    calibrObject["HoldTime      "] = ui->leHoldTime              -> text().toDouble();
    calibrObject["RearEdgeTime  "] = ui->leRearEdge              -> text().toDouble();
    return calibrObject;
}

void wHeaterDriver::eepromFromJson(QJsonObject data)
{
    QJsonArray value = data["CurrentAdcCoeffs"].toArray();
    ui->leCurrentAdcSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentAdcIntercept   -> setText(QString::number(value[1].toDouble()));

    value = data["CurrentDacCoeffs"].toArray();
    ui->leCurrentDacSlope       -> setText(QString::number(value[0].toDouble()));
    ui->leCurrentDacIntercept   -> setText(QString::number(value[1].toDouble()));

    ui->leMaximumCurrent        -> setText(QString::number(data["MaximumCurrent"].toDouble()));
    ui->leFrontEdge             -> setText(QString::number(data["FrontEdgeTime "].toDouble()));
    ui->leHoldTime              -> setText(QString::number(data["HoldTime      "].toDouble()));
    ui->leRearEdge              -> setText(QString::number(data["RearEdgeTime  "].toDouble()));

}

void wHeaterDriver::setDriver(CommonDriver *driver)
{
    auto* _driver = qobject_cast<HeaterDriverM0*>(driver);
    if (!_driver) return;

    QObject::connect(_driver,
                     &HeaterDriverM0::eepromConstUpdated,
                     [=](const CU4HT0V0_EEPROM_Const_t eepromConst){
        ui->leCurrentAdcIntercept   -> setText(QString::number(eepromConst.currentAdc.second));
        ui->leCurrentAdcSlope       -> setText(QString::number(eepromConst.currentAdc.first));
        ui->leCurrentDacIntercept   -> setText(QString::number(eepromConst.currentDac.second));
        ui->leCurrentDacSlope       -> setText(QString::number(eepromConst.currentDac.first));
        ui->leMaximumCurrent        -> setText(QString::number(eepromConst.maximumCurrent));
        ui->leFrontEdge             -> setText(QString::number(eepromConst.frontEdgeTime));
        ui->leHoldTime              -> setText(QString::number(eepromConst.holdTime));
        ui->leRearEdge              -> setText(QString::number(eepromConst.rearEdgeTime));
    });

    DriverWidget::setDriver(driver);
}
