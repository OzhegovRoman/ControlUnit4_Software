#include "hwresources.h"
#include <cstdint>
#include <QFile>
#include <QTextStream>
#include <QStringList>


HWResources::HWResources()
   {

   }


QString HWResources::systemLoad()  // ignoring data
   {
#ifdef __linux__
   uint8_t cpuCount = 0;
   static uint32_t prevCpuStats[4] = {0,0,0,0};
   static uint32_t cpuDiff[4] = {0,0,0,0};
   static bool firstRead = true;

   qreal cpuLoad = 0;
   qreal memoryLoad = 0;
   uint32_t memSpace[2] = {0,0};

   QFile cpuInfo("/proc/cpuinfo");
   if (cpuInfo.open(QIODevice::ReadOnly)){
      QTextStream in(&cpuInfo);
      while (!in.atEnd())
         {
         QString line = in.readLine();
         QStringList strlist = line.split('\n');
         if (strlist.contains("core id"))
            cpuCount++;
         }
      cpuInfo.close();
      }

   QFile procFile("/proc/stat");
   if (procFile.open(QIODevice::ReadOnly)){
      QTextStream in(&procFile);
      QStringList strList = in.readLine().split(' ');

      for(int i = 0; i < 4; ++i) {
         uint32_t val = strList.at(i+1).toInt();
         cpuDiff[i] = val - prevCpuStats[i];
         prevCpuStats[i] = val;
         cpuLoad += cpuDiff[i];
         }
      procFile.close();
      }

   QFile memFile("/proc/meminfo");
   if (memFile.open(QIODevice::ReadOnly)){
      QTextStream in(&memFile);
      int i = 0;
      while (!in.atEnd()){
         QString str = in.readLine().split(':').last().remove(' ').chopped(3);
         memSpace[i] = str.toInt();
         if (i==1) break;
         }
      memFile.close();
      }
   memoryLoad = memSpace[0]/memSpace[1];

   if (!firstRead)
      cpuLoad = 100. * (cpuLoad-cpuDiff[3])/cpuLoad;

   firstRead = false;
   return QString("Mem: %1/%2 (%3)\t\tCpu: %4").arg(memSpace[0]).arg(memSpace[1]).arg(memoryLoad).arg(cpuLoad);
#else
   return QString();
#endif
   }
