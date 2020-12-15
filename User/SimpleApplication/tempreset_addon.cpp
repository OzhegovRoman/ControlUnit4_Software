#include <QTime>
#include <QDebug>
#include <QMessageBox>

#include "tempreset_addon.h"
#include "ui_tempreset_addon.h"

TemperatureResetAddon::TemperatureResetAddon(TemperatureRecycleInterface *tempRecycle, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::TemperatureResetAddon)
   {
   ui->setupUi(this);
   setWindowTitle("Temperature recycle settings");

   setTempRecycle(tempRecycle);
   }


TemperatureResetAddon::~TemperatureResetAddon()
   {
   delete ui;
   }

void TemperatureResetAddon::setTempRecycle(TemperatureRecycleInterface *value)
   {
   progressTimer = new QTimer();
   connect(progressTimer,&QTimer::timeout,this,&TemperatureResetAddon::updateProgressBar);

   checkRelaysTimer = new QTimer();
   checkRelaysTimer->setInterval(1000);
   checkRelaysTimer->start();
   connect(checkRelaysTimer,&QTimer::timeout,this,[=](){
      if (mIsRuning && !mTempRecycle->checkIntegrity()){
         changeAlgoritmState(false);
         QMessageBox::critical(this,"Recycle procedure aborted","The temperature module relays have been modified externally.");
         }

      toggleIndicator(ui->L_25vIndicator,mTempRecycle->getRelayState(cRelaysStatus::ri25V));
      toggleIndicator(ui->L_5vIndicator,mTempRecycle->getRelayState(cRelaysStatus::ri5V));
      });

   mTempRecycle = value;
   connect(this->mTempRecycle,&TemperatureRecycleInterface::stateChanged,this,[=](TemperatureRecycleState trs){
      ui->L_State->setText(TemperatureRecycleInterface::toString(trs));
      if (trs == TRS_Idle && mIsRuning)
         changeAlgoritmState(false);
      });
   connect(this->mTempRecycle,&TemperatureRecycleInterface::progress,this,[=](uint32_t progress){
      //      ui->progressBar->setValue(progress);
      if (mIsRuning)
         ui->L_Time->setText(QTime::fromMSecsSinceStartOfDay(mTempRecycle->getElapsed()).toString("mm:ss"));
      else
         ui->L_Time->setText("");
      });

   }

void TemperatureResetAddon::toggleIndicator(QLabel *label, bool isEnabled)
   {
   if (isEnabled)
      label->setStyleSheet("QLabel { background-color: green}");
   else
      label->setStyleSheet("QLabel { background-color: gray}");
   }

void TemperatureResetAddon::toggleInterface(bool isEnabled)
   {
   ui->SB_CoolingDown->setEnabled(isEnabled);
   ui->SB_ThermalizationTime->setEnabled(isEnabled);
   ui->SB_HeatingTime->setEnabled(isEnabled);
   }

void TemperatureResetAddon::mousePressEvent(QMouseEvent *event)
   {
   if (this->childAt(event->pos()) == ui->progressBar){
//      qDebug() << "pressed on Progress bar";
      currentProgress = 0;
      progressTimer->start(10);
      if (mTempRecycle->getCurrentState() == TRS_Idle){
         ui->progressBar->setFormat("Starting Recycle");
         ui->progressBar->setStyleSheet("");
         }
      else
         {
         ui->progressBar->setFormat("Aborting Recycle");
         ui->progressBar->setStyleSheet("QProgressBar { color : red; }");
         }
      }
   }

void TemperatureResetAddon::mouseReleaseEvent(QMouseEvent *event)
   {
//   qDebug() << "released on Progress bar";
   progressTimer->stop();
   ui->progressBar->setValue(0);
   ui->progressBar->setStyleSheet("");
   currentProgress = 0;

   if (mIsRuning){
      ui->progressBar->setFormat("Hold To Abort Recycle");
      }
   else
      {
      ui->progressBar->setFormat("Hold To Start Recycle");
      }
   }

void TemperatureResetAddon::updateProgressBar()
   {
   if (currentProgress == ui->progressBar->maximum()){
      progressTimer->stop();
      currentProgress = 0;
      ui->progressBar->setValue(0);
      progressBarClicked();
      }
   else
      ui->progressBar->setValue(currentProgress+=(ui->progressBar->maximum()/(msecToPress/progressTimer->interval())));
   }

void TemperatureResetAddon::progressBarClicked()
   {
   changeAlgoritmState(mTempRecycle->getCurrentState() == TRS_Idle);
   ui->progressBar->setStyleSheet("");
   }


void TemperatureResetAddon::changeAlgoritmState(bool isRuning)
   {   
   mIsRuning = isRuning;
   toggleInterface(!mIsRuning);
   if (isRuning){
      ui->progressBar->setFormat("Hold To Abort Recycle");
      mTempRecycle->startProcess(ui->SB_HeatingTime->value() * msecInMins,
                                 ui->SB_ThermalizationTime->value() * msecInMins,
                                 ui->SB_CoolingDown->value() * msecInMins);
      setWindowFlags(Qt::Window | Qt::WindowTitleHint);
      show();
      }
   else
      {
      mTempRecycle->abortProcess();
      ui->progressBar->setFormat("Hold To Start Recycle");
      setWindowFlags(Qt::Window
                     | Qt::WindowCloseButtonHint);
      show();
      }
   }
