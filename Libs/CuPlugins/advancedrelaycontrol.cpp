#include "advancedrelaycontrol.h"


AdvancedRelayControl::AdvancedRelayControl(TempDriverM1 *tDriver, cRelaysStatus::RelayIndex targetRelay,
                                           bool defaultEnabledState):
   mTDriver(tDriver),
   mTargetRelay(targetRelay),
   mDefaultEnabledState(defaultEnabledState),
   isEnabled(defaultEnabledState)
   {
   waitBeforeToggle = new QTimer();
   waitAfterToggle = new QTimer(10000);
   waitAfterToggle.setSingleShot(true);

   connect(waitAfterToggle, &QTimer::timeout, this, [=](){setRelay(true);});
   }

RelayControlState AdvancedRelayControl::getRelayState()
   {

   }


void AdvancedRelayControl::setRelay(bool enable)
   {
   cRelaysStatus status;
   isEnabled = enable;
   status.setStatus(mTargetRelay, (enable == mDefaultEnabledState) ? mTargetRelay : 0);
   mTDriver->relaysStatus()->setValueSync(status, nullptr, 5);
   }

void AdvancedRelayControl::temporarySetRelayOff(uint32_t secBeforeToggleback)
   {
   setRelay(false);
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
   waitAfterToggle->setInterval(secBeforeToggleback);
   waitBeforeToggle->setSingleShot(runOnce);
   connect(waitBeforeToggle, &QTimer::timeout, this, &AdvancedRelayControl::temporarySetRelayOff);
   }
