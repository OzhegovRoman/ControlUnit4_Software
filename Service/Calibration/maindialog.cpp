#include "maindialog.h"
#include "ui_maindialog.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QLayout>

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog),
    mDriverType(CU4DriverType::dtUnknown),
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
    mDriverWidget->getEeprom();
    enableGUI();
}

//set EEPROM
void MainDialog::on_pbSetEeprom_clicked()
{
    disableGUI();
    mDriverWidget->setEeprom();
    enableGUI();
}

//write EEPROM
void MainDialog::on_pbWriteEeprom_clicked()
{
    disableGUI();
    bool ok = false;
    mDriver->writeEeprom()->executeSync(&ok, 5);

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
    QJsonObject calibrObject = mDriverWidget->eepromToJson();;
    bool ok;
    calibrObject["DeviceType"] = mDriver->deviceType()->getValueSync(&ok);
    calibrObject["UDID"] = mDriver->UDID()->getValueSync(&ok).toString();
    mSaveDialog->setJsonData(calibrObject);
    mSaveDialog->exec();
}

void MainDialog::on_pbLoad_clicked()
{
    mOpenDialog->setDeviceName(mDriver->UDID()->getValueSync().toString());
    mOpenDialog->setDeviceType(mDriver->deviceType()->getValueSync());

    if (!mOpenDialog->exec()) return;

    mDriverWidget->eepromFromJson(mOpenDialog->jsonData());
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
    mDriverWidget->enableGUI(enable);
    qApp->processEvents();
}

void MainDialog::setDeviceType(const QString &deviceType)
{
    mDriverType = CU4DriverType::dtUnknown;
    if (deviceType.contains("CU4SDM0") || deviceType.contains("CU4SDM1")) // пока разницы нет между SDM0 и SDM1
        mDriverType = CU4DriverType::dtSspdDriverM0;
    if (deviceType.contains("CU4TDM0"))
        mDriverType = CU4DriverType::dtTempDriverM0;
    if (deviceType.contains("CU4TDM1"))
        mDriverType = CU4DriverType::dtTempDriverM1;
}

void MainDialog::initializeUI()
{
    switch (mDriverType){
    case CU4DriverType::dtSspdDriverM0:
    {
        mDriver = new SspdDriverM0(this);
        mDriverWidget = new wSspdDriver(this);
        break;
    }
    case CU4DriverType::dtTempDriverM0:
        mDriver = new TempDriverM0(this);
        mDriverWidget = new wTempCalibr(this);
        break;
    case CU4DriverType::dtTempDriverM1:
        mDriverWidget = new wTempM1Calibr(this);
        mDriver = new TempDriverM1(this);
        break;
    default:
        QMessageBox::warning(nullptr,"Warning","Unknown type of Device");
        exit(0);
    }

    mDriverWidget->setDriver(mDriver);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    QVBoxLayout *vbox = new QVBoxLayout();

    hbox->addWidget(mDriverWidget);
    hbox->addWidget(ui->vline);
    vbox->addWidget(ui->pbCalibrate);
    vbox->addWidget(ui->hline);
    vbox->addWidget(ui->pbSave);
    vbox->addWidget(ui->pbLoad);
    vbox->addWidget(ui->hline_2);
    vbox->addWidget(ui->pbGetEeprom);
    vbox->addWidget(ui->pbSetEeprom);
    vbox->addWidget(ui->pbWriteEeprom);
    vbox->addStretch(1);
    vbox->addWidget(ui->pbClose);
    hbox->addLayout(vbox);
    setLayout(hbox);

    mDriver->setIOInterface(mInterface);
    mDriver->setDevAddress(static_cast<quint8>(mDeviceAddress));
    setWindowTitle(QString("%1 EEPROM data")
                   .arg(mDriver->deviceType()->getValueSync(nullptr, 5)));

    on_pbGetEeprom_clicked();
}

void MainDialog::setInterface(cuIOInterfaceImpl *interface)
{
    mInterface = interface;
}
