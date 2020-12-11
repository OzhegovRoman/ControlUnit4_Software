#include <QTime>

#include "tempreset_addon.h"
#include "ui_tempreset_addon.h"

TemperatureResetAddon::TemperatureResetAddon(TempDriverM1 *tDriver, QVector<CommonDriver*> drivers, QWidget *parent) :
   QWidget(parent),
   tempRecycleAddon(new Addon_TemperatureRecycle(tDriver, drivers, parent)),
   ui(new Ui::TemperatureResetAddon)
   {
   ui->setupUi(this);

   updateTimer = new QTimer();
   updateTimer->setInterval(34);

//   connect(updateTimer, &QTimer::timeout, this, &TemperatureResetAddon::updateElapsedTime);
//   connect(ui->PB_Stop,&QPushButton::clicked,relayControl,&AdvancedRelayControl::stop);
//   connect(ui->PB_ToggleOn,&QPushButton::clicked,this,[=](){
//      ui->PB_ToggleOn->setEnabled(false);
//      relayControl->setRelay(true);});
//   connect(ui->PB_ToggleOff, &QPushButton::clicked,this,[=](){
//      ui->PB_ToggleOff->setEnabled(false);
//      relayControl->setRelay(false);});
//   connect(ui->PB_SetupReset, &QPushButton::clicked, this, &TemperatureResetAddon::setupReset);

   updateTimer->start();
   }

void TemperatureResetAddon::updateElapsedTime()
   {
//   ui->L_Elapsed->setText(QTime::fromMSecsSinceStartOfDay(relayControl->getElapsed()).toString("mm:ss.zzz"));
//   if (relayControl->getRs() == RS_WaitReset)
//      ui->L_RelayState->setStyleSheet("QLabel { background-color: blue}");
//   else if (relayControl->getRs() == RS_WaitSetBack)
//      ui->L_RelayState->setStyleSheet("QLabel { background-color: yellow}");
//   else if (relayControl->getRs() == RS_Idle)
//      ui->L_RelayState->setStyleSheet("QLabel { background-color: white}");
   updateRelayState();
   }

void TemperatureResetAddon::updateRelayState()
   {
//   bool isEnabled = relayControl->isEnabled();
//   if (isEnabled)
//      ui->L_RelayOnOffState->setStyleSheet("QLabel { background-color: Green}");
//   else
//      ui->L_RelayOnOffState->setStyleSheet("QLabel { background-color: Red}");

//   ui->PB_ToggleOff->setVisible(isEnabled);
//   ui->PB_ToggleOn->setVisible(!isEnabled);
   ui->PB_ToggleOff->setEnabled(true);
   ui->PB_ToggleOn->setEnabled(true);
   }

TemperatureResetAddon::~TemperatureResetAddon()
   {
   delete ui;
   }
