#include "welcomepagewidget.h"
#include <QThread>
#include "../compactdraw.h"
#include <QFile>

const uint8_t welcome_screen[]={
    #include "../Design/welcomepage_alt/images/Logo_2.binh"
};
const uint8_t welcome_screen_tirphotonics[]={
    #include "../Design/welcomepage_tirphot/images/Tirphotonics_2.binh"
};

WelcomePageWidget::WelcomePageWidget(Gpu_Hal_Context_t *host)
{
    CD::loadTheme();
    setHost(host);
}

void WelcomePageWidget::setup()
{
    Gpu_Hal_WrCmd32(host(), CMD_INFLATE);
    Gpu_Hal_WrCmd32(host(), 0);
    uint32_t image_width = 460;
    uint32_t image_height = 83;
    if (QFile::exists("/home/pi/.tirphotonics")){
        Gpu_Hal_WrCmdBuf(host(), const_cast<uint8_t*>(welcome_screen_tirphotonics), sizeof(welcome_screen_tirphotonics));
        image_height = 101;
    }
    else
        Gpu_Hal_WrCmdBuf(host(), const_cast<uint8_t*>(welcome_screen), sizeof(welcome_screen));

    uint16_t pos = 0;
#ifdef DEBUG
    uint16_t max = 10;
#else
    uint16_t max = 200;
#endif
    while (pos < max){
        Gpu_CoCmd_Dlstart(host());
        Gpu_CoCmd_Gradient(host(), 0, 0, 0xC4c8c8, 480, 272, 0xE2E6E6);

        App_WrCoCmd_Buffer(host(), BITMAP_HANDLE(0));

        App_WrCoCmd_Buffer(host(), BITMAP_SOURCE(0));
        App_WrCoCmd_Buffer(host(), BITMAP_LAYOUT(ARGB4, 920, 272));
        App_WrCoCmd_Buffer(host(), BITMAP_SIZE(BILINEAR, BORDER, BORDER, image_width, image_height));

        App_WrCoCmd_Buffer(host(), BEGIN(BITMAPS));
        App_WrCoCmd_Buffer(host(), VERTEX2II(10, 86, 0, 0));
        App_WrCoCmd_Buffer(host(), END());

        App_WrCoCmd_Buffer(host(), COLOR_RGB(218, 131, 0));
        Gpu_CoCmd_FgColor(host(), 0xC8510B);
        Gpu_CoCmd_BgColor(host(), 0x783508);
        Gpu_CoCmd_Progress(host(), 30, 240, 420, 18, 0, pos, max);

        App_WrCoCmd_Buffer(host(), DISPLAY());
        Gpu_CoCmd_Swap(host());
        App_Flush_Co_Buffer(host());
        Gpu_Hal_WaitCmdfifo_empty(host());

        pos++;
        Gpu_Hal_Sleep(50);
    }
    terminate();
    emit done();
}
