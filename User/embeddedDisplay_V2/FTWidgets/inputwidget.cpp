#include "inputwidget.h"
#include <QDebug>

double InputWidget::getDouble(Gpu_Hal_Context_t *host, QString title)
{
    if (host == nullptr)
        return 0;

    Gpu_CoCmd_Dlstart(host);
    App_WrCoCmd_Buffer(host, TAG_MASK(1));
    App_WrCoCmd_Buffer(host, TAG(BT_Back));
    Gpu_CoCmd_Button(host, 17, 8, 48, 48, 27, 0, "");
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

    App_WrCoCmd_Buffer(host, LINE_WIDTH(40));

    App_WrCoCmd_Buffer(host, COLOR_RGB(102, 85, 102));
    App_WrCoCmd_Buffer(host, BEGIN(RECTS));
    App_WrCoCmd_Buffer(host, VERTEX2II(12, 84, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(468, 124, 0, 0));
    App_WrCoCmd_Buffer(host, END());

    App_WrCoCmd_Buffer(host, COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host, BEGIN(RECTS));
    App_WrCoCmd_Buffer(host, VERTEX2II(10, 82, 0, 0));
    App_WrCoCmd_Buffer(host, VERTEX2II(466, 122, 0, 0));
    App_WrCoCmd_Buffer(host, END());

    Gpu_CoCmd_BgColor(host, 0x783508);
    App_WrCoCmd_Buffer(host, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(host, 268, 32, 31, OPT_CENTER, title.toLocal8Bit());

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
        if (buttonTag){
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

            Gpu_CoCmd_Text(host, 240, 102, 30, OPT_CENTER, str.toLocal8Bit());

            Gpu_CoCmd_FgColor(host, 0xC8510B);
            Gpu_CoCmd_Keys(host, 10, 132, 460, 40, 29, static_cast<char>(lastButtonPressedTag), "12345");
            Gpu_CoCmd_Keys(host, 10, 178, 459, 40, 29, static_cast<char>(lastButtonPressedTag), "67890");
            Gpu_CoCmd_Keys(host, 10, 224, 273, 40, 29, static_cast<char>(lastButtonPressedTag), "<.-");
            Gpu_CoCmd_FgColor(host, 0x1DCC37);
            App_WrCoCmd_Buffer(host, TAG_MASK(1));
            App_WrCoCmd_Buffer(host, TAG(BT_Ok));
            Gpu_CoCmd_Button(host, 288, 224, 182, 40, 29, buttonTag == BT_Ok ? OPT_FLAT: 0, "Ok");
            App_WrCoCmd_Buffer(host, TAG_MASK(0));

            App_WrCoCmd_Buffer(host, DISPLAY());
            Gpu_CoCmd_Swap(host);
            App_Flush_Co_Buffer(host);
            Gpu_Hal_WaitCmdfifo_empty(host);
        }

        Gpu_Hal_Sleep(50);
    }
}
