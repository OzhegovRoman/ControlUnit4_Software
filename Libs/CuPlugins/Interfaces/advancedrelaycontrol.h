#ifndef TEMPRELAYCONTROL_H
#define ADVANCEDRELAYCONTROL_H

#include <QObject>
#include <QTimer>
#include <QPair>

#include "Drivers/tempdriverm1.h"

enum RelayControlType{
   RCT_Idle,
   RCT_SimpleReset,
   RCT_ResetAfterDelay,
   RCT_ResetSchedule,
   _RCT_Count,
   };

enum RelayState {
   RS_Idle = 0,
   RS_WaitReset,
   RS_WaitSetBack
   };

enum TemperatureRecycleState{
   TRS_Idle = 0,
   TRS_SaveSSPDParams,
   TRS_PrepareSSPD,
   TRS_StartRecycle,
   TRS_Wait5VRelay,
   TRS_Wait25VRelay,
   TRS_RestoreSSPDParams,
   _TRS_Count
   };

class AdvancedRelayControl : public QObject
   {
   Q_OBJECT

   TempDriverM1* mTDriver;
   RelayState rs = RS_Idle;
   RelayControlType rct = RCT_Idle;
   const bool mDefaultEnabledState;
   bool mIsEnabled;
   cRelaysStatus::RelayIndex mTargetRelay;
   QTimer* waitBeforeToggle;
   QTimer* waitAfterToggle;

public:
   AdvancedRelayControl(TempDriverM1 *tDriver,
                        cRelaysStatus::RelayIndex targetRelay,
                        bool defaultEnabledState = true,
                        QObject *parent = nullptr);
   ~AdvancedRelayControl();

   void beginRelaySchedule(uint32_t secBeforeToggle, uint32_t secAfterToggle, bool runOnce = true);

   uint32_t getElapsed();
   RelayState getRs() const;
   void setRs(const RelayState &value);
   bool isEnabled() const;

public slots:
   bool checkState();
   void setRelay(bool enable);
   void temporarySetRelayOff(uint32_t secBeforeToggleback = 0);
   void stop();
   };



class Addon_TemperatureRecycle : public QObject
   {
   QTimer* progressReport;

   AdvancedRelayControl* mRelay5v;
   AdvancedRelayControl* mRelay25v;

   QList<CommonDriver*> mDrivers;
   QMap<CommonDriver*, double> SSPDParams;

public:
   Addon_TemperatureRecycle(TempDriverM1 *tDriver,
                            QVector<CommonDriver*> drivers,
                            QObject *parent);

   void saveSSPDParams();
   void prepareSSPD();
   void startRecycle(uint32_t switchOff5VMins, uint32_t switchOff25VMins);
   void restoreSSPDParams();

public slots:
   uint8_t progress();

signals:
   void stateChanged(TemperatureRecycleState);

   };

#endif // ADVANCEDRELAYCONTROL_H
