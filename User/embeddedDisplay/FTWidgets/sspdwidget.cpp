#include "sspdwidget.h"
#include <QDebug>
#include <QElapsedTimer>
#include "inputwidget.h"
#include "sspddriveroption.h"
#include <QThread>

#include "../compactdraw.h"

SspdWidget::SspdWidget(Gpu_Hal_Context_t *host)
    : FTWidget (nullptr)
    , mUpdateFlag(false)
    , mDriver(nullptr)
    , dataReady(false)
    , errorFlag(false)
    , dataTimer(new QTimer(this))
{
    setHost(host);
    dataTimer->setSingleShot(true);
    connect(dataTimer, &QTimer::timeout, this, &SspdWidget::readData);
    connect(this, &SspdWidget::restart, this, &SspdWidget::exec);
}

void SspdWidget::setDriver(SspdDriverM0 *driver)
{
    mDriver = driver;
}

void SspdWidget::setup()
{
    mUpdateFlag = false;

    if (host() == nullptr || mDriver == nullptr){
        terminate();
        return;
    }

    Gpu_CoCmd_Dlstart(host());
    App_WrCoCmd_Buffer(host(), TAG_MASK(1));
    App_WrCoCmd_Buffer(host(), TAG(BT_Dummy));
    Gpu_CoCmd_Button(host(), 0, 0, 480, 270, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG(BT_Back));
     Gpu_CoCmd_Button(host(), 17, 8, 48, 48, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG(BT_Option));
    Gpu_CoCmd_Button(host(), 412, 8, 48, 48, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG(BT_SetCurrent));
    Gpu_CoCmd_Button(host(), 12, 84, 290, 173, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG_MASK(0));

    Gpu_CoCmd_FgColor(host(),CD::themeColor(Grad_Buttons));
    Gpu_CoCmd_GradColor(host(),CD::themeColor(Grad_Top));

    CD::headPanel("SSPD", QString("Address: %1").arg(mDriver->devAddress()));
    CD::buttonBack();
    CD::buttonMenu();
    CD::mainBackground();
    CD::mainArea(320);


//    Gpu_CoCmd_Gradient(host(), 464, 73, 0x3E3E3E, 464, 283, 0x000000);
//    App_WrCoCmd_Buffer(host(), LINE_WIDTH(16));

//    App_WrCoCmd_Buffer(host(), COLOR_RGB(130, 130, 130));
//    App_WrCoCmd_Buffer(host(), BEGIN(LINES));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(0, 68, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(480, 68, 0, 0));
//    App_WrCoCmd_Buffer(host(), END());
//    App_WrCoCmd_Buffer(host(), COLOR_RGB(0, 0, 0));
//    App_WrCoCmd_Buffer(host(), BEGIN(LINES));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(0, 66, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(480, 66, 0, 0));
//    App_WrCoCmd_Buffer(host(), END());

//    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
//    Gpu_CoCmd_Text(host(), 240, 24, 31, OPT_CENTER, "SSPD");
//    Gpu_CoCmd_Text(host(), 240, 57, 26, OPT_CENTER, QString("Address: %1").arg(mDriver->devAddress()).toLocal8Bit());

//    //кнопка назад и меню
//    App_WrCoCmd_Buffer(host(), LINE_WIDTH(32));
//    App_WrCoCmd_Buffer(host(), BEGIN(LINES));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(30, 32, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(58, 32, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(26, 32, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(40, 18, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(26, 32, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(40, 46, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(420, 32, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(452, 32, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(420, 20, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(452, 20, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(420, 44, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(452, 44, 0, 0));
//    App_WrCoCmd_Buffer(host(), END());

//    App_WrCoCmd_Buffer(host(), LINE_WIDTH(40));

//    App_WrCoCmd_Buffer(host(), COLOR_RGB(102, 85, 102));
//    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(14, 85, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(320, 258, 0, 0));
//    App_WrCoCmd_Buffer(host(), END());

//    App_WrCoCmd_Buffer(host(), COLOR_RGB(0, 0, 0));
//    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(12, 83, 0, 0));
//    App_WrCoCmd_Buffer(host(), VERTEX2II(318, 256, 0, 0));
//    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));
    Gpu_CoCmd_Text(host(), 16,106,31, 1024, "I:");
    Gpu_CoCmd_Text(host(), 12, 166, 31, OPT_CENTERY, "U:");
    Gpu_CoCmd_Text(host(), 12, 226, 31, 1024, "cps:");
    Gpu_CoCmd_Text(host(), 290, 106, 31, OPT_CENTERY | OPT_RIGHTX, "uA");
    Gpu_CoCmd_Text(host(), 290, 166, 31, OPT_CENTERY | OPT_RIGHTX, "mV");


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

void SspdWidget::loop()
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
                mDriver->current()->setCurrentValue(mDriver->current()->currentValue() + sign * 1e-7f);
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
                auto tmp = mDriver->status()->currentValue();
                tmp.stShorted = !tmp.stShorted;

                mDriver->shortEnable()->setValueSync(tmp.stShorted);
                mDriver->status()->setCurrentValue(tmp);
                update();
                break;
            }
            case BT_Amp: {
                lastButtonPressedTag = 0;
                auto tmp = mDriver->status()->currentValue();
                tmp.stAmplifierOn = !tmp.stAmplifierOn;

                mDriver->amplifierEnable()->setValueSync(tmp.stAmplifierOn);
                mDriver->status()->setCurrentValue(tmp);
                update();
                break;
            }
            case BT_Counter: {
                lastButtonPressedTag = 0;
                auto tmp = mDriver->status()->currentValue();
                qDebug()<<tmp.Data;
                bool tmpBool = !(tmp.stCounterOn | tmp.stRfKeyToCmp | tmp.stComparatorOn);

                tmp.stCounterOn =    tmpBool;
                tmp.stRfKeyToCmp =   tmpBool;
                tmp.stComparatorOn = tmpBool;

                mDriver->status()->setValueSync(tmp);
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
                double newValue = InputWidget::getDouble(host(), "Current, uA");
                if (!isnan(newValue))
                    mDriver->current()->setValueSync(static_cast<float>(newValue*1e-6), nullptr, 2);
                QThread::currentThread()->msleep(100);
                emit restart();
                return;
            }
            case BT_Option:{
                qDebug()<<"option";
                lastButtonPressedTag = 0;
                terminate();
                SspdDriverOption::getOptions(host(), mDriver);
                emit restart();
                //вводим новое значение тока
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

        Gpu_CoCmd_Text(host(), 214, 106, 31, OPT_CENTERY | OPT_RIGHTX,
                          QString("%1")
                          .arg(static_cast<double>(mDriver->current()->currentValue()) * 1e6, 6,'f', 1)
                          .toLocal8Bit());
        Gpu_CoCmd_Text(host(), 214, 166, 31, OPT_CENTERY | OPT_RIGHTX,
                          QString("%1")
                          .arg(static_cast<double>(mDriver->voltage()->currentValue()) * 1e3, 6,'f', 1)
                          .toLocal8Bit());
        QString tmp = QString("%1")
                .arg(static_cast<double>(mDriver->counts()->currentValue())/mDriver->params()->currentValue().Time_Const, 6, 'g', 4);
        tmp.replace("e+0","e");

        Gpu_CoCmd_Text(host(), 290, 226, 31, OPT_CENTERY | OPT_RIGHTX,
                          tmp.toLocal8Bit());

        // кнопки

        Gpu_CoCmd_FgColor(host(),CD::themeColor(Grad_Bottom));
        Gpu_CoCmd_GradColor(host(),CD::themeColor(Grad_Buttons));

        App_WrCoCmd_Buffer(host(), TAG_MASK(1));
        App_WrCoCmd_Buffer(host(), TAG(BT_Plus));
        Gpu_CoCmd_Button(host(), 403, 201, 65, 60, 31, buttonTag == BT_Plus ? OPT_FLAT : 0, "+");
        App_WrCoCmd_Buffer(host(), TAG(BT_Minus));
        Gpu_CoCmd_Button(host(), 332, 201, 65, 60, 31, buttonTag == BT_Minus ? OPT_FLAT : 0, "-");

        Gpu_CoCmd_FgColor(host(), 0xC8510B);


        bool tmpBool = mDriver->status()->currentValue().stShorted;
        CD::sliderButton(348,88,"Short""\xFF""Open",BT_Short,tmpBool,100,false);
        //        Gpu_CoCmd_BgColor(host(), tmpBool ? 0x525252 : 0x783508);
//        App_WrCoCmd_Buffer(host(), TAG(BT_Short));
//        Gpu_CoCmd_Toggle(host(), 348, 88, 100, 28, 0, tmpBool ? 0: 65535, "Short""\xFF""Open");

        tmpBool = mDriver->status()->currentValue().stAmplifierOn;

        CD::sliderButton(348,128,"Amp""\xFF""Amp",BT_Amp,tmpBool,100);

//        Gpu_CoCmd_BgColor(host(), tmpBool ? 0x783508 : 0x525252);
//        App_WrCoCmd_Buffer(host(), TAG(BT_Amp));
//        Gpu_CoCmd_Toggle(host(), 348, 128, 100, 28, 0, tmpBool ? 65535 : 0, "Amp""\xFF""Amp");

        tmpBool = mDriver->status()->currentValue().stCounterOn &
                mDriver->status()->currentValue().stRfKeyToCmp &
                mDriver->status()->currentValue().stComparatorOn;


        CD::sliderButton(348,168,"Counter""\xFF""Counter",BT_Counter,tmpBool,100);
//        Gpu_CoCmd_BgColor(host(), tmpBool ? 0x783508 : 0x525252);
//        App_WrCoCmd_Buffer(host(), TAG(BT_Counter));
//        Gpu_CoCmd_Toggle(host(), 348, 168, 100, 28, 0, tmpBool ? 65535 : 0, "Counter""\xFF""Counter");
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

//        App_WrCoCmd_Buffer(host(), BEGIN(POINTS));
//        App_WrCoCmd_Buffer(host(), VERTEX2II(310, 92, 0, 0));
//        App_WrCoCmd_Buffer(host(), END());

        App_WrCoCmd_Buffer(host(), DISPLAY());
        Gpu_CoCmd_Swap(host());
        App_Flush_Co_Buffer(host());
        Gpu_Hal_WaitCmdfifo_empty(host());
    }

    FTWidget::loop();
}

void SspdWidget::readData()
{
    if (isStoped() || mDriver == nullptr)
        return;
    bool ok = false;
    mDriver->data()->getValueSync(&ok);
    if (ok){
        mDriver->params()->getValueSync(&ok);
    }

    dataReady = ok;
    errorFlag = !ok;
    update();
    dataTimer->start(1000);
}

void SspdWidget::update()
{
    mUpdateFlag = true;
}
