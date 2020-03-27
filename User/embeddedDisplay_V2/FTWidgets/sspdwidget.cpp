#include "sspdwidget.h"
#include <QDebug>
#include <QElapsedTimer>
#include "inputwidget.h"
#include "sspddriveroption.h"
#include <QThread>

SspdWidget::SspdWidget(FT801_SPI *ft801)
    : FTWidget (nullptr)
    , mUpdateFlag(false)
    , mDriver(nullptr)
    , dataReady(false)
    , errorFlag(false)
    , dataTimer(new QTimer(this))
{
    setFt801(ft801);
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

    if (ft801() == nullptr || mDriver == nullptr){
        terminate();
        return;
    }

    ft801()->DLStart();

    ft801()->TagMask(1);
    ft801()->Tag(BT_Back);
    ft801()->Cmd_Button(17, 8, 48, 48, 27, 0, "");
    ft801()->Tag(BT_Option);
    ft801()->Cmd_Button(412, 8, 48, 48, 27, 0, "");
    ft801()->Tag(BT_SetCurrent);
    ft801()->Cmd_Button(12, 84, 290, 173, 27, 0, "");

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
    ft801()->Cmd_Text(240,24,31, 1536, "SSPD");
    ft801()->Cmd_Text(240,57,26, 1536, QString("Address: %1").arg(mDriver->devAddress()).toLocal8Bit());

    //кнопка назад и меню
    ft801()->LineWidth(32);
    ft801()->ColorRGB(255, 255, 255);
    ft801()->Begin(FT_LINES);
    ft801()->Vertex2ii(30, 32, 0, 0);
    ft801()->Vertex2ii(58, 32, 0, 0);
    ft801()->Vertex2ii(26, 32, 0, 0);
    ft801()->Vertex2ii(40, 18, 0, 0);
    ft801()->Vertex2ii(26, 32, 0, 0);
    ft801()->Vertex2ii(40, 46, 0, 0);
    ft801()->Vertex2ii(420, 32, 0, 0);
    ft801()->Vertex2ii(452, 32, 0, 0);
    ft801()->Vertex2ii(420, 20, 0, 0);
    ft801()->Vertex2ii(452, 20, 0, 0);
    ft801()->Vertex2ii(420, 44, 0, 0);
    ft801()->Vertex2ii(452, 44, 0, 0);
    ft801()->End();

    ft801()->LineWidth(40);

    ft801()->ColorRGB(102, 85, 102);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(14, 85, 0, 0);
    ft801()->Vertex2ii(320, 258, 0, 0);
    ft801()->End();

    ft801()->ColorRGB(0, 0, 0);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(12, 83, 0, 0);
    ft801()->Vertex2ii(318, 256, 0, 0);
    ft801()->End();

    ft801()->ColorRGB(255,255,255);
    ft801()->Cmd_Text(16,106,31, 1024, "I:");
    ft801()->Cmd_Text(12, 166, 31, FT_OPT_CENTERY, "U:");
    ft801()->Cmd_Text(12, 226, 31, 1024, "cps:");
    ft801()->Cmd_Text(290, 106, 31, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                      QString("uA")
                      .toLocal8Bit());
    ft801()->Cmd_Text(290, 166, 31, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                      QString("mV")
                      .toLocal8Bit());


    ft801()->PointSize(100);
    ft801()->ColorRGB(255,255,255);

    dlOffset = ft801()->Read16(REG_CMD_DL);
    ft801()->Cmd_Memcpy(100000L, FT_RAM_DL, dlOffset);

    ft801()->ColorRGB(255, 119, 0);
    ft801()->Cmd_Spinner(240, 172, 0, 1);

    ft801()->DLEnd();
    ft801()->Finish();

    readData();
}

void SspdWidget::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    static QElapsedTimer buttonTimer;
    static int timeToValueChange = 0;
    static int timerCounts = 0;
    uint8_t buttonTag = ft801()->Read(REG_TOUCH_TAG);
    if (buttonTag){

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
                double newValue = InputWidget::getDouble(ft801(), "Current, uA");
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
                SspdDriverOption::getOptions(ft801(), mDriver);
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

        ft801()->DLStart();
        ft801()->Cmd_Append(100000L, dlOffset);


        ft801()->Cmd_Text(214, 106, 31, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                          QString("%1")
                          .arg(static_cast<double>(mDriver->current()->currentValue()) * 1e6, 6,'f', 1)
                          .toLocal8Bit());
        ft801()->Cmd_Text(214, 166, 31, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                          QString("%1")
                          .arg(static_cast<double>(mDriver->voltage()->currentValue()) * 1e3, 6,'f', 1)
                          .toLocal8Bit());
        ft801()->Cmd_Text(290, 226, 31, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                          QString("%1")
                          .arg(static_cast<double>(mDriver->counts()->currentValue())/mDriver->params()->currentValue().Time_Const, 6, 'g', 3)
                          .toLocal8Bit());

        // кнопки
        ft801()->Cmd_FGColor(0x2b2b2b);
        ft801()->Cmd_GradColor(0xffffff);

        ft801()->TagMask(1);
        ft801()->Tag(BT_Plus);
        ft801()->Cmd_Button(403, 201, 65, 60, 31, buttonTag == BT_Plus ? FT_OPT_FLAT : 0, "+");
        ft801()->Tag(BT_Minus);
        ft801()->Cmd_Button(332, 201, 65, 60, 31, buttonTag == BT_Minus ? FT_OPT_FLAT : 0, "-");

        ft801()->Cmd_FGColor(0xC8510B);
        bool tmpBool = mDriver->status()->currentValue().stShorted;
        ft801()->Cmd_BGColor(tmpBool ? 0x525252 : 0x783508);
        ft801()->Tag(BT_Short);
        ft801()->Cmd_Toggle(348, 88, 100, 28, 0, tmpBool ? 0: 65535, "Short""\xFF""Open");

        tmpBool = mDriver->status()->currentValue().stAmplifierOn;
        ft801()->Cmd_BGColor(tmpBool ? 0x783508 : 0x525252);
        ft801()->Tag(BT_Amp);
        ft801()->Cmd_Toggle(348, 128, 100, 28, 0, tmpBool ? 65535 : 0, "Amp""\xFF""Amp");

        tmpBool = mDriver->status()->currentValue().stCounterOn &
                mDriver->status()->currentValue().stRfKeyToCmp &
                mDriver->status()->currentValue().stComparatorOn;
        ft801()->Cmd_BGColor(tmpBool ? 0x783508 : 0x525252);
        ft801()->Tag(BT_Counter);
        ft801()->Cmd_Toggle(348, 168, 100, 28, 0, tmpBool ? 65535 : 0, "Counter""\xFF""Counter");
        ft801()->TagMask(0);


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
        ft801()->Vertex2ii(310, 92, 0, 0);
        ft801()->End();

        ft801()->DLEnd();
        ft801()->Finish();
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
