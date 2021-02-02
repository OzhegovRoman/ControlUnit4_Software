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
   mDefaultEnabledState(defaultEnabledState)
   {
   waitBeforeToggle = new QTimer(this);
   waitAfterToggle = new QTimer(this);
   waitAfterToggle->setInterval(10000);
   waitAfterToggle->setSingleShot(true);

   mIsEnabled = !(mDefaultEnabledState ^ getRelayState());

   connect(waitAfterToggle, &QTimer::timeout, this, [=](){
      setRelay(true);
      setRs((waitBeforeToggle->isActive())?RS_WaitReset:RS_Idle);
      emit processFinished();
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

bool AdvancedRelayControl::getRelayState()
   {
   bool rv = false;
   bool ok;
   cRelaysStatus status = mTDriver->relaysStatus()->getValueSync(&ok);
   if (ok){
      rv = status.status() & mTargetRelay;
      }
   return rv;
   }

void AdvancedRelayControl::temporarySetRelayOff(uint32_t timeBeforeToggleback)
   {
   setRelay(false);
   setRs(RS_WaitSetBack);
   if (timeBeforeToggleback == 0){
      waitAfterToggle->start();
      }
   else{
      waitAfterToggle->start(timeBeforeToggleback);
      }
   }

void AdvancedRelayControl::beginRelaySchedule(uint32_t timeBeforeToggle,
                                              uint32_t timeAfterToggle,
                                              bool runOnce)
   {
   if (timeBeforeToggle < 500)
      timeBeforeToggle = 500;
   stop();
   waitBeforeToggle->setSingleShot(runOnce);
   setRs(RS_WaitReset);
   connect(waitBeforeToggle, &QTimer::timeout, this, [=](){
      waitBeforeToggle->setInterval((timeAfterToggle+timeBeforeToggle));
      temporarySetRelayOff(timeAfterToggle);
      });
   waitBeforeToggle->start(timeBeforeToggle);
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
   bool rv = true;
   bool relayState = getRelayState();

   if (relayState == !(mDefaultEnabledState ^ isEnabled()) ){
      rv = true;
      remainingFails = 2;
      }
   else
      if (remainingFails-- <= 0)
         rv = false;

   //   qDebug() << remainingFails << relayState << mDefaultEnabledState << mIsEnabled << rv;
   return rv;
   }

RelayState AdvancedRelayControl::getRs() const
   {
   return rs;
   }

void AdvancedRelayControl::setRs(const RelayState &value)
   {
   rs = value;
   //   qDebug() << "Rs: " << rs;
   }

void TemperatureRecycleInterface::setDrivers(QVector<CommonDriver *> *drivers)
   {
   mDrivers = drivers;
   }

TemperatureRecycleState TemperatureRecycleInterface::getCurrentState() const
   {
   return currentState;
   }

bool TemperatureRecycleInterface::getRelayState(cRelaysStatus::RelayIndex ri)
   {
   switch (ri) {
      case cRelaysStatus::ri25V: return mRelay25v->getRelayState();
      case cRelaysStatus::ri5V:  return mRelay5v->getRelayState();
      }
   }

void TemperatureRecycleInterface::setCurrentState(const TemperatureRecycleState &value)
   {
   currentState = value;
   }

bool TemperatureRecycleInterface::checkIntegrity()
   {
   //   qDebug() << "Check 5V";
   bool rv = mRelay5v->checkState();
   //   qDebug() << "Check 25V";
   rv &= mRelay25v->checkState();
   return rv;
   }

void TemperatureRecycleInterface::setParameters(const uint32_t &HeatingTime,
                                                const uint32_t &ThermalizationTime,
                                                const uint32_t &CoolingDownTime)
   {
   mHeatingTime =          HeatingTime      ;
   mThermalizationTime =   ThermalizationTime;
   mCoolingDownTime =      CoolingDownTime  ;
   }

QString TemperatureRecycleInterface::toString(TemperatureRecycleState trs)
   {
   switch (trs) {
      case TRS_Idle:              { return QString(""); } break;
      case TRS_SaveSSPDParams:    { return QString("Saving Params"); } break;
      case TRS_PrepareSSPD:       { return QString("Setup SSPD"); } break;
      case TRS_StartRecycle:      { return QString("Starting Recycle"); } break;
      case TRS_Heating:           { return QString("Heating"); } break;
      case TRS_Thermalization:    { return QString("Thermalization"); } break;
      case TRS_CoolingDown:       { return QString("Cooling Down"); } break;
      case TRS_RestoreSSPDParams: { return QString("Finish Recycling"); } break;
      case TRS_Aborted:           { return QString("Process Aborted"); } break;
      default: return QString("");
      }
   }

void TemperatureRecycleInterface::startProcess(uint32_t HeatingTime, uint32_t ThermalizationTime, uint32_t CoolingDownTime)
   {
   emit stateChanged(TRS_StartRecycle);
   currentProgress = 0;
   saveSSPDParams();
   prepareSSPD();
   startRecycle(HeatingTime, ThermalizationTime, CoolingDownTime);
   }

TemperatureRecycleInterface::TemperatureRecycleInterface(TempDriverM1 *tDriver,
                                                         QVector<CommonDriver *> *drivers,
                                                         QObject *parent):
   mRelay5v(new AdvancedRelayControl(tDriver,cRelaysStatus::ri5V,true,this)),
   mRelay25v(new AdvancedRelayControl(tDriver,cRelaysStatus::ri25V,false,this)),
   waitCoolingDownTimer(new QTimer(this)),
   progressReportTimer(new QTimer(this)),
   mDrivers(drivers),
   currentProgress(0),
   maxProgress(10000),
   //   fullTime({0,0}),
   currentState(TRS_Idle),
   QObject(parent)
   {
   waitCoolingDownTimer->setSingleShot(true);
   connect(waitCoolingDownTimer,&QTimer::timeout,this,&TemperatureRecycleInterface::restoreSSPDParams);
   connect(this->progressReportTimer,&QTimer::timeout,this,&TemperatureRecycleInterface::reportProgress);
   connect(this,&TemperatureRecycleInterface::stateChanged,this,[=](TemperatureRecycleState trs){currentState = trs;});
   connect(mRelay5v,&AdvancedRelayControl::processFinished,this,&TemperatureRecycleInterface::startWaitCoolingDown);
   connect(mRelay25v,&AdvancedRelayControl::processFinished,this,[=](){emit stateChanged(TRS_Thermalization);});
   }

void TemperatureRecycleInterface::saveSSPDParams()
   {
   emit stateChanged(TRS_SaveSSPDParams);
   for(auto a : *mDrivers) {
      auto driver = qobject_cast<SspdDriverM1*>(a);
      if (driver){
         QPair<double, bool> temp = {0,true};
         bool ok = false;
         double current = driver->current()->getValueSync(&ok);
         if (ok)
            temp.first = current;
         bool isShorted = driver->status()->getValueSync(&ok).stShorted;
         if (ok)
            temp.second = isShorted;
         SSPDParams.insert(a,temp);
         }
      else
         {
         auto driver = qobject_cast<SspdDriverM0*>(a);
         if (driver){
            QPair<double, bool> temp = {0,true};
            bool ok = false;
            double current = driver->current()->getValueSync(&ok);
            if (ok)
               temp.first = current;
            bool isShorted = driver->status()->getValueSync(&ok).stShorted;
            if (ok)
               temp.second = isShorted;
            SSPDParams.insert(a,temp);
            }
         }
      }
   }

void TemperatureRecycleInterface::prepareSSPD()
   {
   emit stateChanged(TRS_PrepareSSPD);
   for(auto a : *mDrivers) {
      auto driver = qobject_cast<SspdDriverM1*>(a);
      if (driver){
         driver->shortEnable()->setValueSync(true, nullptr, 5);
         }
      else
         {
         auto driver = qobject_cast<SspdDriverM0*>(a);
         if (driver){
            driver->shortEnable()->setValueSync(true, nullptr, 5);
            }
         }
      }
   }

void TemperatureRecycleInterface::restoreSSPDParams()
   {
   if (currentState != TRS_Aborted)
      emit stateChanged(TRS_RestoreSSPDParams);
   for(auto a : *mDrivers) {
      auto driver = qobject_cast<SspdDriverM1*>(a);
      if (driver){
         driver->shortEnable()->setValueSync(SSPDParams.value(driver).second, nullptr, 5);
         }
      else
         {
         auto driver = qobject_cast<SspdDriverM0*>(a);
         if (driver){
            driver->shortEnable()->setValueSync(SSPDParams.value(driver).second, nullptr, 5);
            }
         }
      }
   currentProgress = 0;
   emit stateChanged(TRS_Idle);
   emit progress(currentProgress);
   }

void TemperatureRecycleInterface::startRecycle(uint32_t HeatingTime,
                                               uint32_t ThermalizationTime,
                                               uint32_t CoolingDownTime)
   {
   emit stateChanged(TRS_Heating);
   setParameters(HeatingTime, ThermalizationTime, CoolingDownTime);
   startRecycle();
   progressReportTimer->start(200);
   }

void TemperatureRecycleInterface::startRecycle()
   {
   mRelay25v->temporarySetRelayOff(mHeatingTime);
   mRelay5v->temporarySetRelayOff(mThermalizationTime+mHeatingTime);
   }

void TemperatureRecycleInterface::startWaitCoolingDown()
   {
   emit stateChanged(TRS_CoolingDown);
   waitCoolingDownTimer->start(mCoolingDownTime);
   }

void TemperatureRecycleInterface::abortProcess()
   {
//   emit stateChanged(TRS_Aborted);
   waitCoolingDownTimer->stop();
   progressReportTimer->stop();
   mRelay5v->stop();
   mRelay25v->stop();
   restoreSSPDParams();
   }

uint32_t TemperatureRecycleInterface::getElapsed()
   {
   uint32_t rv = 0;
   uint32_t relay5Elapsed = mRelay5v->getElapsed();
   uint32_t relay25Elapsed = mRelay25v->getElapsed();
   uint32_t waitCoolingDown = waitCoolingDownTimer->remainingTime();
   if (relay25Elapsed > 0)
      rv = relay25Elapsed;
   else if (relay5Elapsed > 0)
      rv = relay5Elapsed;
   else if (waitCoolingDown > 0)
      rv = waitCoolingDown;

   return rv;
   }

uint8_t TemperatureRecycleInterface::reportProgress()
   {
   uint32_t relay5Elapsed = mRelay5v->getElapsed();
   uint32_t relay25Elapsed = mRelay25v->getElapsed();

   currentProgress = maxProgress / (mHeatingTime+mThermalizationTime+mCoolingDownTime)/progressReportTimer->interval();

   //   emit stateChanged(currentState);
   emit progress(currentProgress);
   }

void Averager::setMaxLen(const uint32_t &maxLen)
   {
   mMaxLen = maxLen;
   }

Averager::Averager(uint32_t maxLen):
   mMaxLen(maxLen)
   {
   reset();
   }

double Averager::average(double val)
   {
   double rv = 0;
   mSum+=val;
   buffer.append(val);
   if (buffer.count() > mMaxLen){
      mSum -= buffer.takeFirst();
      }
   rv = getAvg();
   if (buffer.count() > (mMaxLen/2))
      calcTrend(rv);

   return rv;
   }

double Averager::getTrend() const
   {
   return mTrend;
   }

void Averager::calcTrend(double val)
   {
   avgBuffer.append(val);
   if (avgBuffer.count() > mMaxLen){
      avgBuffer.removeFirst();
      }
   mTrend = avgBuffer.last() - avgBuffer.first();
   }

double Averager::getAvg() const {
   return mSum/buffer.count();
   }

void Averager::reset(){
   buffer.clear();
   avgBuffer.clear();
   mSum = 0;
   mTrend = 0;
   }

