#include "inputwidget.h"
#include "../compactdraw.h"

#include <QDebug>

double InputWidget::getDouble(Gpu_Hal_Context_t *host, QString title)
{
    if (host == nullptr)
        return 0;

    Gpu_CoCmd_Dlstart(host);
    App_WrCoCmd_Buffer(host, TAG_MASK(1));
    App_WrCoCmd_Buffer(host, TAG(BT_Dummy));
    Gpu_CoCmd_Button(host, 0, 0, 480, 270, 27, 0, "");
    App_WrCoCmd_Buffer(host, TAG(BT_Back));
    Gpu_CoCmd_Button(host, 17, 8, 48, 48, 27, 0, "");
    App_WrCoCmd_Buffer(host, TAG_MASK(0));

    CD::headPanel(title);
    CD::buttonBack();
    CD::mainBackground();
    CD::mainArea(0,122);

    App_Flush_Co_Buffer(host);
    Gpu_Hal_WaitCmdfifo_empty(host);

    uint16_t  dlOffset = Gpu_Hal_Rd16(host, REG_CMD_DL);
    Gpu_CoCmd_Memcpy(host, 110000L, RAM_DL, dlOffset);

    App_WrCoCmd_Buffer(host, DISPLAY());
    Gpu_CoCmd_Swap(host);
    App_Flush_Co_Buffer(host);
    Gpu_Hal_WaitCmdfifo_empty(host);

    QString str;
    bool update = true;
    uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag;
    while (true){
        // отрисовываем
        buttonTag = Gpu_Hal_Rd8(host, REG_TOUCH_TAG);
        if (buttonTag && buttonTag != 255){
           qDebug() << "buttonPressed" << buttonTag;
            if (buttonTag != lastButtonPressedTag){
                lastButtonPressedTag = buttonTag;
                update = true;
            }
        }
        else {
            if (lastButtonPressedTag == BT_Back){
                return nan("1");
            }
            if ((lastButtonPressedTag >= '0' && lastButtonPressedTag <= '9') ||
                    lastButtonPressedTag == '.'){
                update = true;
                str += static_cast<char>(lastButtonPressedTag);
                lastButtonPressedTag = 0;
            }
            if (lastButtonPressedTag == '<'){
                update = true;
                str.chop(1);
                lastButtonPressedTag = 0;
            }
            if (lastButtonPressedTag == '-'){
                update = true;
                if (!str.isEmpty()){
                    if (str[0] == '-')
                        str.remove(0,1);
                    else str = "-" + str;
                }
                lastButtonPressedTag = 0;
            }
            if (lastButtonPressedTag == BT_Ok){
                if (str.isEmpty()) return nan("1");
                return(str.toDouble());
            }

        }

        if (update){
            update = false;
            Gpu_CoCmd_Dlstart(host);
            Gpu_CoCmd_Append(host, 110000L, dlOffset);


            App_WrCoCmd_Buffer(host, COLOR(CD::themeColor(TextNormal)));
            Gpu_CoCmd_Text(host, 240, 102, 30, OPT_CENTER, str.toLocal8Bit());


            App_WrCoCmd_Buffer(host, COLOR(CD::themeColor(Colors::Grad_Center )));
            Gpu_CoCmd_FgColor(host,CD::themeColor(Colors::TextFail));
            Gpu_CoCmd_GradColor(host,CD::themeColor(Grad_Buttons));

            Gpu_CoCmd_Keys(host, 10, 132, 460, 40, 29, static_cast<char>(lastButtonPressedTag), "12345");
            Gpu_CoCmd_Keys(host, 10, 178, 459, 40, 29, static_cast<char>(lastButtonPressedTag), "67890");
            Gpu_CoCmd_Keys(host, 10, 224, 274, 40, 29, static_cast<char>(lastButtonPressedTag), "<.-");
            Gpu_CoCmd_FgColor(host, CD::themeColor(Colors::LedOk));
            App_WrCoCmd_Buffer(host, TAG_MASK(1));
            App_WrCoCmd_Buffer(host, TAG(BT_Ok));
            Gpu_CoCmd_Button(host, 287, 224, 182, 40, 29, buttonTag == BT_Ok ? OPT_FLAT: 0, "Ok");
            App_WrCoCmd_Buffer(host, TAG_MASK(0));

            App_WrCoCmd_Buffer(host, DISPLAY());
            Gpu_CoCmd_Swap(host);
            App_Flush_Co_Buffer(host);
            Gpu_Hal_WaitCmdfifo_empty(host);
        }

        Gpu_Hal_Sleep(50);
    }
}
