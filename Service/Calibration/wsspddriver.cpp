#include "wsspddriver.h"
#include "ui_wsspddriver.h"
#include <QDoubleValidator>
#include <QMessageBox>
#include <QJsonArray>

wSspdDriver::wSspdDriver(QWidget *parent) :
    DriverWidget(parent),
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
    ui->leCmpRefDacIntercept    -> setText(QString("%1").arg(eepromConst.Cmp_Ref_DAC.second));
    ui->leCmpRefDacSlope        -> setText(QString("%1").arg(eepromConst.Cmp_Ref_DAC.first));
    ui->leCurrentAdcIntercept   -> setText(QString("%1").arg(eepromConst.Current_ADC.second));
    ui->leCurrentAdcSlope       -> setText(QString("%1").arg(eepromConst.Current_ADC.first));
    ui->leCurrentDacIntercept   -> setText(QString("%1").arg(eepromConst.Current_DAC.second));
    ui->leCurrentDacSlope       -> setText(QString("%1").arg(eepromConst.Current_DAC.first));
    ui->leVoltageAdcIntercept   -> setText(QString("%1").arg(eepromConst.Voltage_ADC.second));
    ui->leVoltageAdcSlope       -> setText(QString("%1").arg(eepromConst.Voltage_ADC.first));
}

void wSspdDriver::setDriver(CommonDriver *driver)
{
    mSspdDriver = qobject_cast<SspdDriverM0*>(driver);
    if (mSspdDriver){
        DriverWidget::setDriver(driver);
        connect(mSspdDriver,
                &SspdDriverM0::eepromConstUpdated,
                this,
                &wSspdDriver::onEepromConstReceived);
    }
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
}

void wSspdDriver::getEeprom()
{
    bool ok = false;
    mSspdDriver->eepromConst()->getValueSync(&ok, 5);
    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");
}

void wSspdDriver::setEeprom()
{
    bool ok = false;
    mSspdDriver->eepromConst()->setValueSync(getEepromConst(), &ok, 5);
    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't set eeprom constants. Please try one more time!!!");
}

QJsonObject wSspdDriver::eepromToJson()
{
    QJsonObject calibrObject;
    QJsonArray value;
    CU4SDM0V1_EEPROM_Const_t eepromConst = getEepromConst();
    value.append(static_cast<double>(eepromConst.Voltage_ADC.first));
    value.append(static_cast<double>(eepromConst.Voltage_ADC.second));
    calibrObject["VoltageAdcCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.Current_ADC.first);
    value[1] = static_cast<double>(eepromConst.Current_ADC.second);
    calibrObject["CurrentAdcCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.Current_DAC.first);
    value[1] = static_cast<double>(eepromConst.Current_DAC.second);
    calibrObject["CurrentDacCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.Cmp_Ref_DAC.first);
    value[1] = static_cast<double>(eepromConst.Cmp_Ref_DAC.second);
    calibrObject["leCmpRefDacCoeffs"] = value;
    return calibrObject;
}

void wSspdDriver::eepromFromJson(QJsonObject data)
{
    CU4SDM0V1_EEPROM_Const_t eepromConst;

    QJsonArray value = data["VoltageAdcCoeffs"].toArray();
    eepromConst.Voltage_ADC.first = static_cast<float>(value[0].toDouble());
    eepromConst.Voltage_ADC.second = static_cast<float>(value[1].toDouble());

    value = data["CurrentAdcCoeffs"].toArray();
    eepromConst.Current_ADC.first = static_cast<float>(value[0].toDouble());
    eepromConst.Current_ADC.second = static_cast<float>(value[1].toDouble());

    value = data["CurrentDacCoeffs"].toArray();
    eepromConst.Current_DAC.first = static_cast<float>(value[0].toDouble());
    eepromConst.Current_DAC.second = static_cast<float>(value[1].toDouble());

    value = data["leCmpRefDacCoeffs"].toArray();
    eepromConst.Cmp_Ref_DAC.first = static_cast<float>(value[0].toDouble());
    eepromConst.Cmp_Ref_DAC.second = static_cast<float>(value[1].toDouble());

    onEepromConstReceived(eepromConst);

}
