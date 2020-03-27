#include "sspddriveroption.h"
#include <QThread>
#include <QDebug>
#include "inputwidget.h"


void SspdDriverOption::getOptions(FT801_SPI *ft801, SspdDriverM0 *driver)
{
    if (ft801 == nullptr || driver == nullptr)
        return;

    ft801->DLStart();

    ft801->TagMask(1);
    ft801->Tag(BT_Back);
    ft801->Cmd_Button(16, 8, 48, 48, 27, 0, "");
    ft801->Tag(BT_ComparatorLevel);
    ft801->Cmd_Button(16, 85, 442, 28, 29, 0, "");
    ft801->Tag(BT_CounterTimeOut);
    ft801->Cmd_Button(16, 122, 442, 28, 29, 0, "");
    ft801->Tag(BT_AutoresetThreshold);
    ft801->Cmd_Button(16, 194, 442, 28, 29, 0, "");
    ft801->Tag(BT_AutoresetTimeOut);
    ft801->Cmd_Button(16, 230, 442, 28, 29, 0, "");
    ft801->TagMask(0);


    ft801->Cmd_Gradient(464, 73, 0x3E3E3E, 464, 283, 0x000000);
    ft801->LineWidth(16);

    ft801->ColorRGB(130, 130, 130);
    ft801->Begin(FT_LINES);
    ft801->Vertex2ii(0, 68, 0, 0);
    ft801->Vertex2ii(480, 68, 0, 0);
    ft801->End();
    ft801->ColorRGB(0,0,0);
    ft801->Begin(FT_LINES);
    ft801->Vertex2ii(0, 66, 0, 0);
    ft801->Vertex2ii(480, 66, 0, 0);
    ft801->End();

    ft801->LineWidth(32);
    ft801->ColorRGB(255, 255, 255);
    ft801->Begin(FT_LINES);
    ft801->Vertex2ii(30, 32, 0, 0);
    ft801->Vertex2ii(58, 32, 0, 0);
    ft801->Vertex2ii(26, 32, 0, 0);
    ft801->Vertex2ii(40, 18, 0, 0);
    ft801->Vertex2ii(26, 32, 0, 0);
    ft801->Vertex2ii(40, 46, 0, 0);
    ft801->End();

    ft801->ColorRGB(255,255,255);
    ft801->Cmd_Text(243,22,31, 1536, "SSPD Options");
    ft801->Cmd_Text(243,55,26, 1536, QString("Address: %1").arg(driver->devAddress()).toLocal8Bit());

    ft801->LineWidth(40);
    ft801->ColorRGB(102, 85, 102);
    ft801->Begin(FT_RECTS);
    ft801->Vertex2ii(12, 84, 0, 0);
    ft801->Vertex2ii(470, 260, 0, 0);
    ft801->End();

    ft801->ColorRGB(0, 0, 0);
    ft801->Begin(FT_RECTS);
    ft801->Vertex2ii(10, 82, 0, 0);
    ft801->Vertex2ii(468, 258, 0, 0);
    ft801->End();

    ft801->LineWidth(16);
    ft801->ColorRGB(120, 120, 120);
    ft801->Begin(FT_LINES);
    ft801->Vertex2ii(18, 118, 0, 0);
    ft801->Vertex2ii(460, 118, 0, 0);
    ft801->Vertex2ii(18, 154, 0, 0);
    ft801->Vertex2ii(460, 154, 0, 0);
    ft801->Vertex2ii(18, 190, 0, 0);
    ft801->Vertex2ii(460, 190, 0, 0);
    ft801->Vertex2ii(18, 226, 0, 0);
    ft801->Vertex2ii(460, 226, 0, 0);
    ft801->End();

    ft801->ColorRGB(255, 255, 255);
    ft801->Cmd_Text(18, 100, 29, FT_OPT_CENTERY, "Counter level:");
    ft801->Cmd_Text(18, 136, 29, FT_OPT_CENTERY, "Counter timeout:");
    ft801->Cmd_Text(18, 172, 29, FT_OPT_CENTERY, "Autoreset:");
    ft801->Cmd_Text(18, 208, 29, FT_OPT_CENTERY, "Autoreset threshold:");
    ft801->Cmd_Text(18, 244, 29, FT_OPT_CENTERY, "Autoreset timeout:");

    uint16_t dlOffset = ft801->Read16(REG_CMD_DL);
    ft801->Cmd_Memcpy(120000L, FT_RAM_DL, dlOffset);

    ft801->ColorRGB(255, 119, 0);
    ft801->Cmd_Spinner(240, 172, 0, 1);

    ft801->DLEnd();
    ft801->Finish();

    bool ok = false;

    driver->params()->getValueSync(&ok, 5);
    if (ok)
        driver->status()->getValueSync(&ok, 5);

    bool update = ok;
    uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag;

    while (true){

        //обработка кнопок
        buttonTag = ft801->Read(REG_TOUCH_TAG);
        if (buttonTag){
            if (buttonTag != lastButtonPressedTag){
                lastButtonPressedTag = buttonTag;
                update = true;
            }
        }
        else {
            switch (lastButtonPressedTag) {
            case BT_Back: {
                qDebug()<<"back";
                return;
            }
            case BT_ComparatorLevel: {
                double temp = InputWidget::getDouble(ft801, "Counter level, mV");
                if (!isnan(temp)){
                    driver->cmpReferenceLevel()->setValueSync(static_cast<float>(temp * 1e-3), nullptr, 5);
                    driver->params()->getValueSync(nullptr, 5);
                }
                lastButtonPressedTag = 0;
                update = true;
                break;
            }
            case BT_CounterTimeOut: {
                double temp = InputWidget::getDouble(ft801, "Timeout, sec");
                if (!isnan(temp)){
                    driver->timeConst()->setValueSync(static_cast<float>(temp), nullptr, 5);
                    driver->params()->getValueSync(nullptr, 5);
                }
                lastButtonPressedTag = 0;
                update = true;
                break;
            }
            case BT_AutoresetThreshold: {
                double temp = InputWidget::getDouble(ft801, "AR trigger level, mV");
                if (!isnan(temp)){
                    driver->autoResetThreshold()->setValueSync(static_cast<float>(temp * 1e-3), nullptr, 5);
                    driver->params()->getValueSync(nullptr, 5);
                }
                lastButtonPressedTag = 0;
                update = true;
                break;
            }
            case BT_AutoresetTimeOut: {
                double temp = InputWidget::getDouble(ft801, "AR timeout, sec");
                if (!isnan(temp)){
                    driver->autoResetTimeOut()->setValueSync(static_cast<float>(temp), nullptr, 5);
                    driver->params()->getValueSync(nullptr, 5);
                }
                lastButtonPressedTag = 0;
                update = true;
                break;
            }
            case BT_AutoresetEnable: {
                driver->autoResetEnable()->setValueSync(!driver->status()->currentValue().stAutoResetOn, nullptr, 5);
                driver->status()->getValueSync(nullptr, 5);
                lastButtonPressedTag = 0;
                update = true;
                break;
            }

            }
        }
        //отрисовка
        if (update){
            update = false;
            ft801->DLStart();
            ft801->Cmd_Append(120000L, dlOffset);

            ft801->ColorRGB(255, 255, 255);
            ft801->Cmd_Text(450, 100, 29, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                            QString("%1 mV").arg(static_cast<double>(driver->params()->currentValue().Cmp_Ref_Level) * 1000.0).toLocal8Bit());
            ft801->Cmd_Text(450, 136, 29, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                            QString("%1 sec").arg(static_cast<double>(driver->params()->currentValue().Time_Const)).toLocal8Bit());
            ft801->Cmd_Text(450, 208, 29, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                            QString("%1 mV").arg(static_cast<double>(driver->params()->currentValue().AutoResetThreshold) * 1000.0).toLocal8Bit());
            ft801->Cmd_Text(450, 244, 29, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                            QString("%1 sec").arg(static_cast<double>(driver->params()->currentValue().AutoResetTimeOut)).toLocal8Bit());

            ft801->Cmd_FGColor(0xC8510B);
            ft801->Cmd_BGColor(0x783508);

            ft801->TagMask(1);
            bool tmpBool = driver->status()->currentValue().stAutoResetOn;
            ft801->Cmd_BGColor(tmpBool ? 0x783508 : 0x525252);
            ft801->Tag(BT_AutoresetEnable);
            ft801->Cmd_Toggle(396, 166, 48, 27, 0, tmpBool ? 65535 : 0, "off""\xFF""on");
            ft801->TagMask(0);

            ft801->DLEnd();
            ft801->Finish();
        }

        QThread::currentThread()->msleep(50);
    }

}
