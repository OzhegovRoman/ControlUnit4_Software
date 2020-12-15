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
   TRS_Heating,
   TRS_Thermalization,
   TRS_CoolingDown,
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
   int remainingFails = 2;
   cRelaysStatus::RelayIndex mTargetRelay;
   QTimer* waitBeforeToggle;
   QTimer* waitAfterToggle;

public:   
   AdvancedRelayControl(TempDriverM1 *tDriver,
                        cRelaysStatus::RelayIndex targetRelay,
                        bool defaultEnabledState = true,
                        QObject *parent = nullptr);
   ~AdvancedRelayControl();

   void beginRelaySchedule(uint32_t timeBeforeToggle, uint32_t timeAfterToggle, bool runOnce = true);

   uint32_t getElapsed();
   RelayState getRs() const;
   void setRs(const RelayState &value);
   bool isEnabled() const;

public slots:
   bool checkState();
   void setRelay(bool enable);
   bool getRelayState();
   void temporarySetRelayOff(uint32_t timeBeforeToggleback = 0);
   void stop();

signals:
   void processFinished();
   };



class TemperatureRecycleInterface : public QObject
   {
   Q_OBJECT

   QTimer* progressReportTimer;
   QTimer* waitCoolingDownTimer;
   uint32_t currentProgress;
   TemperatureRecycleState currentState;
   uint32_t maxProgress;
//   uint32_t fullTime[2];

   uint32_t mHeatingTime = 0;
   uint32_t mThermalizationTime = 0;
   uint32_t mCoolingDownTime = 0;

   AdvancedRelayControl* mRelay5v;
   AdvancedRelayControl* mRelay25v;

   QVector<CommonDriver*> *mDrivers;
   QMap<CommonDriver*, QPair<double,bool>> SSPDParams;

public:
   TemperatureRecycleInterface(TempDriverM1 *tDriver,
                            QVector<CommonDriver *> *drivers,
                            QObject *parent);

   static QString toString(TemperatureRecycleState trs);


   void startProcess(uint32_t HeatingTime = 22,
                     uint32_t ThermalizationTime = 12,
                     uint32_t CoolingDownTime = 25);
   void saveSSPDParams();
   void prepareSSPD();
   void startRecycle();
   void startRecycle(uint32_t HeatingTime,
                     uint32_t ThermalizationTime,
                     uint32_t CoolingDownTime);
   void abortProcess();

   void setDrivers(QVector<CommonDriver *> *drivers);
   uint32_t getElapsed();

   TemperatureRecycleState getCurrentState() const;
   bool getRelayState(cRelaysStatus::RelayIndex ri);
   void setCurrentState(const TemperatureRecycleState &value);
   bool checkIntegrity();

   void setParameters(const uint32_t &HeatingTime,
                      const uint32_t &ThermalizationTime,
                      const uint32_t &CoolingDownTime);

public slots:
   void restoreSSPDParams();
   void startWaitCoolingDown();
   uint8_t reportProgress();

signals:
   void stateChanged(TemperatureRecycleState);
   void progress(uint32_t);

   };

#endif // ADVANCEDRELAYCONTROL_H
