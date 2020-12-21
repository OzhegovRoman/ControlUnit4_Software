#include "tempm1widget.h"
#include "ui_tempm1widget.h"
#include <QDebug>
#include <QTime>
#include <QDialog>
#include <QSettings>


TempM1Widget::TempM1Widget(QWidget *parent)
   : CommonWidget(parent)
   , ui(new Ui::TempM1Widget)
   {
   ui->setupUi(this);

   mTimer.setSingleShot(true);
   connect(&mTimer, &QTimer::timeout, this, &TempM1Widget::onTimerTicker);

   connect(&watcher, &QTimer::timeout, this, &TempM1Widget::checkTemperature);
   }

TempM1Widget::~TempM1Widget()
   {
   tempReset->saveSettings();
   saveSettings();
   delete ui;
   }

void TempM1Widget::updateWidget()
   {

   }

void TempM1Widget::openWidget()
   {
   mDriver->readDefaultParams();
   watcher.start(1000);
   onTimerTicker();
   }

void TempM1Widget::closeWidget()
   {
   mTimer.stop();
   }

void TempM1Widget::setTempReset(TemperatureRecycleInterface *value)
   {
   tempReset = new TemperatureResetAddon(value);
   tempReset->setModal(true);
   tempReset->setStyleSheet(ui->cb5V->styleSheet());
   connect(ui->PB_RecycleTemp,&QPushButton::clicked,tempReset,[=](){
      tempReset->show();
      });
   connect(tempReset,&TemperatureResetAddon::aborted, ui->CB_TemperatureControl, &QCheckBox::setChecked);
   connect(ui->CB_TemperatureControl, &QCheckBox::toggled, &watcher, [=](bool isChecked){
      if (isChecked){
         tempReset->resetAvg();
         watcher.start();
         }
      else
         watcher.stop();
      });
   QSettings settings("Scontel", "cu-simpleapp");
   ui->CB_SelectedTempSensor->setCurrentIndex(settings.value("tempSensorIndex",0).toUInt());
   ui->DSB_TempThreshold->setValue(settings.value("autoRecycleTreshold",1.5).toReal());
   }

void TempM1Widget::setDriver(TempDriverM1 *driver)
   {
   mDriver = driver;
   }

void TempM1Widget::checkTemperature()
   {
   mDriver->updateTemperature();
   for (int i = 0; i < 4; ++i) {
      tempReset->avg[i].average(mDriver->currentTemperature(i));
      }
   tempReset->toggleIndicator(ui->L_isOperating, (tempReset->avg[0].getAvg() < 10));

   QSettings settings("Scontel", "cu-simpleapp");
   if (ui->CB_TemperatureControl->isChecked()
       && tempReset->avg[0].getAvg() < settings.value("workTemperatureT1",10).toReal()
       && tempReset->getRecycleState() == TRS_Idle)
      {
      uint8_t sensorIdx = ui->CB_SelectedTempSensor->currentText().right(1).toInt();
      if (tempReset->avg[sensorIdx].getAvg() > ui->DSB_TempThreshold->value()
          && tempReset->avg[sensorIdx].getTrend() > 0){
         tempReset->showPreStartMsg();
         }
      }
   }

void TempM1Widget::saveSettings()
   {
   QSettings settings("Scontel", "cu-simpleapp");
   if (settings.value("workTemperatureT1",-1).toInt() == -1)
      settings.setValue("workTemperatureT1",10.);
   settings.setValue("tempSensorIndex", ui->CB_SelectedTempSensor->currentIndex());
   settings.setValue("autoRecycleTreshold", ui->DSB_TempThreshold->value());
   }

void TempM1Widget::onTimerTicker()
   {
   QString strTemperature, strVoltage;
   if (!ui->CB_TemperatureControl->isChecked())
      mDriver->updateTemperature();
   mDriver->updateVoltage();

   for (int i = 0; i < 4; ++i){
      if (mDriver->defaultParam(i).enable){
         strTemperature += QString("T%1: %2K\n").arg(i) .arg(mDriver->currentTemperature(i), 4, 'f', 2);
         strVoltage += QString("U%1: %2V\n").arg(i) .arg(mDriver->currentVoltage(i), 0, 'g', 5);
         }
      }

   strTemperature.chop(1);
   strVoltage.chop(1);
   ui->lbTemperature->setText(strTemperature);
   ui->lbVoltage->setText(strVoltage);

   mDriver->relaysStatus()->getValueSync();
   ui->cb25V->setChecked(mDriver->relaysStatus()->currentValue()[0]);
   ui->cb5V->setChecked(mDriver->relaysStatus()->currentValue()[1]);

//   checkTemperature();
   mTimer.start(1000);
   }


void TempM1Widget::on_cb5V_clicked(bool checked)
   {
   cRelaysStatus status;
   status.setStatus(cRelaysStatus::ri5V, checked ? cRelaysStatus::ri5V : 0);
   mDriver->relaysStatus()->setValueSync(status, nullptr, 5);
   }

void TempM1Widget::on_cb25V_clicked(bool checked)
   {
   cRelaysStatus status;
   status.setStatus(cRelaysStatus::ri25V, checked ? cRelaysStatus::ri25V : 0);
   mDriver->relaysStatus()->setValueSync(status, nullptr, 5);
   }



