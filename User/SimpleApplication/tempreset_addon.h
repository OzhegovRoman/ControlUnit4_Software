#ifndef TEMPERATURERESET_ADDON_H
#define TEMPERATURERESET_ADDON_H

#include <QWidget>
#include <QMouseEvent>
#include <QDialog>
#include <QLabel>

#include "advancedrelaycontrol.h"
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

   void mousePressEvent(QMouseEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);

public:
   explicit TemperatureResetAddon(TemperatureRecycleInterface *tempRecycle, QWidget *parent = nullptr);
   ~TemperatureResetAddon();  

   void setDriver(TempDriverM1 *driver);
   void setTempRecycle(TemperatureRecycleInterface *value);
   void toggleIndicator(QLabel* label, bool isEnabled);
   void toggleInterface(bool isEnabled);

public slots:
   void updateProgressBar();
   void progressBarClicked();
   void changeAlgoritmState(bool isRuning);

private:
   Ui::TemperatureResetAddon *ui;
   };

#endif // TEMPERATURERESET_ADDON_H
