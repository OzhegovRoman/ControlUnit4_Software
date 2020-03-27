#include "displayinitializer.h"
#include <QDebug>
#include <QSettings>

DisplayInitializer::DisplayInitializer(FT801_SPI *ft801)
    : FTWidget (nullptr)
{
    setFt801(ft801);
}

void DisplayInitializer::setup()
{
    if (ft801() == nullptr){
        qDebug()<<"Critical error!!! FT801 does not setted";
        exit(1);
    }

    ft801()->Init(FT_DISPLAY_RESOLUTION);

    if (!isFT801present())
        return;

    ft801()->SetDisplayEnablePin(FT_DISPENABLE_PIN);
    ft801()->SetAudioEnablePin(FT_AUDIOENABLE_PIN);
    ft801()->DisplayOn();
    ft801()->AudioOff();

    QSettings settings("Scontel", "embeddedDisplay");

    ft801()->Write32(REG_CTOUCH_TRANSFORM_A,settings.value("TransformA", 4294942054).toUInt());
    ft801()->Write32(REG_CTOUCH_TRANSFORM_B,settings.value("TransformB", 520).toUInt());
    ft801()->Write32(REG_CTOUCH_TRANSFORM_C,settings.value("TransformC", 31452914).toUInt());
    ft801()->Write32(REG_CTOUCH_TRANSFORM_D,settings.value("TransformD", 405).toUInt());
    ft801()->Write32(REG_CTOUCH_TRANSFORM_E,settings.value("TransformE", 4294941440).toUInt());
    ft801()->Write32(REG_CTOUCH_TRANSFORM_F,settings.value("TransformF", 18322989).toUInt());

    ft801()->DLStart();
    ft801()->Finish();
    ft801()->DLStart();
    ft801()->DLEnd();
    ft801()->Finish();

    ft801()->DLStart();
    ft801()->ClearColorRGB(0,0,0);
    ft801()->Clear(1, 1, 1);
    ft801()->DLEnd();
    ft801()->Finish();

    terminate();

    emit initialized();
}

void DisplayInitializer::loop()
{
}

// TODO: странный выход. Всегда правда/ Надо как то по другому чтоле
bool DisplayInitializer::isFT801present()
{
    //Read Register ID to check if FT800 is ready.
    unsigned int tmp = 0;
    int tryCount = 0;
    while ((FT801_CHIPID != tmp) && (tryCount++ < 10)){
        tmp = ft801()->Read32(FT_ROM_CHIPID);
        qDebug()<<tmp;
    }

    if (tmp != FT801_CHIPID){
        qDebug()<<"Critical error!!! There is no any FT801 based display";
        exit(1);
    }
    return true;
}
