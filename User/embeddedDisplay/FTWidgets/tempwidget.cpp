#include "tempwidget.h"
#include <QDebug>

#include "../compactdraw.h"

TempWidget::TempWidget(Gpu_Hal_Context_t *host)
   : FTWidget (nullptr)
   , mUpdateFlag(false)
   , mDriver(nullptr)
   , dataReady(false)
   , errorFlag(false)
   , dataTimer(new QTimer(this))
   {
   setHost(host);
   dataTimer->setSingleShot(true);
   connect(dataTimer, &QTimer::timeout, this, &TempWidget::readData);
   }

void TempWidget::setup()
   {
   mUpdateFlag = false;

   if (host() == nullptr || mDriver == nullptr){
      terminate();
      return;
      }

   Gpu_CoCmd_Dlstart(host());
   App_WrCoCmd_Buffer(host(), TAG_MASK(1));
   CD::dummyButton(BT_Dummy);
   App_WrCoCmd_Buffer(host(), TAG(BT_Back));
   Gpu_CoCmd_Button(host(), 10, 10, 50, 50, 27, 0, "");
   App_WrCoCmd_Buffer(host(), TAG_MASK(0));

   CD::headPanel("Temperature Unit",QString("Address: %1").arg(mDriver->devAddress()));
   CD::buttonBack();
   CD::mainBackground();
   CD::mainArea(115,205,130,350);

   App_WrCoCmd_Buffer(host(), POINT_SIZE(100));

   App_Flush_Co_Buffer(host());
   Gpu_Hal_WaitCmdfifo_empty(host());

   dlOffset = Gpu_Hal_Rd16(host(), REG_CMD_DL);
   Gpu_CoCmd_Memcpy(host(), 100000L, RAM_DL, dlOffset);

   App_WrCoCmd_Buffer(host(), COLOR(0xffffff));
   Gpu_CoCmd_Spinner(host(), 240, 160, 0, 0);

   App_WrCoCmd_Buffer(host(), DISPLAY());
   Gpu_CoCmd_Swap(host());
   App_Flush_Co_Buffer(host());
   Gpu_Hal_WaitCmdfifo_empty(host());

   readData();
   }

void TempWidget::loop()
   {
   static uint32_t lastButtonPressedTag = 0;
   uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);
   if (buttonTag && buttonTag != 255)
      lastButtonPressedTag = buttonTag;
   else {
      if (lastButtonPressedTag){
         qDebug()<<"button pressed";

         lastButtonPressedTag = 0;
         terminate();
         emit backClicked();
         return;
         }
      }

   if (mUpdateFlag){

      mUpdateFlag = false;

      if (mDriver == nullptr)
         return;

      Gpu_CoCmd_Dlstart(host());
      Gpu_CoCmd_Append(host(), 100000L, dlOffset);

      bool isConnected = qAbs(static_cast<double>(mDriver->temperature()->currentValue())) > 1e-5;
      QString tempStr;
      ColoredStatus csText;

      if (isConnected){
         csText = CS_Normal;
         tempStr = QString("T: %1 K").
                   arg(static_cast<double>(mDriver->temperature()->currentValue()), 4, 'f', 2);
         }
      else {
         csText = CS_Inactive;
         tempStr = "N/C";
         }

      App_WrCoCmd_Buffer(host(), COLOR(CD::statusColor(csText,true)));
      Gpu_CoCmd_Text(host(), 240, 160, 31, 1536, tempStr.toLocal8Bit());

      ColoredStatus cs;
      if (dataReady){
         // зеленый
         cs = CS_Normal;
         dataReady = false;
         update();
         }
      else  if (errorFlag){
         // красный
         cs = CS_Fail;
         errorFlag = false;
         update();
         }
      else
         //серый
         cs = CS_Inactive;

      App_WrCoCmd_Buffer(host(), COLOR(CD::statusColor(cs,false)));
      App_WrCoCmd_Buffer(host(), BEGIN(POINTS));
      App_WrCoCmd_Buffer(host(), VERTEX2II(342, 124, 0, 0));
      App_WrCoCmd_Buffer(host(), END());

      App_WrCoCmd_Buffer(host(), DISPLAY());
      Gpu_CoCmd_Swap(host());
      App_Flush_Co_Buffer(host());
      Gpu_Hal_WaitCmdfifo_empty(host());
      }
   FTWidget::loop();
   }

void TempWidget::readData()
   {
   if (isStoped() || mDriver == nullptr)
      return;
   bool ok = false;
   mDriver->data()->getValueSync(&ok);

   if (ok  && !mDriver->commutator()->currentValue())
      mDriver->commutator()->setValueSync(true);
   dataReady = ok;
   errorFlag = !ok;
   update();
   dataTimer->start(1000);
   }

void TempWidget::update()
   {
   mUpdateFlag = true;
   }

void TempWidget::setDriver(TempDriverM0 *driver)
   {
   mDriver = driver;
   }
