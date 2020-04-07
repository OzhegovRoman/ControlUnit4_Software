#include "systeminfo.h"
#include <QDebug>
#include <QtNetwork>
#include <QNetworkInterface>
#include <QFile>

SystemInfo::SystemInfo(FT801_SPI *ft801)
    : FTWidget (nullptr)
{
    setFt801(ft801);
}

void SystemInfo::setup()
{
    if (ft801() == nullptr){
        terminate();
        return;
    }

    ft801()->DLStart();

    ft801()->TagMask(1);
    ft801()->Tag(BT_Back);
    ft801()->Cmd_Button(17, 8, 48, 48, 27, 0, "");
    ft801()->TagMask(0);

    ft801()->Cmd_Gradient(464, 73, 0x3E3E3E, 464, 283, 0x000000);
    ft801()->LineWidth(16);

    ft801()->ColorRGB(130, 130, 130);
    ft801()->Begin(FT_LINES);
    ft801()->Vertex2ii(0, 68, 0, 0);
    ft801()->Vertex2ii(480, 68, 0, 0);
    ft801()->End();
    ft801()->ColorRGB(0,0,0);
    ft801()->Begin(FT_LINES);
    ft801()->Vertex2ii(0, 66, 0, 0);
    ft801()->Vertex2ii(480, 66, 0, 0);
    ft801()->End();

    ft801()->ColorRGB(255, 255, 255);
    ft801()->Cmd_Text(240, 32, 31, FT_OPT_CENTER, "System Info");

    ft801()->LineWidth(32);
    ft801()->ColorRGB(255, 255, 255);
    ft801()->Begin(FT_LINES);
    ft801()->Vertex2ii(30, 32, 0, 0);
    ft801()->Vertex2ii(58, 32, 0, 0);
    ft801()->Vertex2ii(26, 32, 0, 0);
    ft801()->Vertex2ii(40, 18, 0, 0);
    ft801()->Vertex2ii(26, 32, 0, 0);
    ft801()->Vertex2ii(40, 46, 0, 0);
    ft801()->End();

    ft801()->LineWidth(40);

    ft801()->ColorRGB(102, 85, 102);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(12, 84, 0, 0);
    ft801()->Vertex2ii(468, 256, 0, 0);
    ft801()->End();

    ft801()->ColorRGB(0, 0, 0);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(10, 82, 0, 0);
    ft801()->Vertex2ii(466, 254, 0, 0);
    ft801()->End();

    ft801()->ColorRGB(255,255,255);
    ft801()->Cmd_FGColor(0x000000);
    ft801()->Cmd_GradColor(0xffffff);

    ft801()->Cmd_Text(12, 112, 29, 0, QString("Software version: " + QString(VERSION)).toLocal8Bit());
    ft801()->Cmd_Text(12, 82, 29, 0, QString("TcpIp address: " + myTcpIpAddress()).toLocal8Bit());
    ft801()->Cmd_Text(12, 142, 29, 0, QString("Serial number: " + mySerialNumber()).toLocal8Bit());

    ft801()->DLEnd();
    ft801()->Finish();
}

void SystemInfo::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag = ft801()->Read(REG_TOUCH_TAG);
    if (buttonTag)
        lastButtonPressedTag = buttonTag;
    else {
        if (lastButtonPressedTag){

            lastButtonPressedTag = 0;
            terminate();
            emit backClicked();
        }
    }
    FTWidget::loop();
}

QString SystemInfo::myTcpIpAddress()
{
    for (QHostAddress address: QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost)
            return address.toString();
    return QString();
}

QString SystemInfo::mySerialNumber()
{
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly))
        return QString();
    QString str;
    do {
        QStringList strL = QString(file.readLine()).split(": ");
        if (strL[0].contains("Serial")){
            str = strL[1].simplified();
        }
    } while (!file.atEnd());

    file.close();
    return str;
}
