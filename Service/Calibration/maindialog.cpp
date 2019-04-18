#include "maindialog.h"
#include "ui_maindialog.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDebug>

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog),
    mDriverType(tDriverType::UNKNOWN),
    mDriver(nullptr),
    mInterface(nullptr),
    mSaveDialog(new SaveDialog(this)),
    mOpenDialog(new OpenDialog(this)),
    mCalibrate(new CalibrateDialog(this))
{
    ui->setupUi(this);
}

MainDialog::~MainDialog()
{
    delete ui;
}

// get EEPROM
void MainDialog::on_pbGetEeprom_clicked()
{
    disableGUI();
    switch (mDriverType) {
    case tDriverType::CU4SD:{
        bool ok = false;
        qobject_cast<cCu4SdM0Driver*>(mDriver)->eepromConst()->getValueSequence(&ok, 5);

        if (!ok)
            QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");
        break;
    }

    case tDriverType::CU4TD: {
        bool ok = false;
        cCu4TdM0Driver* lDriver = qobject_cast<cCu4TdM0Driver*>(mDriver);

        lDriver->eepromConst()->getValueSequence(&ok, 5);

        if (!ok) {
            QMessageBox::warning(this, "Warning!!!", "Can't read eeprom data");
            enableGUI();
            return;
        }

        qApp->processEvents();
        ok = lDriver->receiveTempTable();
        if (!ok)
            QMessageBox::warning(this, "Warning!!!", "Can't read table");
        else
            ui->wTemp->setTempTable(qobject_cast<cCu4TdM0Driver*>(mDriver)->tempTable());
        break;
    }

    default:
        break;
    }
    enableGUI();
}

//set EEPROM
void MainDialog::on_pbSetEeprom_clicked()
{
    disableGUI();
    switch (mDriverType) {
    case tDriverType::CU4SD:{
        bool ok = false;
        qobject_cast<cCu4SdM0Driver*>(mDriver)->eepromConst()->setValueSequence(ui->wSspd->getEepromConst(), &ok, 5);
        if (!ok)
            QMessageBox::warning(this, "Warning!!!","Can't set eeprom constants. Please try one more time!!!");
        break;
    }
    case tDriverType::CU4TD:{
        bool ok = false;
        cCu4TdM0Driver* lDriver = qobject_cast<cCu4TdM0Driver*>(mDriver);
        lDriver->eepromConst()->setValueSequence(ui->wTemp->getEepromConst(), &ok, 5);
        if (!ok){
            QMessageBox::warning(this, "Warning!!!","Can't set eeprom constants. Please try one more time!!!");
            enableGUI();
            return;
        }

        CU4TDM0V1_Temp_Table_Item_t *tTable = ui->wTemp->getTempTable();
        for (int i = 0; i < TEMP_TABLE_SIZE; ++i){
            lDriver->tempTable()[i] = tTable[i];
        }
        ok = lDriver->sendTempTable();
    }
    default:
        break;
    }
    enableGUI();
}

//write EEPROM
void MainDialog::on_pbWriteEeprom_clicked()
{
    disableGUI();
    bool ok = false;
    int cnt = 0;
    while ((!ok) && (cnt++<5)){
        mDriver->writeEeprom();
        ok = mDriver->waitingAnswer();
    }
    if (!ok)
        QMessageBox::warning(this, "Warning!!!","Can't write eeprom. Please try one more time!!!");
    enableGUI();
}

// Start calibrate
void MainDialog::on_pbCalibrate_clicked()
{
    mCalibrate->setDriver(mDriver);
    mCalibrate->setDriverType(mDriverType);
    int result = mCalibrate->exec();
    if (result == -1){
        //не смогли найти драйвера visa32
        QMessageBox::warning(this, "Warning!!!", "There is no any driver for visa32. Calibration process will be blocked");
    }
    on_pbGetEeprom_clicked();
}

void MainDialog::on_pbSave_clicked()
{
    QJsonObject calibrObject;
    bool ok;
    calibrObject["DeviceType"] = mDriver->getDeviceType()->getValueSequence(&ok);
    calibrObject["UDID"] = mDriver->getUDID()->getValueSequence(&ok).toString();
    switch (mDriverType) {
    case tDriverType::CU4SD: {
        QJsonArray value;
        CU4SDM0V1_EEPROM_Const_t eepromConst = ui->wSspd->getEepromConst();
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
        break;
    }
    case tDriverType::CU4TD: {
        QJsonArray value;
        CU4TDM0V1_EEPROM_Const_t eepromConst = ui->wTemp->getEepromConst();
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
        CU4TDM0V1_Temp_Table_Item_t *tTable = ui->wTemp->getTempTable();
        for (int i = 0; i< TEMP_TABLE_SIZE; ++i){
            value[0] = static_cast<double>(tTable[i].Voltage);
            value[1] = static_cast<double>(tTable[i].Temperature);
            table.append(value);
        }
        calibrObject["TempTable"] = table;
        break;
    }
    default:
        QMessageBox::warning(nullptr,"Warning","Unknown type of Device");
        return;
        break;
    };

    mSaveDialog->setJsonData(calibrObject);
    mSaveDialog->exec();
}

void MainDialog::on_pbLoad_clicked()
{
    mOpenDialog->setDeviceName(mDriver->getUDID()->getValueSequence().toString());
    mOpenDialog->setDeviceType(mDriver->getDeviceType()->getValueSequence());

    if (!mOpenDialog->exec()) return;

    QJsonObject json = mOpenDialog->jsonData();

    switch (mDriverType) {
    case tDriverType::CU4SD: {
        CU4SDM0V1_EEPROM_Const_t eepromConst;

        QJsonArray value = json["VoltageAdcCoeffs"].toArray();
        eepromConst.Voltage_ADC.first = static_cast<float>(value[0].toDouble());
        eepromConst.Voltage_ADC.second = static_cast<float>(value[1].toDouble());

        value = json["CurrentAdcCoeffs"].toArray();
        eepromConst.Current_ADC.first = static_cast<float>(value[0].toDouble());
        eepromConst.Current_ADC.second = static_cast<float>(value[1].toDouble());

        value = json["CurrentDacCoeffs"].toArray();
        eepromConst.Current_DAC.first = static_cast<float>(value[0].toDouble());
        eepromConst.Current_DAC.second = static_cast<float>(value[1].toDouble());

        value = json["leCmpRefDacCoeffs"].toArray();
        eepromConst.Cmp_Ref_DAC.first = static_cast<float>(value[0].toDouble());
        eepromConst.Cmp_Ref_DAC.second = static_cast<float>(value[1].toDouble());

        ui->wSspd->onEepromConstReceived(eepromConst);
        break;
    }
    case tDriverType::CU4TD: {
        CU4TDM0V1_EEPROM_Const_t eepromConst;

        QJsonArray value = json["PressSensorCoeffs"].toArray();
        eepromConst.pressSensorCoeffs.first = static_cast<float>(value[0].toDouble());
        eepromConst.pressSensorCoeffs.second = static_cast<float>(value[1].toDouble());

        value = json["PressSensorVoltagePCoeffs"].toArray();
        eepromConst.pressSensorVoltageP.first = static_cast<float>(value[0].toDouble());
        eepromConst.pressSensorVoltageP.second = static_cast<float>(value[1].toDouble());

        value = json["PressSensorVoltageNCoeffs"].toArray();
        eepromConst.pressSensorVoltageN.first = static_cast<float>(value[0].toDouble());
        eepromConst.pressSensorVoltageN.second = static_cast<float>(value[1].toDouble());

        value = json["TempSensorCurrentADCCoeffs"].toArray();
        eepromConst.tempSensorCurrentAdc.first = static_cast<float>(value[0].toDouble());
        eepromConst.tempSensorCurrentAdc.second = static_cast<float>(value[1].toDouble());

        value = json["TempSensorCurrentDACCoeffs"].toArray();
        eepromConst.tempSensorCurrentDac.first = static_cast<float>(value[0].toDouble());
        eepromConst.tempSensorCurrentDac.second = static_cast<float>(value[1].toDouble());

        value = json["TempSensorVoltageCoeffs"].toArray();
        eepromConst.tempSensorVoltage.first = static_cast<float>(value[0].toDouble());
        eepromConst.tempSensorVoltage.second = static_cast<float>(value[1].toDouble());

        ui->wTemp->onEepromConstReceived(eepromConst);

        QJsonArray table = json["TempTable"].toArray();
        CU4TDM0V1_Temp_Table_Item_t mTable[TEMP_TABLE_SIZE];
        for (int i = 0; i< TEMP_TABLE_SIZE; ++i){
            value = table[i].toArray();
            mTable[i].Voltage = static_cast<float>(value[0].toDouble());
            mTable[i].Temperature = static_cast<float>(value[1].toDouble());
        }
        ui->wTemp->setTempTable(mTable);
        break;
    }
    default:
        break;
    }
}

void MainDialog::setDeviceAddress(int deviceAddress)
{
    mDeviceAddress = deviceAddress;
}

void MainDialog::enableGUI(bool enable)
{
    ui->pbCalibrate->setEnabled(enable);
    ui->pbClose->setEnabled(enable);
    ui->pbGetEeprom->setEnabled(enable);
    ui->pbLoad->setEnabled(enable);
    ui->pbSave->setEnabled(enable);
    ui->pbSetEeprom->setEnabled(enable);
    ui->pbWriteEeprom->setEnabled(enable);
    ui->stackedWidget->setEnabled(enable);
    qApp->processEvents();
}

void MainDialog::setDeviceType(const QString &deviceType)
{
    mDriverType = tDriverType::UNKNOWN;
    if (deviceType.contains("CU4SD"))
        mDriverType = tDriverType::CU4SD;
    if (deviceType.contains("CU4TD"))
        mDriverType = tDriverType::CU4TD;
}

void MainDialog::initializeUI()
{
    switch (mDriverType){
    case tDriverType::CU4SD:
        ui->stackedWidget->setCurrentIndex(0);
        mDriver = new cCu4SdM0Driver(this);
        connect(mDriver, SIGNAL(eepromConstUpdated(CU4SDM0V1_EEPROM_Const_t)),
                ui->wSspd, SLOT(onEepromConstReceived(CU4SDM0V1_EEPROM_Const_t)));
        break;
    case tDriverType::CU4TD:
        ui->stackedWidget->setCurrentIndex(1);
        mDriver = new cCu4TdM0Driver(this);
        connect(mDriver, SIGNAL(eepromConstUpdated(CU4TDM0V1_EEPROM_Const_t)),
                ui->wTemp, SLOT(onEepromConstReceived(CU4TDM0V1_EEPROM_Const_t)));
        break;
    default:
        QMessageBox::warning(nullptr,"Warning","Unknown type of Device");
        exit(0);
        return;
        break;
    }

    mDriver->setIOInterface(mInterface);
    mDriver->setDevAddress(static_cast<quint8>(mDeviceAddress));

    on_pbGetEeprom_clicked();
    ui->stackedWidget->setCurrentIndex(static_cast<int>(mDriverType));
}

void MainDialog::setInterface(cuIOInterfaceImpl *interface)
{
    mInterface = interface;
}
