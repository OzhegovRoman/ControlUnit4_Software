#include "wtempcalibr.h"
#include "ui_wtempcalibr.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonArray>

wTempCalibr::wTempCalibr(QWidget *parent) :
    DriverWidget(parent),
    ui(new Ui::wTempCalibr)
{
    ui->setupUi(this);
    ui->leTempSensorCurrentADCslope->setValidator(new QDoubleValidator());
    ui->leTempSensorCurrentADCintercept->setValidator(new QDoubleValidator());
    ui->leTempSensorCurrentDACslope->setValidator(new QDoubleValidator());
    ui->leTempSensorCurrentDACintercept->setValidator(new QDoubleValidator());
    ui->leTempSensorVoltageslope->setValidator(new QDoubleValidator());
    ui->leTempSensorVoltageintercept->setValidator(new QDoubleValidator());
    ui->lePressSensorVoltagePslope->setValidator(new QDoubleValidator());
    ui->lePressSensorVoltagePintercept->setValidator(new QDoubleValidator());
    ui->lePressSensorVoltageNslope->setValidator(new QDoubleValidator());
    ui->lePressSensorVoltageNintercept->setValidator(new QDoubleValidator());
    ui->lePressSensorslope->setValidator(new QDoubleValidator());
    ui->lePressSensorintercept->setValidator(new QDoubleValidator());

    ui->twTempTable->setRowCount(TEMP_TABLE_SIZE);
    for (int i = 0; i< TEMP_TABLE_SIZE; ++i){
        ui->twTempTable->setItem(i,0, new QTableWidgetItem(""));
        ui->twTempTable->setItem(i,1, new QTableWidgetItem(""));
    }
}

wTempCalibr::~wTempCalibr()
{
    delete ui;
}

CU4TDM0V1_EEPROM_Const_t wTempCalibr::getEepromConst()
{
    CU4TDM0V1_EEPROM_Const_t eepromConst;
    eepromConst.tempSensorCurrentAdc.first   = ui->leTempSensorCurrentADCslope->text().toFloat();
    eepromConst.tempSensorCurrentAdc.second  = ui->leTempSensorCurrentADCintercept->text().toFloat();
    eepromConst.tempSensorCurrentDac.first   = ui->leTempSensorCurrentDACslope->text().toFloat();
    eepromConst.tempSensorCurrentDac.second  = ui->leTempSensorCurrentDACintercept->text().toFloat();
    eepromConst.tempSensorVoltage.first      = ui->leTempSensorVoltageslope->text().toFloat();
    eepromConst.tempSensorVoltage.second     = ui->leTempSensorVoltageintercept->text().toFloat();
    eepromConst.pressSensorVoltageP.first    = ui->lePressSensorVoltagePslope->text().toFloat();
    eepromConst.pressSensorVoltageP.second   = ui->lePressSensorVoltagePintercept->text().toFloat();
    eepromConst.pressSensorVoltageN.first    = ui->lePressSensorVoltageNslope->text().toFloat();
    eepromConst.pressSensorVoltageN.second   = ui->lePressSensorVoltageNintercept->text().toFloat();
    eepromConst.pressSensorCoeffs.first      = ui->lePressSensorslope->text().toFloat();
    eepromConst.pressSensorCoeffs.second     = ui->lePressSensorintercept->text().toFloat();
    return eepromConst;
}

CU4TDM0V1_Temp_Table_Item_t *wTempCalibr::getTempTable()
{
    for (int i = 0; i < TEMP_TABLE_SIZE; ++i){
        mTempTable[i].Voltage     = ui->twTempTable->item(i,0)->text().toFloat();
        mTempTable[i].Temperature = ui->twTempTable->item(i,1)->text().toFloat();
    }
    return mTempTable;
}

void wTempCalibr::onEepromConstReceived(CU4TDM0V1_EEPROM_Const_t eepromConst)
{
    ui->leTempSensorCurrentADCslope     -> setText(QString("%1").arg(eepromConst.tempSensorCurrentAdc.first));
    ui->leTempSensorCurrentADCintercept -> setText(QString("%1").arg(eepromConst.tempSensorCurrentAdc.second));
    ui->leTempSensorCurrentDACslope     -> setText(QString("%1").arg(eepromConst.tempSensorCurrentDac.first));
    ui->leTempSensorCurrentDACintercept -> setText(QString("%1").arg(eepromConst.tempSensorCurrentDac.second));
    ui->leTempSensorVoltageslope        -> setText(QString("%1").arg(eepromConst.tempSensorVoltage.first));
    ui->leTempSensorVoltageintercept    -> setText(QString("%1").arg(eepromConst.tempSensorVoltage.second));
    ui->lePressSensorVoltagePslope      -> setText(QString("%1").arg(eepromConst.pressSensorVoltageP.first));
    ui->lePressSensorVoltagePintercept  -> setText(QString("%1").arg(eepromConst.pressSensorVoltageP.second));
    ui->lePressSensorVoltageNslope      -> setText(QString("%1").arg(eepromConst.pressSensorVoltageN.first));
    ui->lePressSensorVoltageNintercept  -> setText(QString("%1").arg(eepromConst.pressSensorVoltageN.second));
    ui->lePressSensorslope              -> setText(QString("%1").arg(eepromConst.pressSensorCoeffs.first));
    ui->lePressSensorintercept          -> setText(QString("%1").arg(eepromConst.pressSensorCoeffs.second));
}

void wTempCalibr::setTempTable(CU4TDM0V1_Temp_Table_Item_t *tempTable)
{
    for (int i = 0; i < TEMP_TABLE_SIZE; ++i){
        ui->twTempTable->item(i,0)->setText(QString("%1").arg(tempTable[i].Voltage));
        ui->twTempTable->item(i,1)->setText(QString("%1").arg(tempTable[i].Temperature));
    }
}

void wTempCalibr::on_pbLoadFromFile_clicked()
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
            ui->twTempTable->item(i, 0)->setText(strL[0]);
            ui->twTempTable->item(i, 1)->setText(strL[1]);
        }
    }
    file.close();
}

void wTempCalibr::setDriver(CommonDriver *driver)
{
    mTempDriver = qobject_cast<TempDriverM0*>(driver);
    if (mTempDriver){
        DriverWidget::setDriver(driver);
        connect(mTempDriver,
                &TempDriverM0::eepromConstUpdated,
                this,
                &wTempCalibr::onEepromConstReceived);
    }
}

void wTempCalibr::enableGUI(bool enable)
{
    ui->leTempSensorCurrentADCslope     -> setEnabled(enable);
    ui->leTempSensorCurrentADCintercept -> setEnabled(enable);
    ui->leTempSensorCurrentDACslope     -> setEnabled(enable);
    ui->leTempSensorCurrentDACintercept -> setEnabled(enable);
    ui->leTempSensorVoltageslope        -> setEnabled(enable);
    ui->leTempSensorVoltageintercept    -> setEnabled(enable);
    ui->lePressSensorVoltagePslope      -> setEnabled(enable);
    ui->lePressSensorVoltagePintercept  -> setEnabled(enable);
    ui->lePressSensorVoltageNslope      -> setEnabled(enable);
    ui->lePressSensorVoltageNintercept  -> setEnabled(enable);
    ui->lePressSensorslope              -> setEnabled(enable);
    ui->lePressSensorintercept          -> setEnabled(enable);
}

void wTempCalibr::getEeprom()
{
    bool ok = false;
    mTempDriver->eepromConst()->getValueSync(&ok, 5);
    if (!ok) {
        QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");
        return;
    }
    ok = mTempDriver->receiveTempTable();

    if (!ok)
        QMessageBox::warning(this, "Warning!!!", "Can't read table");
    else
        setTempTable(mTempDriver->tempTable());
}

void wTempCalibr::setEeprom()
{
    bool ok = false;
    mTempDriver->eepromConst()->setValueSync(getEepromConst(), &ok, 5);
    if (!ok){
        QMessageBox::warning(this, "Warning!!!","Can't set eeprom constants. Please try one more time!!!");
        return;
    }

    getTempTable();
    for (int i = 0; i < TEMP_TABLE_SIZE; ++i){
        mTempDriver->tempTable()[i] = mTempTable[i];
    }
    ok = mTempDriver->sendTempTable();
}

QJsonObject wTempCalibr::eepromToJson()
{
    QJsonObject calibrObject;
    QJsonArray value;
    CU4TDM0V1_EEPROM_Const_t eepromConst = getEepromConst();
    value.append(static_cast<double>(eepromConst.pressSensorCoeffs.first));
    value.append(static_cast<double>(eepromConst.pressSensorCoeffs.second));
    calibrObject["PressSensorCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.pressSensorVoltageP.first);
    value[1] = static_cast<double>(eepromConst.pressSensorVoltageP.second);
    calibrObject["PressSensorVoltagePCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.pressSensorVoltageN.first);
    value[1] = static_cast<double>(eepromConst.pressSensorVoltageN.second);
    calibrObject["PressSensorVoltageNCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.tempSensorCurrentAdc.first);
    value[1] = static_cast<double>(eepromConst.tempSensorCurrentAdc.second);
    calibrObject["TempSensorCurrentADCCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.tempSensorCurrentDac.first);
    value[1] = static_cast<double>(eepromConst.tempSensorCurrentDac.second);
    calibrObject["TempSensorCurrentDACCoeffs"] = value;
    value[0] = static_cast<double>(eepromConst.tempSensorVoltage.first);
    value[1] = static_cast<double>(eepromConst.tempSensorVoltage.second);
    calibrObject["TempSensorVoltageCoeffs"] = value;
    QJsonArray table;
    CU4TDM0V1_Temp_Table_Item_t *tTable = getTempTable();
    for (int i = 0; i< TEMP_TABLE_SIZE; ++i){
        value[0] = static_cast<double>(tTable[i].Voltage);
        value[1] = static_cast<double>(tTable[i].Temperature);
        table.append(value);
    }
    calibrObject["TempTable"] = table;
    return calibrObject;

}

void wTempCalibr::eepromFromJson(QJsonObject data)
{
    CU4TDM0V1_EEPROM_Const_t eepromConst;

    QJsonArray value = data["PressSensorCoeffs"].toArray();
    eepromConst.pressSensorCoeffs.first = static_cast<float>(value[0].toDouble());
    eepromConst.pressSensorCoeffs.second = static_cast<float>(value[1].toDouble());

    value = data["PressSensorVoltagePCoeffs"].toArray();
    eepromConst.pressSensorVoltageP.first = static_cast<float>(value[0].toDouble());
    eepromConst.pressSensorVoltageP.second = static_cast<float>(value[1].toDouble());

    value = data["PressSensorVoltageNCoeffs"].toArray();
    eepromConst.pressSensorVoltageN.first = static_cast<float>(value[0].toDouble());
    eepromConst.pressSensorVoltageN.second = static_cast<float>(value[1].toDouble());

    value = data["TempSensorCurrentADCCoeffs"].toArray();
    eepromConst.tempSensorCurrentAdc.first = static_cast<float>(value[0].toDouble());
    eepromConst.tempSensorCurrentAdc.second = static_cast<float>(value[1].toDouble());

    value = data["TempSensorCurrentDACCoeffs"].toArray();
    eepromConst.tempSensorCurrentDac.first = static_cast<float>(value[0].toDouble());
    eepromConst.tempSensorCurrentDac.second = static_cast<float>(value[1].toDouble());

    value = data["TempSensorVoltageCoeffs"].toArray();
    eepromConst.tempSensorVoltage.first = static_cast<float>(value[0].toDouble());
    eepromConst.tempSensorVoltage.second = static_cast<float>(value[1].toDouble());

    onEepromConstReceived(eepromConst);

    QJsonArray table = data["TempTable"].toArray();
    CU4TDM0V1_Temp_Table_Item_t mTable[TEMP_TABLE_SIZE];
    for (int i = 0; i< TEMP_TABLE_SIZE; ++i){
        value = table[i].toArray();
        mTable[i].Voltage = static_cast<float>(value[0].toDouble());
        mTable[i].Temperature = static_cast<float>(value[1].toDouble());
    }
    setTempTable(mTable);
}
