#include "displayinitializer.h"
#include <QDebug>
#include <QSettings>


DisplayInitializer::DisplayInitializer(Gpu_Hal_Context_t *host)
    : FTWidget (nullptr)
{
    setHost(host);
}

void DisplayInitializer::setup()
{
    if (host() == nullptr){
        qDebug()<<"Critical error!!! FT801 does not setted";
        exit(1);
    }

    if (!DisplaySettings::getInstance().autoDetectDisplayGeneration())
        exit(1);

    DisplaySettings::getInstance().setDisplaySize(DisplaySettings::ds43);

    App_Common_Init(host());    
    QSettings settings("Scontel", "embeddedDisplay");
    
    Gpu_Hal_Wr32(host(), REG_TOUCH_TRANSFORM_A, settings.value("TransformA", 4294942916).toUInt());
    Gpu_Hal_Wr32(host(), REG_TOUCH_TRANSFORM_B, settings.value("TransformB", 4294965629).toUInt());
    Gpu_Hal_Wr32(host(), REG_TOUCH_TRANSFORM_C, settings.value("TransformC", 32018140).toUInt());
    Gpu_Hal_Wr32(host(), REG_TOUCH_TRANSFORM_D, settings.value("TransformD", 250).toUInt());
    Gpu_Hal_Wr32(host(), REG_TOUCH_TRANSFORM_E, settings.value("TransformE", 4294943436).toUInt());
    Gpu_Hal_Wr32(host(), REG_TOUCH_TRANSFORM_F, settings.value("TransformF", 17614141).toUInt());

    Gpu_CoCmd_Dlstart(host());
    App_WrCoCmd_Buffer(host(), CLEAR_COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host(), CLEAR(1, 1, 1));
    App_WrCoCmd_Buffer(host(), DISPLAY());
    Gpu_CoCmd_Swap(host());
    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    terminate();

    emit initialized();
}

void DisplayInitializer::loop()
{
}
