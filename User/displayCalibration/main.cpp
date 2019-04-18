#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QSettings>

#define FT_DISPLAYWIDTH		FT_DISPLAY_HSIZE_WQVGA
#define FT_DISPLAYHEIGHT	FT_DISPLAY_VSIZE_WQVGA
#define FT_DISPLAY_RESOLUTION	FT_DISPLAY_WQVGA_480x272
#define FT_DISPLAY_ROTATE       1

#include "../FTDI/hardware/FT801/FT801.h"
#include "../FTDI/libraries/FT_GC/FT_Transport_SPI/FT_Transport_SPI_RaspPI.h"
#include "../FTDI/libraries/FT_GC/FT_GC.h"
#include "../FTDI/libraries/FT_GC/FT801/FT801Impl.h"

typedef FT801Impl<FT_Transport_SPI_RaspPi> FT801IMPL_SPI;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("cu-displaycalibration");
    QCoreApplication::setApplicationVersion(QString(VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple application to display calibration for Scontel\'s control unit (CU4)");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(a);


    FT801IMPL_SPI FTImpl;
    QSettings settings("Scontel", "embeddedDisplay");

    // инициализация дисплея
    FTImpl.Init(FT_DISPLAY_RESOLUTION);
    delay(20);

    //Read Register ID to check if FT800 is ready.
    unsigned int tmp = 0;
    int tryCount = 0;
    while ((FT801_CHIPID != tmp) && (tryCount++ < 10)){
        tmp = FTImpl.Read32(FT_ROM_CHIPID);
        qDebug()<<tmp;
    }

    if (tmp != FT801_CHIPID){
        qDebug()<<"Critical error!!! There is no any FT801 based display";
        return 1;
    }

    FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
    FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN);
    FTImpl.DisplayOn();
    FTImpl.AudioOff();


    FTImpl.DLStart();
    FTImpl.ClearColorRGB(64,64,64);
    FTImpl.Clear(1,1,1);
    FTImpl.ColorRGB(0xff,0xff,0xff);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, FT_DISPLAYHEIGHT/2, 27, FT_OPT_CENTER, "Please Tap on the dot.");
    FTImpl.Cmd_Calibrate(0);

    FTImpl.DLEnd();
    FTImpl.Finish();

    delay(20);

    tmp = FTImpl.Read32(REG_CTOUCH_TRANSFORM_A);
    qDebug()<<"TransformA"<<tmp;
    settings.setValue("TransformA", tmp);
    tmp = FTImpl.Read32(REG_CTOUCH_TRANSFORM_B);
    qDebug()<<"TransformB"<<tmp;
    settings.setValue("TransformB", tmp);
    tmp = FTImpl.Read32(REG_CTOUCH_TRANSFORM_C);
    qDebug()<<"TransformC"<<tmp;
    settings.setValue("TransformC", tmp);
    tmp = FTImpl.Read32(REG_CTOUCH_TRANSFORM_D);
    qDebug()<<"TransformD"<<tmp;
    settings.setValue("TransformD", tmp);
    tmp = FTImpl.Read32(REG_CTOUCH_TRANSFORM_E);
    qDebug()<<"TransformE"<<tmp;
    settings.setValue("TransformE", tmp);
    tmp = FTImpl.Read32(REG_CTOUCH_TRANSFORM_F);
    qDebug()<<"TransformF"<<tmp;
    settings.setValue("TransformF", tmp);

    FTImpl.DLStart();
    FTImpl.Clear(1,1,1);
    FTImpl.ColorRGB(0xff,0xff,0xff);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, FT_DISPLAYHEIGHT/2, 27, FT_OPT_CENTER, "Well Done!!");
    FTImpl.DLEnd();
    FTImpl.Finish();

    return 0;
}

