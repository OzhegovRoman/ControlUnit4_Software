#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QSettings>

#include "riverdieve.h"

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

    DisplaySettings::getInstance().autoDetectDisplayGeneration();

    Gpu_Hal_Context_t host;
    QSettings settings("Scontel", "embeddedDisplay");

    App_Common_Init(&host);
    App_Calibrate_Screen(&host);


    //Read Register ID to check if FT800 is ready.
    unsigned int tmp = 0;

    tmp = Gpu_Hal_Rd32(&host, REG_TOUCH_TRANSFORM_A);
    qDebug()<<"TransformA"<<tmp;
    settings.setValue("TransformA", tmp);
    tmp = Gpu_Hal_Rd32(&host, REG_TOUCH_TRANSFORM_B);
    qDebug()<<"TransformB"<<tmp;
    settings.setValue("TransformB", tmp);
    tmp = Gpu_Hal_Rd32(&host, REG_TOUCH_TRANSFORM_C);
    qDebug()<<"TransformC"<<tmp;
    settings.setValue("TransformC", tmp);
    tmp = Gpu_Hal_Rd32(&host, REG_TOUCH_TRANSFORM_D);
    qDebug()<<"TransformD"<<tmp;
    settings.setValue("TransformD", tmp);
    tmp = Gpu_Hal_Rd32(&host, REG_TOUCH_TRANSFORM_E);
    qDebug()<<"TransformE"<<tmp;
    settings.setValue("TransformE", tmp);
    tmp = Gpu_Hal_Rd32(&host, REG_TOUCH_TRANSFORM_F);
    qDebug()<<"TransformF"<<tmp;
    settings.setValue("TransformF", tmp);

    Gpu_CoCmd_Dlstart(&host);

    App_WrCoCmd_Buffer(&host, CLEAR(1,1,1));
    App_WrCoCmd_Buffer(&host, COLOR_RGB(255,255,255));

    Gpu_CoCmd_Text(&host,
                   static_cast<int16_t>(DispWidth/2),
                   static_cast<int16_t>(DispHeight/2),
                   27,
                   static_cast<uint16_t>(OPT_CENTER), "Well Done!!");

    /* Calibration */
    App_WrCoCmd_Buffer(&host, DISPLAY());
    Gpu_CoCmd_Swap(&host);
    App_Flush_Co_Buffer(&host);
    Gpu_Hal_WaitCmdfifo_empty(&host);

    return 0;
}

