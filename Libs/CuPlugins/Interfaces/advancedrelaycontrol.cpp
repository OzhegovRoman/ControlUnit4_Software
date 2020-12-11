#include <QTime>
#include <QDebug>

#include "advancedrelaycontrol.h"

#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"


AdvancedRelayControl::AdvancedRelayControl(TempDriverM1 *tDriver,
                                           cRelaysStatus::RelayIndex targetRelay,
                                           bool defaultEnabledState,
                                           QObject *parent):
   QObject(parent),
   mTDriver(tDriver),
   mTargetRelay(targetRelay),
   mDefaultEnabledState(defaultEnabledState),
   mIsEnabled(defaultEnabledState)
   {
   waitBeforeToggle = new QTimer(this);
   waitAfterToggle = new QTimer(this);
   waitAfterToggle->setInterval(10000);
   waitAfterToggle->setSingleShot(true);

   connect(waitAfterToggle, &QTimer::timeout, this, [=](){
      setRelay(true);
      setRs((waitBeforeToggle->isActive())?RS_WaitReset:RS_Idle);
      });
   }

AdvancedRelayControl::~AdvancedRelayControl()
   {
   stop();
   }

void AdvancedRelayControl::setRelay(bool enable)
   {
   cRelaysStatus status;
   mIsEnabled = enable;
   status.setStatus(mTargetRelay, (enable == mDefaultEnabledState) ? mTargetRelay : 0);
   mTDriver->relaysStatus()->setValueSync(status, nullptr, 5);
   }

void AdvancedRelayControl::temporarySetRelayOff(uint32_t secBeforeToggleback)
   {
   setRelay(false);
   setRs(RS_WaitSetBack);
   if (secBeforeToggleback == 0){
      waitAfterToggle->start();
      }
   else{
      waitAfterToggle->start(secBeforeToggleback*1000);
      }
   }

void AdvancedRelayControl::beginRelaySchedule(uint32_t secBeforeToggle,
                                              uint32_t secAfterToggle,
                                              bool runOnce)
   {
   if (secBeforeToggle < 1)
      secBeforeToggle = 1;
   stop();
   waitBeforeToggle->setSingleShot(runOnce);
   setRs(RS_WaitReset);
   connect(waitBeforeToggle, &QTimer::timeout, this, [=](){
      waitBeforeToggle->setInterval((secAfterToggle+secBeforeToggle)*1000);
      temporarySetRelayOff(secAfterToggle);
      });
   waitBeforeToggle->start(secBeforeToggle*1000);
   }

void AdvancedRelayControl::stop()
   {
   setRelay(true);
   waitBeforeToggle->stop();
   waitAfterToggle->stop();
   setRs(RS_Idle);
   }

uint32_t AdvancedRelayControl::getElapsed()
   {
   uint32_t rv = 0;
   switch(getRs()){
      case RS_Idle: break;
      case RS_WaitReset:   {rv = waitBeforeToggle->remainingTime(); } break;
      case RS_WaitSetBack: {rv = waitAfterToggle->remainingTime();  } break;
      default: break;
      }
   return rv;
   }

bool AdvancedRelayControl::isEnabled() const
   {
   return mIsEnabled;
   }

bool AdvancedRelayControl::checkState()
   {
   bool rv = false;
   bool ok;
   cRelaysStatus status = mTDriver->relaysStatus()->getValueSync(&ok);
   if (ok){
      bool relayState = status.status() & mTargetRelay;
      if (relayState == !(mDefaultEnabledState ^ mIsEnabled) )
         rv = true;
      }
   return rv;
   }

RelayState AdvancedRelayControl::getRs() const
   {
   return rs;
   }

void AdvancedRelayControl::setRs(const RelayState &value)
   {
   rs = value;
   qDebug() << "Rs: " << rs;
   }

Addon_TemperatureRecycle::Addon_TemperatureRecycle(TempDriverM1 *tDriver,
                                                   QVector<CommonDriver*> drivers,
                                                   QObject *parent):
   mRelay5v(new AdvancedRelayControl(tDriver,cRelaysStatus::ri5V,true,this)),
   mRelay25v(new AdvancedRelayControl(tDriver,cRelaysStatus::ri25V,false,this)),
   mDrivers(drivers.toList()),
   QObject(parent)
   {

   }

void Addon_TemperatureRecycle::saveSSPDParams()
   {
   for(auto a : mDrivers) {
      auto driver = qobject_cast<SspdDriverM1*>(a);
      if (driver){
         bool ok = false;
         double current = driver->current()->getValueSync(&ok);
         if (ok)
            {
            SSPDParams[a] = current;
            }
         }
      }
   }

void Addon_TemperatureRecycle::prepareSSPD()
   {
   for(auto a : mDrivers) {
      auto driver = qobject_cast<SspdDriverM1*>(a);
      if (driver){
//         bool ok = false;
         driver->autoResetEnable()->setValueSync(false, nullptr, 5);
         driver->shortEnable()->setValueSync(true, nullptr, 5);
         }
      }
   }

void Addon_TemperatureRecycle::startRecycle(uint32_t switchOff5VMins, uint32_t switchOff25VMins)
   {
   mRelay5v->temporarySetRelayOff(switchOff5VMins*60);
   mRelay25v->temporarySetRelayOff(switchOff25VMins*60);
   uint32_t maxTime = (switchOff5VMins>switchOff25VMins)?switchOff5VMins:switchOff25VMins;
   QTimer::singleShot(maxTime*60*1000,this,restoreSSPDParams());
   }

void Addon_TemperatureRecycle::restoreSSPDParams()
   {
   for(auto a : mDrivers) {
      auto driver = qobject_cast<SspdDriverM1*>(a);
      if (driver){
//         bool ok = false;
         driver->autoResetEnable()->setValueSync(true, nullptr, 5);
         driver->shortEnable()->setValueSync(false, nullptr, 5);
         }
      }
   }

uint8_t Addon_TemperatureRecycle::progress()
   {

   }


