#ifndef TEMPERATURERESET_ADDON_H
#define TEMPERATURERESET_ADDON_H

#include <QWidget>

#include "advancedrelaycontrol.h"
#include "Drivers/tempdriverm1.h"

namespace Ui {
   class TemperatureResetAddon;
   }

class TemperatureResetAddon : public QWidget
   {
   Q_OBJECT

   QTimer *updateTimer;

   Addon_TemperatureRecycle* tempRecycleAddon;
//   AdvancedRelayControl* mRelay5v;
//   AdvancedRelayControl* mRelay25v;

public:
   explicit TemperatureResetAddon(TempDriverM1 *tDriver, QVector<CommonDriver*> drivers, QWidget *parent = nullptr);
   ~TemperatureResetAddon();

public slots:
   void updateElapsedTime();
   void updateRelayState();
//   void setRelay(bool isOn);
//   void setupReset();

private:
   Ui::TemperatureResetAddon *ui;
   };

#endif // TEMPERATURERESET_ADDON_H
