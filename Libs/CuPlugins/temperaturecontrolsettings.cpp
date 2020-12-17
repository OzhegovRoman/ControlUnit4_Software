#include "temperaturecontrolsettings.h"

//#define TO_STR(x) #x
#define LOAD_SETTING(x) x = settings->value(#x)
#define SAVE_SETTING(x) settings->setValue(#x,(x));

TemperatureControlSettings::TemperatureControlSettings()
   {
   QString settingsFile = QApplication::applicationDirPath()+"/TempControlSettings.ini";
   qDebug() << settingsFile;

   QFileInfo fi(settingsFile);
   settings = new QSettings(settingsFile, QSettings::IniFormat);

   if (fi.exists())
      loadSettings();
   else
      createNewSettings();
   }

void TemperatureControlSettings::createNewSettings()
   {
   settings->clear();

   majorVersion = currentMajorVer;
   minorVersion = currentMinorVer;

   tempSensorIndex = 0;
   autoRecycleTreshold = 1.5;
   workTemperatureT1 = 10;

   heatingMins = 12;
   thermalizationMins = 10;
   coolingDownMins = 25;

   saveSettings();
   }

bool TemperatureControlSettings::checkVersion()
   {
   bool rv = true;
   settings->beginGroup("General");
   LOAD_SETTING(majorVersion).toInt();
   LOAD_SETTING(minorVersion).toInt();
   settings->endGroup();

   if (majorVersion != currentMajorVer || minorVersion != currentMinorVer)
      rv = false;
   return rv;
   }

void TemperatureControlSettings::loadSettings()
   {
   settings->beginGroup("General");
   LOAD_SETTING(majorVersion).toInt();
   LOAD_SETTING(minorVersion).toInt();
   settings->endGroup();

   settings->beginGroup("TemperatureTrigger");
   LOAD_SETTING(tempSensorIndex).toInt();
   LOAD_SETTING(autoRecycleTreshold).toReal();
   LOAD_SETTING(workTemperatureT1).toReal();
   settings->endGroup();

   settings->beginGroup("RecyclingParameters");
   LOAD_SETTING(heatingMins).toInt();
   LOAD_SETTING(thermalizationMins).toInt();
   LOAD_SETTING(coolingDownMins).toInt();
   settings->endGroup();
   }

void TemperatureControlSettings::saveSettings()
   {
   settings->clear();

   settings->beginGroup("General");
   SAVE_SETTING(majorVersion);
   SAVE_SETTING(minorVersion);
   settings->endGroup();

   settings->beginGroup("TemperatureTrigger");
   SAVE_SETTING(tempSensorIndex);
   SAVE_SETTING(autoRecycleTreshold);
   SAVE_SETTING(workTemperatureT1);
   settings->endGroup();

   settings->beginGroup("RecyclingParameters");
   SAVE_SETTING(heatingMins);
   SAVE_SETTING(thermalizationMins);
   SAVE_SETTING(coolingDownMins);
   settings->endGroup();

   settings->sync();
   }
