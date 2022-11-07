#include "testcontrol.h"
#include "ui_testcontrol.h"
#include <QDebug>
#include <QMessageBox>
#include "Drivers/commondriver.h"
#include <QThread>

TestControl::TestControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestControl)
{
    ui->setupUi(this);
}

TestControl::~TestControl()
{
    delete ui;
}

void TestControl::setDeviceList(const QStringList &newDeviceList)
{
    mDeviceList = newDeviceList;
}

void TestControl::setInterface(cuIOInterface *newInterface)
{
    mInterface = newInterface;
}


int TestControl::exec()
{
    ui->lStatusControlUnit->setText("Не проверено");
    ui->lStatusMatrix->setText("Не проверено");
    ui->lStatusProcessingUnit->setText("Не проверено");

    ui->tbControlUnit->setIcon(QIcon(":/images/testcontrol/error.png"));
    ui->tbProcessor->setIcon(QIcon(":/images/testcontrol/error.png"));
    ui->tbMatrix->setIcon(QIcon(":/images/testcontrol/error.png"));
    return QDialog::exec();
}

void TestControl::on_pushButton_2_clicked()
{

//    this->setDisabled(true);
    qDebug()<<"start test";
    qDebug()<<"Device List"<<mDeviceList;

    qDebug()<<"Блок смещения";

    CommonDriver driver;
    driver.setIOInterface(mInterface);
    bool ok;
    ui->tbControlUnit->setIcon(QIcon(":/images/testcontrol/wait.png"));
    ui->lStatusControlUnit->setText("подготовка");

    qApp->processEvents();

    for (int i = 0; i < mDeviceList.count(); i++){
        int address = mDeviceList[i].split(": ")[1].toInt();
        driver.setDevAddress(address);

        driver.deviceType()->getValueSync(&ok, 5);
        qApp->thread()->msleep(500);
    }
    ui->tbControlUnit->setIcon(QIcon(":/images/testcontrol/ready.png"));
    ui->lStatusControlUnit->setText("готов");

    ui->tbProcessor->setIcon(QIcon(":/images/testcontrol/wait.png"));
    ui->lStatusProcessingUnit->setText("подготовка");

    qApp->processEvents();

    qApp->thread()->msleep(1000);
    ui->tbProcessor->setIcon(QIcon(":/images/testcontrol/ready.png"));
    ui->lStatusProcessingUnit->setText("готов");

    ui->tbMatrix->setIcon(QIcon(":/images/testcontrol/wait.png"));
    ui->lStatusMatrix->setText("подготовка");

    qApp->processEvents();

    qApp->thread()->msleep(1000);
    ui->tbMatrix->setIcon(QIcon(":/images/testcontrol/ready.png"));
    ui->lStatusMatrix->setText("готов");

    this->setEnabled(true);
}

