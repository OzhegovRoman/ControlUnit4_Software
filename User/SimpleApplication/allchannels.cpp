#include "allchannels.h"
#include "ui_allchannels.h"
#include <QDebug>
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
#include "Drivers/heaterdriverm0.h"
#include <QSettings>
#include <QDir>
#include <QDateTime>
#include <QFileDialog>

AllChannels::AllChannels(QWidget *parent)
    : CommonWidget(parent)
    , ui(new Ui::AllChannels)
    , model(new AllChannelsDataModel(this))
    , delegate(new AllChannelsDataDelegate(this))
{
    ui->setupUi(this);

    QSettings settings("Scontel", "cu-simpleapp");
    ui->cbLogEnable->setChecked(settings.value("isLogEnable", false).toBool());
    QString tmpPath = settings.value("LogPath", QDir::currentPath()).toString();
    if (!QDir(tmpPath).exists()) tmpPath = QDir::currentPath();
    ui->leLogPath->setText(tmpPath);

}

AllChannels::~AllChannels()
{
    QSettings settings("Scontel", "cu-simpleapp");
    settings.setValue("isLogEnable", ui->cbLogEnable->isChecked());
    settings.setValue("LogPath", ui->leLogPath->text());
    delete ui;
}

void AllChannels::openWidget()
{
    emit setTimeOut(static_cast<int>(ui->dsbTimeOut->value()*1000.0));
}

void AllChannels::closeWidget()
{
    emit setTimeOut(500);
}

void AllChannels::updateWidget()
{
    // опрос всех устройств
    for (int idx = 0; idx < model->drivers.count(); ++idx){
        {
            auto driver = qobject_cast<SspdDriverM0*>(model->drivers[idx]);
            if (driver){
                bool ok;
                CU4SDM0V1_Data_t data = driver->data()->getValueSync(&ok, 5);
                if (ok){
                    if (data.Status.stAutoResetOn && ui->cbLogEnable->isChecked()){
                        auto lastTriggerCount = driver->params()->currentValue().AutoResetCounts;
                        auto param = driver->params()->getValueSync(&ok, 5);
                        if (ok && (param.AutoResetCounts != lastTriggerCount)){
                            QFile m_File(QString("%1\\TriggerLog.txt").arg(ui->leLogPath->text()));
                            m_File.open(QIODevice::ReadWrite | QIODevice::Append);
                            QTextStream out(&m_File);
                            QString tmpStr = QString("[%1]: SSPD Unit Triggered. Adress: %2. Trigger counts: %3\r\n")
                                    .arg(QDateTime::currentDateTime().toString("MM-dd-yyyy HH-mm-ss"))
                                    .arg(driver->devAddress())
                                    .arg(param.AutoResetCounts);
                            qDebug()<<tmpStr;
                            out<<tmpStr;
                            m_File.close();
                        }
                    }
                }

            }
        }
        {
            auto driver = qobject_cast<SspdDriverM1*>(model->drivers[idx]);
            if (driver){
                bool ok;
                CU4SDM1_Data_t data = driver->data()->getValueSync(&ok, 5);
                if (ok){
                    if (data.Status.stAutoResetOn && ui->cbLogEnable->isChecked()){
                        auto lastTriggerCount = driver->params()->currentValue().AutoResetCounts;
                        auto param = driver->params()->getValueSync(&ok, 5);
                        if (ok && (param.AutoResetCounts != lastTriggerCount)){
                            QFile m_File(QString("%1\\TriggerLog.txt").arg(ui->leLogPath->text()));
                            m_File.open(QIODevice::ReadWrite | QIODevice::Append);
                            QTextStream out(&m_File);
                            QString tmpStr = QString("[%1]: SSPD Unit Triggered. Adress: %2. Trigger counts: %3\r\n")
                                    .arg(QDateTime::currentDateTime().toString("MM-dd-yyyy HH-mm-ss"))
                                    .arg(driver->devAddress())
                                    .arg(param.AutoResetCounts);
                            qDebug()<<tmpStr;
                            out<<tmpStr;
                            m_File.close();
                        }
                    }
                }

            }
        }
        {
            auto driver = qobject_cast<TempDriverM0*>(model->drivers[idx]);
            if (driver){
                CU4TDM0V1_Data_t data = driver->data()->getValueSync(nullptr, 5);
                if (!data.CommutatorOn) driver->commutator()->setValueSync(true, nullptr, 5);
            }
        }

        {
            auto driver = qobject_cast<TempDriverM1*>(model->drivers[idx]);
            if (driver){
                QString tmpString = QString();
                driver->updateTemperature();
            }
        }
        {
            auto driver = qobject_cast<HeaterDriverM0*>(model->drivers[idx]);
            if (driver){
                QString tmpString = QString();
            }
        }
    }
    emit model->dataChanged(model->index(0), model->index(model->rowCount()));
}

void AllChannels::initialize(const QVector<CommonDriver*>& mDrivers)
{
    model->initialize(mDrivers);
    ui->tableView->setModel(model);
    delegate->setModel(model);
    ui->tableView->setItemDelegate(delegate);
}

void AllChannels::on_pbSetUpdateTime_clicked()
{
    openWidget();
}

void AllChannels::setInterface(cuIOInterface *interface)
{
    mInterface = interface;
    delegate->setInterface(mInterface);
}

void AllChannels::on_pbLogPath_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, "Open Directory for Log file...");
    if (!str.isEmpty()) ui->leLogPath->setText(str);
}
