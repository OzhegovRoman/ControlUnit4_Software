#include "sspddriveroption.h"
#include "inputwidget.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include <QDebug>
#include <QThread>

void SspdDriverOption::getOptions(Gpu_Hal_Context_t *host,  CommonDriver *driver)
{
    if (host == nullptr || driver == nullptr)
        return;

    Gpu_CoCmd_Dlstart(host);
    App_WrCoCmd_Buffer(host, TAG_MASK(1));
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

    Gpu_CoCmd_Gradient(host, 464, 73, 0x3E3E3E, 464, 283, 0x000000);
    App_WrCoCmd_Buffer(host, LINE_WIDTH(16));

    App_WrCoCmd_Buffer(host, COLOR_RGB(130, 130, 130));
    App_WrCoCmd_Buffer(host, BEGIN(LINES));
    App_WrCoCmd_Buffer(host, VERTEX2II(0, 68, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(480, 68, 0, 0));
    App_WrCoCmd_Buffer(host, END());
    App_WrCoCmd_Buffer(host, COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host, BEGIN(LINES));
    App_WrCoCmd_Buffer(host, VERTEX2II(0, 66, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(480, 66, 0, 0));
    App_WrCoCmd_Buffer(host, END());

    App_WrCoCmd_Buffer(host, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(host, LINE_WIDTH(32));
    App_WrCoCmd_Buffer(host, BEGIN(LINES));
    App_WrCoCmd_Buffer(host, VERTEX2II(30, 32, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(58, 32, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(26, 32, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(40, 18, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(26, 32, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(40, 46, 0, 0));
    App_WrCoCmd_Buffer(host, END());

    App_WrCoCmd_Buffer(host, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(host, 243, 22, 31, 1536, "SSPD Options");
    Gpu_CoCmd_Text(host, 243, 55, 26, 1536, QString("Address: %1").arg(driver->devAddress()).toLocal8Bit());

    App_WrCoCmd_Buffer(host, LINE_WIDTH(40));

    App_WrCoCmd_Buffer(host, COLOR_RGB(102, 85, 102));
    App_WrCoCmd_Buffer(host, BEGIN(RECTS));
    App_WrCoCmd_Buffer(host, VERTEX2II(12, 84, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(470, 260, 0, 0));
    App_WrCoCmd_Buffer(host, END());

    App_WrCoCmd_Buffer(host, COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host, BEGIN(RECTS));
    App_WrCoCmd_Buffer(host, VERTEX2II(10, 82, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(468, 258, 0, 0));
    App_WrCoCmd_Buffer(host, END());

    App_WrCoCmd_Buffer(host, LINE_WIDTH(16));
    App_WrCoCmd_Buffer(host, COLOR_RGB(120, 120, 120));
    App_WrCoCmd_Buffer(host, BEGIN(LINES));
    App_WrCoCmd_Buffer(host, VERTEX2II(18,  118, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(460, 118, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(18,  154, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(460, 154, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(18,  190, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(460, 190, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(18,  226, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(460, 226, 0, 0));
    App_WrCoCmd_Buffer(host, END());

    App_WrCoCmd_Buffer(host, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(host, 18, 100, 29, OPT_CENTERY, "Counter level:");
    Gpu_CoCmd_Text(host, 18, 136, 29, OPT_CENTERY, "Counter timeout:");
    Gpu_CoCmd_Text(host, 18, 172, 29, OPT_CENTERY, "Autoreset:");
    Gpu_CoCmd_Text(host, 18, 208, 29, OPT_CENTERY, "Autoreset threshold:");
    Gpu_CoCmd_Text(host, 18, 244, 29, OPT_CENTERY, "Autoreset timeout:");

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

            App_WrCoCmd_Buffer(host, COLOR_RGB(255, 255, 255));
            CU4SDM0V1_Param_t params;
            if (tmpDriverM0) params = tmpDriverM0->params()->currentValue();
            if (tmpDriverM1) params = tmpDriverM1->params()->currentValue();

            Gpu_CoCmd_Text(host, 450, 100, 29, OPT_CENTERY | OPT_RIGHTX,
                           QString("%1 mV").arg(static_cast<double>(params.Cmp_Ref_Level) * 1000.0).toLocal8Bit());
            Gpu_CoCmd_Text(host, 450, 136, 29, OPT_CENTERY | OPT_RIGHTX,
                           QString("%1 sec").arg(static_cast<double>(params.Time_Const)).toLocal8Bit());
            Gpu_CoCmd_Text(host, 450, 208, 29, OPT_CENTERY | OPT_RIGHTX,
                           QString("%1 mV").arg(static_cast<double>(params.AutoResetThreshold) * 1000.0).toLocal8Bit());
            Gpu_CoCmd_Text(host, 450, 244, 29, OPT_CENTERY | OPT_RIGHTX,
                           QString("%1 sec").arg(static_cast<double>(params.AutoResetTimeOut)).toLocal8Bit());

            Gpu_CoCmd_FgColor(host, 0xC8510B);
            Gpu_CoCmd_BgColor(host, 0x783508);

            App_WrCoCmd_Buffer(host, TAG_MASK(1));
            bool tmpBool = false;
            if (tmpDriverM0) tmpBool = tmpDriverM0->status()->currentValue().stAutoResetOn;
            if (tmpDriverM1) tmpBool = tmpDriverM1->status()->currentValue().stAutoResetOn;
            Gpu_CoCmd_BgColor(host, tmpBool ? 0x783508 : 0x525252);
            App_WrCoCmd_Buffer(host, TAG(BT_AutoresetEnable));
            Gpu_CoCmd_Toggle(host, 396, 166, 48, 27, 0, tmpBool ? 65535 : 0, "off""\xFF""on");
            App_WrCoCmd_Buffer(host, TAG_MASK(0));

            App_WrCoCmd_Buffer(host, DISPLAY());
            Gpu_CoCmd_Swap(host);
            App_Flush_Co_Buffer(host);
            Gpu_Hal_WaitCmdfifo_empty(host);
        }

        Gpu_Hal_Sleep(50);
    }

}
