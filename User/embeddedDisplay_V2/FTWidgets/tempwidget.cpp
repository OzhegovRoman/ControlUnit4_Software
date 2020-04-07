#include "tempwidget.h"
#include <QDebug>

TempWidget::TempWidget(FT801_SPI *ft801)
    : FTWidget (nullptr)
    , mUpdateFlag(false)
    , mDriver(nullptr)
    , dataReady(false)
    , errorFlag(false)
    , dataTimer(new QTimer(this))
{
    setFt801(ft801);
    dataTimer->setSingleShot(true);
    connect(dataTimer, &QTimer::timeout, this, &TempWidget::readData);
}

void TempWidget::setup()
{
    mUpdateFlag = false;

    if (ft801() == nullptr || mDriver == nullptr){
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
    ft801()->Cmd_Text(240,24,31, 1536, "Temperature");
    ft801()->Cmd_Text(240,57,26, 1536, QString("Address: %1").arg(mDriver->devAddress()).toLocal8Bit());

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
    ft801()->Vertex2ii(132, 117, 0, 0);
    ft801()->Vertex2ii(352, 207, 0, 0);
    ft801()->End();

    ft801()->ColorRGB(0, 0, 0);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(130, 115, 0, 0);
    ft801()->Vertex2ii(350, 205, 0, 0);
    ft801()->End();

    ft801()->PointSize(100);

    dlOffset = ft801()->Read16(REG_CMD_DL);
    ft801()->Cmd_Memcpy(100000L, FT_RAM_DL, dlOffset);

    ft801()->ColorRGB(255, 119, 0);
    ft801()->Cmd_Spinner(240, 160, 0, 0);

    ft801()->DLEnd();
    ft801()->Finish();

    readData();
}

void TempWidget::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag = ft801()->Read(REG_TOUCH_TAG);
    if (buttonTag)
        lastButtonPressedTag = buttonTag;
    else {
        if (lastButtonPressedTag){
            qDebug()<<"button pressed";

            lastButtonPressedTag = 0;
            terminate();
            emit backClicked();
            return;
        }
    }

    if (mUpdateFlag){

        mUpdateFlag = false;

        if (mDriver == nullptr)
            return;

        ft801()->DLStart();
        ft801()->Cmd_Append(100000L, dlOffset);

        bool isConnected = qAbs(static_cast<double>(mDriver->temperature()->currentValue())) > 1e-5;
        QString tempStr;

        if (isConnected){
            ft801()->ColorRGB(255,255,255);
            tempStr = QString("T: %1 K").
                    arg(static_cast<double>(mDriver->temperature()->currentValue()), 4, 'f', 2);
        }
        else {
            ft801()->ColorRGB(135, 135, 135);
            tempStr = "N/C";
        }
        ft801()->Cmd_Text(240,160,31, 1536, tempStr.toLocal8Bit());

        if (dataReady){
            // зеленый
            ft801()->ColorRGB(29, 204, 55);
            dataReady = false;
            update();
        }
        else  if (errorFlag){
            // красный
            ft801()->ColorRGB(247, 38, 56);
            errorFlag = false;
            update();
        }
        else
            //серый
            ft801()->ColorRGB(135, 135, 135);

        ft801()->Begin(FT_POINTS);
        ft801()->Vertex2ii(342, 124, 0, 0);
        ft801()->End();

        ft801()->DLEnd();
        ft801()->Finish();

    }
    FTWidget::loop();
}

void TempWidget::readData()
{
    if (isStoped() || mDriver == nullptr)
        return;
    bool ok = false;
    mDriver->data()->getValueSync(&ok);

    if (ok  && !mDriver->commutator()->currentValue())
        mDriver->commutator()->setValueSync(true);
    dataReady = ok;
    errorFlag = !ok;
    update();
    dataTimer->start(1000);
}

void TempWidget::update()
{
    mUpdateFlag = true;
}

void TempWidget::setDriver(TempDriverM0 *driver)
{
    mDriver = driver;
}
