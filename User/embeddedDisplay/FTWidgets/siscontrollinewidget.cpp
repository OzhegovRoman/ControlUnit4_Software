#include "siscontrollinewidget.h"
#include "../compactdraw.h"
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>
#include "inputwidget.h"

SisControlLineWidget::SisControlLineWidget(Gpu_Hal_Context_t *host)
    : FTWidget (nullptr)
    , mDriver (nullptr)
    , mUpdateFlag(false)
    , dataReady(false)
    , errorFlag(false)
    , dataTimer(new QTimer(this))
{
    setHost(host);

    dataTimer->setSingleShot(true);
    connect(dataTimer, &QTimer::timeout, this, &SisControlLineWidget::readData);
    connect(this, &SisControlLineWidget::restart, this, &SisControlLineWidget::exec);
}

void SisControlLineWidget::setup()
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
//    App_WrCoCmd_Buffer(host(), TAG(BT_SetCurrent));
//    Gpu_CoCmd_Button(host(), 12, 84, 290, 173, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG_MASK(0));

    Gpu_CoCmd_FgColor(host(),CD::themeColor(Grad_Buttons));
    Gpu_CoCmd_GradColor(host(),CD::themeColor(Grad_Top));

    CD::headPanel("CONTROL LINE", QString("Address: %1").arg(mDriver->devAddress()));
    CD::buttonBack();
    CD::mainBackground();
    CD::mainArea(320, 180);

    App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));
    Gpu_CoCmd_Text(host(), 16, 128, 31, 1024, "I:");
    Gpu_CoCmd_Text(host(), 290, 128, 31, OPT_CENTERY | OPT_RIGHTX, "mA");


    App_WrCoCmd_Buffer(host(), POINT_SIZE(100));

    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    dlOffset = Gpu_Hal_Rd16(host(), REG_CMD_DL);
    Gpu_CoCmd_Memcpy(host(), 100000L, RAM_DL, dlOffset);

    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 119, 0));
    Gpu_CoCmd_Spinner(host(), 240, 172, 0, 0);

    App_WrCoCmd_Buffer(host(), DISPLAY());
    Gpu_CoCmd_Swap(host());
    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    readData();
}

void SisControlLineWidget::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    static QElapsedTimer buttonTimer;
    static int timeToValueChange = 0;
    static int timerCounts = 0;
    uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);
    if (buttonTag && buttonTag!=255){

        if (lastButtonPressedTag == 0){
            qDebug()<<"start ticking";
            buttonTimer.start();
            timeToValueChange = 0;
            timerCounts = 0;
        }

        lastButtonPressedTag = buttonTag;

        float sign = -1;
        switch (buttonTag) {
        case BT_Plus:
            sign = 1;
        case BT_Minus:{
            if (timeToValueChange < buttonTimer.elapsed())
            {
                qDebug()<<buttonTimer.elapsed()<< timerCounts;
                mDriver->current()->setCurrentValue(mDriver->current()->currentValue() + sign * 2e-5f);
                // определяем время? когда будем следующий раз тикать
                timerCounts ++;
                timeToValueChange = 500;
                if (timerCounts > 1) timeToValueChange = 200;
                if (timerCounts > 9) timeToValueChange = 100;
                if (timerCounts > 29) timeToValueChange = 50;
                dataTimer->stop(); // пока запускаем это каждый раз, но...
                buttonTimer.start();
                update();
            }
            break;
        }

        }
    }
    else {
        switch (lastButtonPressedTag) {
        case BT_Back:{
            terminate();
            emit backClicked();
            lastButtonPressedTag = 0;
            return;
        }
        case BT_Short: {
            lastButtonPressedTag = 0;
            auto tmp = !mDriver->shortEnable()->currentValue();
            mDriver->shortEnable()->setValueSync(tmp);
            mDriver->shortEnable()->setCurrentValue(tmp);
            update();
            break;
        }
        case BT_Plus:
        case BT_Minus: {
            lastButtonPressedTag = 0;
            mDriver->current()->setValueSync(mDriver->current()->currentValue(), nullptr, 2);
            dataTimer->start(1000);
            update();
            break;
        }
        case  BT_SetCurrent: {
            qDebug()<<"set current";
            lastButtonPressedTag = 0;
            terminate();
            double newValue = InputWidget::getDouble(host(), "Current, mA");
            if (!qIsNaN(newValue))
                mDriver->current()->setValueSync(static_cast<float>(newValue*1e-3), nullptr, 2);
            QThread::currentThread()->msleep(100);
            emit restart();
            return;
        }
        }
    }

    if (mUpdateFlag){
        mUpdateFlag = false;

        if (mDriver == nullptr)
            return;

        Gpu_CoCmd_Dlstart(host());
        Gpu_CoCmd_Append(host(), 100000L, dlOffset);

        Gpu_CoCmd_Text(host(), 214, 128, 31, OPT_CENTERY | OPT_RIGHTX,
                       QString("%1")
                       .arg(static_cast<double>(mDriver->current()->currentValue()) * 1e3, 6,'f', 2)
                       .toLocal8Bit());
        // кнопки

        Gpu_CoCmd_FgColor(host(),CD::themeColor(Grad_Bottom));
        Gpu_CoCmd_GradColor(host(),CD::themeColor(Grad_Buttons));

        App_WrCoCmd_Buffer(host(), TAG_MASK(1));
        App_WrCoCmd_Buffer(host(), TAG(BT_Plus));
        Gpu_CoCmd_Button(host(), 333, 201, 138, 60, 31, buttonTag == BT_Plus ? OPT_FLAT : 0, "+");
        App_WrCoCmd_Buffer(host(), TAG(BT_Minus));
        Gpu_CoCmd_Button(host(), 13, 201, 138, 60, 31, buttonTag == BT_Minus ? OPT_FLAT : 0, "-");
        App_WrCoCmd_Buffer(host(), TAG(BT_SetCurrent));
        Gpu_CoCmd_Button(host(), 161, 201, 160, 60, 31, buttonTag == BT_SetCurrent ? OPT_FLAT : 0, "SET");

        Gpu_CoCmd_FgColor(host(), 0xC8510B);


        bool tmpBool = mDriver->shortEnable()->currentValue();

        Gpu_CoCmd_FgColor(host(), COLOR(CD::themeColor(Colors::SliderPoint)));
        Gpu_CoCmd_BgColor(host(), tmpBool ? CD::themeColor(Colors::SliderBG) : CD::themeColor(Colors::InnerArea));
        App_WrCoCmd_Buffer(host(), TAG(BT_Short));
        Gpu_CoCmd_Toggle(host(), 350, 120, 100, 30, 0, tmpBool ? 0 : 65535, "Short""\xFF""Open");

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

void SisControlLineWidget::readData()
{
    if (isStoped() || mDriver == nullptr)
        return;
    bool ok = false;
    mDriver->data()->getValueSync(&ok);

    dataReady = ok;
    errorFlag = !ok;

    update();
    dataTimer->start(1000);
}

void SisControlLineWidget::update()
{
    mUpdateFlag = true;
}

void SisControlLineWidget::setDriver(SisControlLineDriverM0 *driver)
{
    mDriver = driver;
}
