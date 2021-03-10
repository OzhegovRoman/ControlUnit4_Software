#include "hwresources.h"
#include <cstdint>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QProcess>


HWResources::HWResources()
   {

   }


QString HWResources::systemLoad()  // ignoring data
   {
#ifdef __linux__
   qreal temperature = 0;
   qreal cpuFreq = 0;
   qreal memoryLoad = 0;
   uint32_t memSpace[2] = {0,0};

   QFile freqFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
   if (freqFile.open(QIODevice::ReadOnly)){
      QTextStream in(&freqFile);
      cpuFreq = in.readLine().toInt();
      freqFile.close();
      }

   QFile tempFile("/sys/class/thermal/thermal_zone0/temp");
   if (tempFile.open(QIODevice::ReadOnly)){
      QTextStream in(&tempFile);
      temperature = in.readLine().toInt();
      tempFile.close();
      }

   QFile memFile("/proc/meminfo");
   if (memFile.open(QIODevice::ReadOnly)){
      QTextStream in(&memFile);
      memSpace[0] = in.readLine().split(":").last().chopped(3).toInt();
      memSpace[1] = in.readLine().split(":").last().chopped(3).toInt();
      memFile.close();
      }

   if (memSpace[0] != 0 && memSpace[1] != 0)
      memoryLoad = (double)(memSpace[0] - memSpace[1]) / memSpace[0];

   QProcess throttled;
   throttled.start("vcgencmd get_throttled");
   throttled.waitForFinished(5000);
   QString throttledStr = throttled.readAllStandardOutput();
   throttled.close();


   return QString("Mem used: %1/%2 Mb (%3\%)\tCpu: %4\tT: %5\t%6")
         .arg((memSpace[0]-memSpace[1])/1000)
         .arg(memSpace[0]/1000)
         .arg(memoryLoad*100)
         .arg(cpuFreq)
         .arg(temperature/1000)
         .arg(throttledStr);
#else
   return QString();
#endif
   }
