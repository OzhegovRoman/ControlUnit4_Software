#include "sspddriveroption.h"
#include "inputwidget.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include <QDebug>
#include <QThread>

#include "../compactdraw.h"

void SspdDriverOption::getOptions(Gpu_Hal_Context_t *host,  CommonDriver *driver)
   {
   if (host == nullptr || driver == nullptr)
      return;

   Gpu_CoCmd_Dlstart(host);
   App_WrCoCmd_Buffer(host, TAG_MASK(1));
   App_WrCoCmd_Buffer(host, TAG(BT_Dummy));
   Gpu_CoCmd_Button(host, 0, 0, 480, 270, 27, 0, "");
   App_WrCoCmd_Buffer(host, TAG(BT_Back));
   Gpu_CoCmd_Button(host, 16, 8, 48, 48, 27, 0, "");
   App_WrCoCmd_Buffer(host, TAG(BT_ComparatorLevel));
   Gpu_CoCmd_Button(host, 16, 85, 442, 28, 29, 0, "");
   App_WrCoCmd_Buffer(host, TAG(BT_CounterTimeOut));
   Gpu_CoCmd_Button(host, 16, 122, 442, 28, 29, 0, "");
   App_WrCoCmd_Buffer(host, TAG(BT_AutoresetThreshold));
   Gpu_CoCmd_Button(host, 16, 194, 442, 28, 29, 0, "");
   App_WrCoCmd_Buffer(host, TAG(BT_AutoresetTimeOut));
   Gpu_CoCmd_Button(host, 16, 230, 442, 28, 29, 0, "");
   App_WrCoCmd_Buffer(host, TAG_MASK(0));

   CD::headPanel("SSPD Options",QString("Address: %1").arg(driver->devAddress()));
   CD::mainBackground();
   CD::mainArea();
   CD::buttonBack();

   CD::simpleListHeader({"Counter level:",
                         "Counter timeout:",
                         "Autoreset:",
                         "Autoreset threshold:",
                         "Autoreset timeout:"});

   App_Flush_Co_Buffer(host);
   Gpu_Hal_WaitCmdfifo_empty(host);

   uint16_t dlOffset = Gpu_Hal_Rd16(host, REG_CMD_DL);
   Gpu_CoCmd_Memcpy(host, 120000L, RAM_DL, dlOffset);

   App_WrCoCmd_Buffer(host, COLOR_RGB(255, 119, 0));
   Gpu_CoCmd_Spinner(host, 240, 172, 0, 0);

   App_WrCoCmd_Buffer(host, DISPLAY());
   Gpu_CoCmd_Swap(host);
   App_Flush_Co_Buffer(host);
   Gpu_Hal_WaitCmdfifo_empty(host);

   auto *tmpDriverM0 = qobject_cast<SspdDriverM0*>(driver);
   auto *tmpDriverM1 = qobject_cast<SspdDriverM1*>(driver);
   bool ok = false;

   if (tmpDriverM0){
      tmpDriverM0->params()->getValueSync(&ok, 5);
      if (ok)
         tmpDriverM0->status()->getValueSync(&ok, 5);
      }
   if (tmpDriverM1){
      tmpDriverM1->params()->getValueSync(&ok, 5);
      if (ok)
         tmpDriverM1->status()->getValueSync(&ok, 5);
      }

   bool update = ok;
   uint32_t lastButtonPressedTag = 0;
   uint8_t buttonTag;

   while (true){

      //обработка кнопок
      buttonTag = Gpu_Hal_Rd8(host, REG_TOUCH_TAG);
      if (buttonTag && buttonTag!=255){
         if (buttonTag != lastButtonPressedTag){
            lastButtonPressedTag = buttonTag;
            update = true;
            }
         }
      else {
         switch (lastButtonPressedTag) {
            case BT_Back: {
               return;
               }
            case BT_ComparatorLevel: {
               double temp = InputWidget::getDouble(host, "Counter level, mV");
               if (!isnan(temp)){
                  if (tmpDriverM0){
                     tmpDriverM0->cmpReferenceLevel()->setValueSync(static_cast<float>(temp * 1e-3), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM0->cmpReferenceLevel()->getValueSync(nullptr, 5);
                     }
                  if (tmpDriverM1){
                     tmpDriverM1->cmpReferenceLevel()->setValueSync(static_cast<float>(temp * 1e-3), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM1->cmpReferenceLevel()->getValueSync(nullptr, 5);
                     }
                  }
               lastButtonPressedTag = 0;
               update = true;
               break;
               }
            case BT_CounterTimeOut: {
               double temp = InputWidget::getDouble(host, "Timeout, sec");
               if (!isnan(temp)){
                  if (tmpDriverM0){
                     tmpDriverM0->timeConst()->setValueSync(static_cast<float>(temp), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM0->timeConst()->getValueSync(nullptr, 5);
                     }
                  if (tmpDriverM1){
                     tmpDriverM1->timeConst()->setValueSync(static_cast<float>(temp), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM1->timeConst()->getValueSync(nullptr, 5);
                     }
                  }
               lastButtonPressedTag = 0;
               update = true;
               break;
               }
            case BT_AutoresetThreshold: {
               double temp = InputWidget::getDouble(host, "AR trigger level, mV");
               if (!isnan(temp)){
                  if (tmpDriverM0){
                     tmpDriverM0->autoResetThreshold()->setValueSync(static_cast<float>(temp * 1e-3), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM0->autoResetThreshold()->getValueSync(nullptr, 5);
                     }
                  if (tmpDriverM1){
                     tmpDriverM1->autoResetThreshold()->setValueSync(static_cast<float>(temp * 1e-3), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM1->autoResetThreshold()->getValueSync(nullptr, 5);
                     }
                  }
               lastButtonPressedTag = 0;
               update = true;
               break;
               }
            case BT_AutoresetTimeOut: {
               double temp = InputWidget::getDouble(host, "AR timeout, sec");
               if (!isnan(temp)){
                  if (tmpDriverM0){
                     tmpDriverM0->autoResetTimeOut()->setValueSync(static_cast<float>(temp), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM0->autoResetTimeOut()->getValueSync(nullptr, 5);
                     }
                  if (tmpDriverM1){
                     tmpDriverM1->autoResetTimeOut()->setValueSync(static_cast<float>(temp), nullptr, 10);
                     QThread::msleep(100);
                     tmpDriverM1->autoResetTimeOut()->getValueSync(nullptr, 5);
                     }
                  }
               lastButtonPressedTag = 0;
               update = true;
               break;
               }
            case BT_AutoresetEnable: {
               if (tmpDriverM0){
                  tmpDriverM0->autoResetEnable()->setValueSync(!tmpDriverM0->status()->currentValue().stAutoResetOn, nullptr, 10);
                  QThread::msleep(100);
                  tmpDriverM0->status()->getValueSync(nullptr, 5);
                  }
               if (tmpDriverM1){
                  tmpDriverM1->autoResetEnable()->setValueSync(!tmpDriverM1->status()->currentValue().stAutoResetOn, nullptr, 10);
                  QThread::msleep(100);
                  tmpDriverM1->status()->getValueSync(nullptr, 5);
                  }
               lastButtonPressedTag = 0;
               update = true;
               break;
               }

            }
         }
      //отрисовка
      if (update){
         qDebug()<<"update";
         update = false;
         Gpu_CoCmd_Dlstart(host);
         Gpu_CoCmd_Append(host, 120000L, dlOffset);

         App_WrCoCmd_Buffer(host, COLOR(CD::themeColor(TextNormal)));
         CU4SDM0V1_Param_t params;
         if (tmpDriverM0) params = tmpDriverM0->params()->currentValue();
         if (tmpDriverM1) params = tmpDriverM1->params()->currentValue();

         Gpu_CoCmd_Text(host, 450, 98, 29, OPT_CENTERY | OPT_RIGHTX,
                        QString("%1 mV").arg(static_cast<double>(params.Cmp_Ref_Level) * 1000.0).toLocal8Bit());
         Gpu_CoCmd_Text(host, 450, 134, 29, OPT_CENTERY | OPT_RIGHTX,
                        QString("%1 sec").arg(static_cast<double>(params.Time_Const)).toLocal8Bit());
         Gpu_CoCmd_Text(host, 450, 206, 29, OPT_CENTERY | OPT_RIGHTX,
                        QString("%1 mV").arg(static_cast<double>(params.AutoResetThreshold) * 1000.0).toLocal8Bit());
         Gpu_CoCmd_Text(host, 450, 242, 29, OPT_CENTERY | OPT_RIGHTX,
                        QString("%1 sec").arg(static_cast<double>(params.AutoResetTimeOut)).toLocal8Bit());

         App_WrCoCmd_Buffer(host, TAG_MASK(1));
         bool tmpBool = false;
         if (tmpDriverM0) tmpBool = tmpDriverM0->status()->currentValue().stAutoResetOn;
         if (tmpDriverM1) tmpBool = tmpDriverM1->status()->currentValue().stAutoResetOn;

         CD::sliderButton(396,164,"off""\xFF""on",BT_AutoresetEnable,tmpBool,48);

         App_WrCoCmd_Buffer(host, TAG_MASK(0));

         App_WrCoCmd_Buffer(host, DISPLAY());
         Gpu_CoCmd_Swap(host);
         App_Flush_Co_Buffer(host);
         Gpu_Hal_WaitCmdfifo_empty(host);
         }

      Gpu_Hal_Sleep(50);
      }

   }
