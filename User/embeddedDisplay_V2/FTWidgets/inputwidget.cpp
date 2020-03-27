#include "inputwidget.h"
#include <QThread>
#include <QDebug>

double InputWidget::getDouble(FT801_SPI *ft801, QString title)
{
    if (ft801 == nullptr)
        return 0;

    ft801->DLStart();

    ft801->TagMask(1);
    ft801->Tag(BT_Back);
    ft801->Cmd_Button(17, 8, 48, 48, 27, 0, "");
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

    ft801->LineWidth(40);

    ft801->ColorRGB(102, 85, 102);
    ft801->Begin(FT_RECTS);
    ft801->Vertex2ii(12, 84, 0, 0);
    ft801->Vertex2ii(468, 124, 0, 0);
    ft801->End();

    ft801->ColorRGB(0, 0, 0);
    ft801->Begin(FT_RECTS);
    ft801->Vertex2ii(10, 82, 0, 0);
    ft801->Vertex2ii(466, 122, 0, 0);
    ft801->End();

    ft801->Cmd_BGColor(0x783508);
    ft801->ColorRGB(255, 255, 255);
    ft801->Cmd_Text(268, 32, 31, FT_OPT_CENTER, title.toLocal8Bit());

    uint16_t dlOffset = ft801->Read16(REG_CMD_DL);
    ft801->Cmd_Memcpy(110000L, FT_RAM_DL, dlOffset);

    ft801->DLEnd();
    ft801->Finish();

    QString str;
    bool update = true;
    uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag;
    while (true){
        // отрисовываем
        buttonTag = ft801->Read(REG_TOUCH_TAG);
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
            ft801->DLStart();
            ft801->Cmd_Append(110000L, dlOffset);

            ft801->Cmd_Text(240, 102, 30, FT_OPT_CENTER, str.toLocal8Bit());


            ft801->Cmd_FGColor(0xC8510B);
            ft801->Cmd_Keys(10, 132, 460, 40, 29, static_cast<char>(lastButtonPressedTag), "12345");
            ft801->Cmd_Keys(10, 178, 459, 40, 29, static_cast<char>(lastButtonPressedTag), "67890");
            ft801->Cmd_Keys(10, 224, 273, 40, 29, static_cast<char>(lastButtonPressedTag), "<.-");
            ft801->Cmd_FGColor(0x1DCC37);
            ft801->TagMask(1);
            ft801->Tag(BT_Ok);
            ft801->Cmd_Button(288, 224, 182, 40, 29, buttonTag == BT_Ok ? FT_OPT_FLAT: 0, "Ok");
            ft801->TagMask(0);

            ft801->DLEnd();
            ft801->Finish();
        }

        QThread::currentThread()->msleep(50);
    }
}
