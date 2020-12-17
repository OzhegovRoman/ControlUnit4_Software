#ifndef TEMPERATURECONTROLSETTINGS_H
#define TEMPERATURECONTROLSETTINGS_H

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QStringList>
#include <QMessageBox>
#include <QSettings>

#define Singleton(__CLASSNAME__) \
   public: \
   static __CLASSNAME__* getInstance() { \
   static __CLASSNAME__ instance; \
   return &instance; \
   } \
   private:\
   __CLASSNAME__();\
   ~__CLASSNAME__ () { } \
   __CLASSNAME__( const __CLASSNAME__ & ) = delete; \
   __CLASSNAME__ & operator=( __CLASSNAME__ & ) = delete

class TemperatureControlSettings
   {
//   Singleton(TemperatureControlSettings);

   uint currentMajorVer = 1;
   uint currentMinorVer = 1;

   QSettings* settings;

public:
   TemperatureControlSettings();
   uint majorVersion;
   uint minorVersion;

   uint tempSensorIndex;
   qreal autoRecycleTreshold;
   qreal workTemperatureT1;

   uint heatingMins;
   uint thermalizationMins;
   uint coolingDownMins;

   void createNewSettings();
   bool checkVersion();
   void loadSettings();
   void saveSettings();
   };

#endif // TEMPERATURECONTROLSETTINGS_H
