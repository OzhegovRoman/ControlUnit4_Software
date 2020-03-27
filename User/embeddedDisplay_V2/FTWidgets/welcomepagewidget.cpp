#include "welcomepagewidget.h"
#include <QThread>

const uint8_t welcome_screen[]={
    #include "../Design/Images/LOGO.binh"
};

WelcomePageWidget::WelcomePageWidget(FT801_SPI *ft801)
{
    setFt801(ft801);
}

void WelcomePageWidget::setup()
{
    ft801()->WriteCmd(CMD_INFLATE);
    ft801()->WriteCmd(0);
    ft801()->WriteCmdfromflash(welcome_screen, sizeof(welcome_screen));

    uint16_t pos = 0;
    uint16_t max = 120;
    while (pos < max){

        ft801()->DLStart();

        ft801()->Cmd_Gradient(240, 79, 0xFEFFFF, 240, 240, 0xA5A5A5);

        ft801()->BitmapHandle(0);
        ft801()->BitmapSource(0);//?
        ft801()->BitmapLayout(FT_ARGB1555, 960, 272);
        ft801()->BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 480, 272);

        ft801()->Begin(FT_BITMAPS);
        ft801()->Vertex2ii(0, 0, 0, 0);
        ft801()->End();

        ft801()->ColorRGB(218, 131, 0);
        ft801()->Cmd_FGColor(0xC8510B);
        ft801()->Cmd_BGColor(0x783508);
        ft801()->Cmd_Progress(30, 240, 420, 18, 0, pos, max);

        ft801()->DLEnd();
        ft801()->Finish();

        pos++;
        QThread::currentThread()->msleep(50);
    }
    terminate();
    emit done();
}
