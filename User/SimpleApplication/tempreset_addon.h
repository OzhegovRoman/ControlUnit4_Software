#ifndef TEMPERATURERESET_ADDON_H
#define TEMPERATURERESET_ADDON_H

#include <QWidget>
#include <QMouseEvent>
#include <QDialog>
#include <QLabel>

#include "advancedrelaycontrol.h"
#include "temperaturecontrolsettings.h"
#include "Drivers/tempdriverm1.h"

namespace Ui {
   class TemperatureResetAddon;
   }

class TemperatureResetAddon : public QDialog
   {
   Q_OBJECT   

   bool mIsRuning = false;
   uint32_t static const msecInMins = 60*1000;
   TemperatureRecycleInterface* mTempRecycle;
   QTimer* progressTimer;
   QTimer* checkRelaysTimer;
   const uint16_t msecToPress = 600;
   uint8_t currentProgress;

   void applySettings();
   void mousePressEvent(QMouseEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);

public:   
   TemperatureControlSettings* settings;
   Averager avg[4] = {60,60,60,60};

   explicit TemperatureResetAddon(TemperatureRecycleInterface *tempRecycle, QWidget *parent = nullptr);
   ~TemperatureResetAddon();  

   void saveSettings();
   void resetAvg();
   void setDriver(TempDriverM1 *driver);
   void setTempRecycle(TemperatureRecycleInterface *value);
   void toggleIndicator(QLabel* label, bool isEnabled);
   void toggleInterface(bool isEnabled);

   void average(const uint8_t &value);

   TemperatureRecycleState getRecycleState();

public slots:
   void showPreStartMsg();
   void updateProgressBar();
   void progressBarClicked();
   void changeAlgoritmState(bool isRuning);

signals:
   void aborted(bool);

private:
   Ui::TemperatureResetAddon *ui;
   };

#endif // TEMPERATURERESET_ADDON_H
