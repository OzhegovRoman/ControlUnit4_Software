#include "tempwidget.h"
#include <QDebug>

TempWidget::TempWidget(Gpu_Hal_Context_t *host)
    : FTWidget (nullptr)
    , mUpdateFlag(false)
    , mDriver(nullptr)
    , dataReady(false)
    , errorFlag(false)
    , dataTimer(new QTimer(this))
{
    setHost(host);
    dataTimer->setSingleShot(true);
    connect(dataTimer, &QTimer::timeout, this, &TempWidget::readData);
}

void TempWidget::setup()
{
    mUpdateFlag = false;

    if (host() == nullptr || mDriver == nullptr){
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
    Gpu_CoCmd_Text(host(), 240, 24, 31, OPT_CENTER, "Temperature Unit");
    Gpu_CoCmd_Text(host(), 240, 57, 26, OPT_CENTER, QString("Address: %1").arg(mDriver->devAddress()).toLocal8Bit());

    App_WrCoCmd_Buffer(host(), LINE_WIDTH(32));
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
    App_WrCoCmd_Buffer(host(), VERTEX2II(132, 117, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(352, 207, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(130, 115, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(350, 205, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), POINT_SIZE(100));

    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    dlOffset = Gpu_Hal_Rd16(host(), REG_CMD_DL);
    Gpu_CoCmd_Memcpy(host(), 100000L, RAM_DL, dlOffset);

    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 119, 0));
    Gpu_CoCmd_Spinner(host(), 240, 160, 0, 0);

    App_WrCoCmd_Buffer(host(), DISPLAY());
    Gpu_CoCmd_Swap(host());
    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    readData();
}

void TempWidget::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);
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

        Gpu_CoCmd_Dlstart(host());
        Gpu_CoCmd_Append(host(), 100000L, dlOffset);

        bool isConnected = qAbs(static_cast<double>(mDriver->temperature()->currentValue())) > 1e-5;
        QString tempStr;

        if (isConnected){
            App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
            tempStr = QString("T: %1 K").
                    arg(static_cast<double>(mDriver->temperature()->currentValue()), 4, 'f', 2);
        }
        else {
            App_WrCoCmd_Buffer(host(), COLOR_RGB(135, 135, 135));
            tempStr = "N/C";
        }
        Gpu_CoCmd_Text(host(), 240, 160, 31, 1536, tempStr.toLocal8Bit());

        if (dataReady){
            // зеленый
            App_WrCoCmd_Buffer(host(), COLOR_RGB(29, 204, 55));
            dataReady = false;
            update();
        }
        else  if (errorFlag){
            // красный
            App_WrCoCmd_Buffer(host(), COLOR_RGB(247, 38, 56));
            errorFlag = false;
            update();
        }
        else
            //серый
            App_WrCoCmd_Buffer(host(), COLOR_RGB(135, 135, 135));

        App_WrCoCmd_Buffer(host(), BEGIN(POINTS));
        App_WrCoCmd_Buffer(host(), VERTEX2II(342, 124, 0, 0));
        App_WrCoCmd_Buffer(host(), END());

        App_WrCoCmd_Buffer(host(), DISPLAY());
        Gpu_CoCmd_Swap(host());
        App_Flush_Co_Buffer(host());
        Gpu_Hal_WaitCmdfifo_empty(host());
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
