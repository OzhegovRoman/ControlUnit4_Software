#ifndef TEMPRELAYCONTROL_H
#define ADVANCEDRELAYCONTROL_H

#include <QObject>
#include <QTimer>
#include <QPair>

#include "Drivers/tempdriverm1.h"

enum RelayControlType{
   RCT_SimpleReset = 0,
   RCT_ResetAfterDelay,
   RCT_ResetSchedule,
   _RCT_Count,
   };

//struct RelayControlState{
//   bool isInReset;
//   RelayControlType rct;
//   QPair<uint32_t,uint32_t> actualAndMaxElapsed;
//   };

class AdvancedRelayControl
   {
   Q_OBJECT

   const bool mDefaultEnabledState;
   bool isEnabled;
   TempDriverM1* mTDriver;
   cRelaysStatus::RelayIndex mTargetRelay;
   QTimer* waitBeforeToggle;
   QTimer* waitAfterToggle;

public:
   AdvancedRelayControl(TempDriverM1 *tDriver,
                        cRelaysStatus::RelayIndex targetRelay,
                        bool defaultState = true); //TODO: выяснить реальноесостояние "по-умолчанию"

   void beginRelaySchedule(bool targetState, uint32_t secBeforeToggle, uint32_t secAfterToggle, bool runOnce = true);
   void stopRelaySchedule();

   RelayControlState getRelayState();

public slots:
   void setRelay(bool enable);
   void temporarySetRelayOff(uint32_t secBeforeToggleback = 0);
   };

#endif // ADVANCEDRELAYCONTROL_H
