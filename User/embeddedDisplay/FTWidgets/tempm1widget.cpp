#include "tempm1widget.h"

#include "../compactdraw.h"

TempM1Widget::TempM1Widget(Gpu_Hal_Context_t *host)
    : FTWidget (nullptr)
    , mUpdateFlag(false)
    , mDriver(nullptr)
    , dataReady(false)
    , errorFlag(false)
    , dataTimer(new QTimer(this))
{
    setHost(host);
    dataTimer->setSingleShot(true);
    connect(dataTimer, &QTimer::timeout, this, &TempM1Widget::readData);
}

void TempM1Widget::setDriver(TempDriverM1 *driver)
{
    mDriver = driver;
}

void TempM1Widget::setup()
{
    mUpdateFlag = false;

    if (host() == nullptr || mDriver == nullptr){
        terminate();
        return;
    }

    Gpu_CoCmd_Dlstart(host());
    App_WrCoCmd_Buffer(host(), TAG_MASK(1));
   CD::dummyButton(BT_Dummy);
    App_WrCoCmd_Buffer(host(), TAG(BT_Back));
    Gpu_CoCmd_Button(host(), 10, 10, 50, 50, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG_MASK(0));


    CD::headPanel("Temperature Unit",QString("Address: %1").arg(mDriver->devAddress()));
    CD::buttonBack();
    CD::mainBackground();
    CD::mainArea(82,262,12,318);

    App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));
    int16_t top = 105;
    for (int i = 0; i < 4; ++i)
        if (mDriver->defaultParam(i).enable){
            Gpu_CoCmd_Text(host(), 20, top, 31, OPT_CENTERY, QString("T%1:").arg(i).toLocal8Bit());
            top += 45;
        }

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

void TempM1Widget::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);
    if (buttonTag && buttonTag != 255){
        if (lastButtonPressedTag != buttonTag){
            update();
            lastButtonPressedTag = buttonTag;
        }
    }
    else {
        switch (lastButtonPressedTag) {
        case BT_Back:
            terminate();
            emit backClicked();
            lastButtonPressedTag = 0;
            return;
        case BT_Relay5V:{
            lastButtonPressedTag = 0;
            cRelaysStatus status = mDriver->relaysStatus()->currentValue();
            status.setStatus(cRelaysStatus::ri5V, (status.status() & cRelaysStatus::ri5V) ? 0 : cRelaysStatus::ri5V);
            mDriver->relaysStatus()->setValueSync(status, nullptr, 5);
            mDriver->relaysStatus()->getValueSync(nullptr, 5);
            update();
            lastButtonPressedTag = 0;
            break;
        }
        case BT_Relay25V:{
            lastButtonPressedTag = 0;
            cRelaysStatus status = mDriver->relaysStatus()->currentValue();
            status.setStatus(cRelaysStatus::ri25V, (status.status() & cRelaysStatus::ri25V) ? 0 : cRelaysStatus::ri25V);
            mDriver->relaysStatus()->setValueSync(status, nullptr, 5);
            mDriver->relaysStatus()->getValueSync(nullptr, 5);
            update();
            lastButtonPressedTag = 0;
            break;
        }
        default:
            lastButtonPressedTag = 0;
            update();
            break;
        }
    }

    if (mUpdateFlag){

        mUpdateFlag = false;

        if (mDriver == nullptr)
            return;

        Gpu_CoCmd_Dlstart(host());
        Gpu_CoCmd_Append(host(), 100000L, dlOffset);

        int16_t top = 105;
        for (int i = 0; i < 4; ++i)
            if (mDriver->defaultParam(i).enable){
                Gpu_CoCmd_Text(host(), 290, top, 31, OPT_CENTERY | OPT_RIGHTX, QString("%1 K").arg(mDriver->currentTemperature(i), 4, 'f', 2).toLocal8Bit());
                top += 45;
            }

        App_WrCoCmd_Buffer(host(), TAG_MASK(1));
        App_WrCoCmd_Buffer(host(), TAG(BT_Relay5V));

        CD::toggleButton(345,88,120,69,
                         (mDriver->relaysStatus()->currentValue().status() & cRelaysStatus::ri5V),
                         (buttonTag == BT_Relay5V),
                         "Relay 5V");

        App_WrCoCmd_Buffer(host(), TAG(BT_Relay25V));

        CD::toggleButton(345,182,120,69,
                         (mDriver->relaysStatus()->currentValue().status() & cRelaysStatus::ri25V),
                         (buttonTag == BT_Relay25V),
                         "Relay 25V");

        App_WrCoCmd_Buffer(host(), TAG_MASK(0));


        ColoredStatus cs;
        if (dataReady){
           // зеленый
           cs = CS_Normal;
           dataReady = false;
           update();
           }
        else  if (errorFlag){
           // красный
           cs = CS_Fail;
           errorFlag = false;
           update();
           }
        else
           //серый
           cs = CS_Inactive;

        CD::updateIndicator(310,92,cs);

        App_WrCoCmd_Buffer(host(), DISPLAY());
        Gpu_CoCmd_Swap(host());
        App_Flush_Co_Buffer(host());
        Gpu_Hal_WaitCmdfifo_empty(host());

    }

    FTWidget::loop();
}

void TempM1Widget::readData()
{
    if (isStoped() || mDriver == nullptr)
        return;
    bool ok = mDriver->updateTemperature();
    if (ok) mDriver->relaysStatus()->getValueSync(&ok);

    dataReady = ok;
    errorFlag = !ok;
    update();
    dataTimer->start(1000);
}

void TempM1Widget::update()
{
    mUpdateFlag = true;
}
