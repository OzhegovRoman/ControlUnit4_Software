#include "systeminfo.h"
#include <QDebug>
#include <QtNetwork>
#include <QNetworkInterface>
#include <QFile>

SystemInfo::SystemInfo(Gpu_Hal_Context_t *host)
    : FTWidget (nullptr)
{
    setHost(host);
}

void SystemInfo::setup()
{
    if (host() == nullptr){
        terminate();
        return;
    }

    Gpu_CoCmd_Dlstart(host());
    App_WrCoCmd_Buffer(host(), TAG_MASK(1));
    App_WrCoCmd_Buffer(host(), TAG(BT_Back));
    Gpu_CoCmd_Button(host(), 17, 8, 48, 48, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG_MASK(0));

    Gpu_CoCmd_Gradient(host(), 464, 73, 0x3E3E3E, 464, 283, 0x000000);
    App_WrCoCmd_Buffer(host(), LINE_WIDTH(16));

    App_WrCoCmd_Buffer(host(), COLOR_RGB(130, 130, 130));
    App_WrCoCmd_Buffer(host(), BEGIN(LINES));
    App_WrCoCmd_Buffer(host(), VERTEX2II(0, 68, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(480, 68, 0, 0));
    App_WrCoCmd_Buffer(host(), END());
    App_WrCoCmd_Buffer(host(), COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host(), BEGIN(LINES));
    App_WrCoCmd_Buffer(host(), VERTEX2II(0, 66, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(480, 66, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(host(), 240, 32, 31, OPT_CENTER, "System Info");

    App_WrCoCmd_Buffer(host(), LINE_WIDTH(32));
    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(host(), BEGIN(LINES));
    App_WrCoCmd_Buffer(host(), VERTEX2II(30, 32, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(58, 32, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(26, 32, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(40, 18, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(26, 32, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(40, 46, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), LINE_WIDTH(40));

    App_WrCoCmd_Buffer(host(), COLOR_RGB(102, 85, 102));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(12, 84, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(468, 256, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(10, 82, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(466, 254, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_FgColor(host(), 0x000000);
    Gpu_CoCmd_GradColor(host(), 0xffffff);

    Gpu_CoCmd_Text(host(), 12, 112, 29, 0, QString("Software version: " + QString(VERSION)).toLocal8Bit());
    Gpu_CoCmd_Text(host(), 12, 82, 29, 0, QString("TcpIp address: " + myTcpIpAddress()).toLocal8Bit());
    Gpu_CoCmd_Text(host(), 12, 142, 29, 0, QString("Serial number: " + mySerialNumber()).toLocal8Bit());

    App_WrCoCmd_Buffer(host(), DISPLAY());
    Gpu_CoCmd_Swap(host());
    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());
}

void SystemInfo::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);
    if (buttonTag && (buttonTag  == BT_Back))
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
