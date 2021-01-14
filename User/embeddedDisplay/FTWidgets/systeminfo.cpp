#include "systeminfo.h"
#include <QDebug>
#include <QtNetwork>
#include <QNetworkInterface>
#include <QFile>

#include "../compactdraw.h"

SystemInfo::SystemInfo(Gpu_Hal_Context_t *host)
   : FTWidget (nullptr)
   {
   setHost(host);
   }

void SystemInfo::setup()
   {
   if (host() == nullptr){
      terminate();
      return;
      }

   Gpu_CoCmd_Dlstart(host());
   App_WrCoCmd_Buffer(host(), TAG_MASK(1));
   App_WrCoCmd_Buffer(host(), TAG(BT_Dummy));
   Gpu_CoCmd_Button(host(), 0, 0, 480, 270, 27, 0, "");
   App_WrCoCmd_Buffer(host(), TAG(BT_Back));
   Gpu_CoCmd_Button(host(), 17, 8, 48, 48, 27, 0, "");
   App_WrCoCmd_Buffer(host(), TAG(BT_Theme));
   Gpu_CoCmd_Button(host(), 412, 8, 48, 48, 27, 0, "");
   App_WrCoCmd_Buffer(host(), TAG_MASK(0));

   CD::headPanel("System Info");
   CD::buttonBack();

   CD::buttonChangeTheme();

   CD::mainBackground();
   CD::mainArea();

   App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));

   Gpu_CoCmd_Text(host(), 12, 112, 29, 0, QString("Software version: " + QString(VERSION)).toLocal8Bit());
   Gpu_CoCmd_Text(host(), 12, 82,  29, 0, QString("TcpIp address: " + myTcpIpAddress()).toLocal8Bit());
   Gpu_CoCmd_Text(host(), 12, 142, 29, 0, QString("Serial number: " + mySerialNumber()).toLocal8Bit());

   App_WrCoCmd_Buffer(host(), DISPLAY());
   Gpu_CoCmd_Swap(host());
   App_Flush_Co_Buffer(host());
   Gpu_Hal_WaitCmdfifo_empty(host());
   }

void SystemInfo::loop()
   {
   static uint32_t lastButtonPressedTag = 0;
   uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);

   if (lastButtonPressedTag != 0 && buttonTag != lastButtonPressedTag)
      qDebug() << "buttonPressed" << lastButtonPressedTag;

   if (buttonTag && (buttonTag  != 255))
      lastButtonPressedTag = buttonTag;
   else {
      switch(lastButtonPressedTag){
         case BT_Back: {
            lastButtonPressedTag = 0;
            terminate();
            emit backClicked();
            break;
            }
         case BT_Theme:{
            lastButtonPressedTag = 0;
            CD::cycleTheme();
            CD::saveTheme();
            setup();
//            terminate();
//            emit backClicked();
            break;
            }
         default: {lastButtonPressedTag = 0;} break;
         }
      }
   FTWidget::loop();
   }

QString SystemInfo::myTcpIpAddress()
   {
   for (QHostAddress address: QNetworkInterface::allAddresses())
      if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost)
         return address.toString();
   return QString();
   }

QString SystemInfo::mySerialNumber()
   {
   QFile file("/proc/cpuinfo");
   if (!file.open(QIODevice::ReadOnly))
      return QString();
   QString str;
   do {
      QStringList strL = QString(file.readLine()).split(": ");
      if (strL[0].contains("Serial")){
         str = strL[1].simplified();
         }
      } while (!file.atEnd());

   file.close();
   return str;
   }
