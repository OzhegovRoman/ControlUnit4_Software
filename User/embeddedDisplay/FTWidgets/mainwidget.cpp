#include "mainwidget.h"
#include <QDebug>
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
#include "Drivers/heaterdriverm0.h"
#include <QThread>

MainWidget::MainWidget(Gpu_Hal_Context_t *host, DataHarvester *harvester)
    : FTWidget (nullptr)
    , mHarvester(harvester)
    , mUpdateFlag(true)
    , mTopIndex(0)
    , dataHarvesterTimer (new QTimer(this))
{
    setHost(host);
    dataHarvesterTimer->setSingleShot(true);
    connect(dataHarvesterTimer, &QTimer::timeout, this, &MainWidget::dataHarvest);
}

void MainWidget::setup()
{
    mUpdateFlag = true;
    mTopIndex = 0;

    dataInfo.resize(mHarvester->drivers().size());
    for (int i = 0; i < dataInfo.size(); i++){
        dataInfo[i].channelInited = false;
        dataInfo[i].channelUpdated = false;
        dataInfo[i].channelError = false;
    }


    if (host() == nullptr){
        terminate();
        return;
    }

    Gpu_CoCmd_Dlstart(host());

    App_WrCoCmd_Buffer(host(), TAG_MASK(1));
    CD::dummyButton(BT_Dummy);
    App_WrCoCmd_Buffer(host(), TAG(BT_Info));
    Gpu_CoCmd_Button(host(), 420, 10, 50, 50, 27, 0, "");
    App_WrCoCmd_Buffer(host(), TAG_MASK(0));

    Gpu_CoCmd_FgColor(host(),CD::themeColor(Grad_Bottom));
    Gpu_CoCmd_GradColor(host(),CD::themeColor(Grad_Buttons));

    CD::headPanel("Control Unit","Available units list:");
    CD::buttonInfo();

    CD::mainBackground();

    uint16_t right = mHarvester->drivers().size() > maxDeviceListSize ? 434: 470;

    CD::mainArea(right);

    App_WrCoCmd_Buffer(host(), POINT_SIZE(100));

    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    dlOffset = Gpu_Hal_Rd16(host(), REG_CMD_DL);
    Gpu_CoCmd_Memcpy(host(), 100000L, RAM_DL, dlOffset);

    dataHarvesterTimer->start(100);
}

void MainWidget::loop()
{
    static uint32_t lastButtonPressedTag = 0;

    uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);

    if (buttonTag && (buttonTag != 255)){
        if (buttonTag != lastButtonPressedTag){
            update();
            lastButtonPressedTag = buttonTag;
        }
    }
    else {
        switch (lastButtonPressedTag) {
        case BT_Info:{
            lastButtonPressedTag = 0;
            terminate();
            emit systemInfoClicked();
            return;
        }
        default: {
            if (lastButtonPressedTag >= BT_DriverButton){
                terminate();
                emit channelChoosen(static_cast<int>(lastButtonPressedTag - BT_DriverButton));
                lastButtonPressedTag = 0;
                return;
            }
        }
        }
    }
    // положение скроллера
    if (mHarvester->drivers().size() > maxDeviceListSize){
        uint32_t track = Gpu_Hal_Rd32(host(), REG_TRACKER);
        if ( (track & 0xFF) == BT_SCROLLER){
            update();
            int32_t pos = (track>>16);
            pos *= mHarvester->drivers().size();
            pos -= (maxDeviceListSize << 15);
            pos>>=16;
            if (pos<0) pos = 0;
            if (pos > mHarvester->drivers().size() - maxDeviceListSize )
                pos = mHarvester->drivers().size() - maxDeviceListSize;
            mTopIndex = pos & 0xFF;
        }
    }
    //увеличил количество отрисовок, но добавил анимацию

    //    if (mUpdateFlag){
    //        mUpdateFlag = false;
    Gpu_CoCmd_Dlstart(host());

    Gpu_CoCmd_Append(host(), 100000L, dlOffset);
    //TODO: проверить на больших числах
    bool wideList = true;

    if (mHarvester->drivers().size() > maxDeviceListSize){
        Gpu_CoCmd_FgColor(host(),CD::themeColor(Colors::SliderPoint));
        Gpu_CoCmd_BgColor(host(),CD::themeColor(Colors::InnerArea));

        App_WrCoCmd_Buffer(host(), TAG_MASK(1));
        App_WrCoCmd_Buffer(host(), TAG(BT_SCROLLER));
        Gpu_CoCmd_Scrollbar(host(), 444, 90, 26, 160, 0, mTopIndex, maxDeviceListSize, static_cast<uint16_t>(mHarvester->drivers().size()));
        Gpu_CoCmd_Track(host(), 441, 79, 33, 183, BT_SCROLLER);
        App_WrCoCmd_Buffer(host(), TAG_MASK(0));

        Gpu_CoCmd_FgColor(host(),CD::themeColor(Grad_Bottom));

        wideList = false;
    }
    Gpu_CoCmd_BgColor(host(),CD::themeColor(Colors::InnerArea));

    drawer->setIsWideList(wideList);

    int size = qMin(mHarvester->drivers().size(), static_cast<int>(maxDeviceListSize));
    for (int16_t i = 0; i<size; i++){
        int16_t top = 84 + 34 * i;
        App_WrCoCmd_Buffer(host(), TAG_MASK(1));
        uint8_t buttonTag = BT_DriverButton + mTopIndex + static_cast<uint8_t>(i);
        App_WrCoCmd_Buffer(host(), TAG(buttonTag));

        Gpu_CoCmd_Button(host(), 11, top , wideList ? 456 : 420, 32, 30,
                         lastButtonPressedTag == buttonTag ? OPT_FLAT : 0,
                         "");
        App_WrCoCmd_Buffer(host(), TAG_MASK(0));

        {
            auto sspddriver = qobject_cast<SspdDriverM0*>(mHarvester->drivers()[i+mTopIndex]);
            if (sspddriver){
                ColoredStatus cs;
                // отрисовываем информацию по SSPD
                if (!dataInfo[i+mTopIndex].channelInited || sspddriver->status()->currentValue().stShorted) // серый
                    cs = ColoredStatus::CS_Inactive;
                else
                    if (qAbs(static_cast<double>(sspddriver->voltage()->currentValue()))<0.02){ // норм - белый
                        cs = ColoredStatus::CS_Normal;
                    }
                    else  // упал - оранжевый
                        cs = ColoredStatus::CS_Fail;
                QString uA, cps;
                if (dataInfo[i+mTopIndex].channelInited) {
                    uA = QString("%1 uA")
                            .arg(static_cast<double>(sspddriver->current()->currentValue()) * 1e6, 6,'f', 1);
                    cps = QString("%1 cps")
                            .arg(static_cast<double>(sspddriver->counts()->currentValue() / sspddriver->params()->currentValue().Time_Const), 6, 'g', 4);
                    cps.replace("e+0","e");
                }
                CD::listButtonText(cs,top,QString("#%1 SSPD").arg(sspddriver->devAddress()), uA, cps);
            }
        }
        //тоже самое для SSpdDriverM1
        {
            auto sspddriver = qobject_cast<SspdDriverM1*>(mHarvester->drivers()[i+mTopIndex]);

            if (sspddriver){
                ColoredStatus cs;
                // отрисовываем информацию по SSPD
                if (!dataInfo[i+mTopIndex].channelInited || sspddriver->status()->currentValue().stShorted)
                    // серый
                    cs = ColoredStatus::CS_Inactive;
                else
                    if (qAbs(static_cast<double>(sspddriver->voltage()->currentValue()))<0.02){
                        cs = ColoredStatus::CS_Normal; // норм - белый
                    }
                    else
                        cs = ColoredStatus::CS_Fail;   // упал - оранжевый
                QString uA, cps;
                if (dataInfo[i+mTopIndex].channelInited) {
                    uA = QString("%1 uA")
                            .arg(static_cast<double>(sspddriver->current()->currentValue()) * 1e6, 6,'f', 1);
                    cps = QString("%1 cps")
                            .arg(static_cast<double>(sspddriver->counts()->currentValue() / sspddriver->params()->currentValue().Time_Const), 6, 'g', 4);
                    cps.replace("e+0","e");
                }
                CD::listButtonText(cs,top,QString("#%1 SSPD").arg(sspddriver->devAddress()), uA, cps);

            }
        }
        //  а теперь попробуем скастовать в температуру
        {
            auto tempdriver =qobject_cast<TempDriverM0*>(mHarvester->drivers()[i+mTopIndex]);
            if (tempdriver){
                //удачно
                bool isConnected = qAbs(static_cast<double>(tempdriver->temperature()->currentValue())) > 1e-5;
                QString tempStr;
                ColoredStatus cs;

                if (dataInfo[i+mTopIndex].channelInited && isConnected){
                    cs = CS_Normal;
                    tempStr = QString("%1 K").
                            arg(static_cast<double>(tempdriver->temperature()->currentValue()), 4, 'f', 2);
                }
                else {
                    cs = CS_Inactive;
                    tempStr = "Not connected";
                }

                CD::listButtonText(cs,top,QString("#%1 Temperature").arg(tempdriver->devAddress()),tempStr);
            }
        }
        {
            auto tempdriver =qobject_cast<TempDriverM1*>(mHarvester->drivers()[i+mTopIndex]);
            if (tempdriver){
                //удачно
                QString tempStr;
                for (uint8_t i = 0; i < 4; ++i){
                    if (tempdriver->defaultParam(i).enable)
                        tempStr += QString("T%1: %2 K; ")
                                .arg(i)
                                .arg(static_cast<double>(tempdriver->currentTemperature(i)), 4, 'f', 2);
                }
                tempStr.chop(2);
                int16_t center = wideList ? 309 : 291;

                CD::listButtonText(CS_Normal,top,QString("#%1 Temperature").arg(tempdriver->devAddress()), QString());
                CD::animatedButtonText(CS_Normal, top, center, animationProcess, animationPeriod, tempStr);
            }
        }

        {
            auto heaterdriver =qobject_cast<HeaterDriverM0*>(mHarvester->drivers()[i+mTopIndex]);
            if (heaterdriver){
                //удачно
                QString tempStr;

                CD::listButtonText(CS_Inactive,top,QString("#%1 Heater").arg(heaterdriver->devAddress()), QString("No Data"));
            }
        }

        // общая для всех точка моргания сетодиодом
        ColoredStatus cs;
        if (dataInfo[i + mTopIndex].channelUpdated){
            // зеленый
            cs = CS_Normal;
            dataInfo[i + mTopIndex].channelUpdated = false;
            update();
        }
        else  if (dataInfo[i + mTopIndex].channelError){
            // красный
            cs = CS_Fail;
            dataInfo[i + mTopIndex].channelError = false;
            update();
        }
        else
            //серый
            cs = CS_Inactive;
        CD::updateIndicator(wideList ? 456: 420, top+16,cs);
    }

    App_WrCoCmd_Buffer(host(), DISPLAY());
    Gpu_CoCmd_Swap(host());
    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());


    animationProcess= (animationProcess + 1) % animationPeriod;

    FTWidget::loop();
}

void MainWidget::update()
{
    mUpdateFlag = true;
}

void MainWidget::dataHarvest()
{
    // идем по всем модулям
    int currentIndex = mTopIndex;
    bool ok = false;

    while ((currentIndex < mTopIndex + maxDeviceListSize) &&
           (currentIndex < mHarvester->drivers().size())) {
        // опрашиваем устройство

        if (isStoped()) return;

        {
            auto tempdriver = qobject_cast<TempDriverM0*>(mHarvester->drivers()[currentIndex]);
            if (tempdriver){
                // в случае успеха
                // начинаем вычитывать данные? в случае если он не включен? отрубаем его
                tempdriver->data()->getValueSync(&ok);

                if (ok  && !tempdriver->commutator()->currentValue())
                    tempdriver->commutator()->setValueSync(true);
            }
        }

        {
            auto tempdriver = qobject_cast<TempDriverM1*>(mHarvester->drivers()[currentIndex]);
            if (tempdriver){
                // в случае успеха
                ok = tempdriver->updateTemperature();
            }
        }

        {
            auto sspddriver = qobject_cast<SspdDriverM0*>(mHarvester->drivers()[currentIndex]);
            if (sspddriver){
                // в случае успеха
                sspddriver->data()->getValueSync(&ok);
                if (ok)
                    sspddriver->params()->getValueSync(&ok);
            }
        }

        {
            auto sspddriver = qobject_cast<SspdDriverM1*>(mHarvester->drivers()[currentIndex]);
            if (sspddriver){
                // в случае успеха
                sspddriver->data()->getValueSync(&ok);
                if (ok)
                    sspddriver->params()->getValueSync(&ok);
            }
        }
        {
            auto heaterdriver = qobject_cast<HeaterDriverM0*>(mHarvester->drivers()[currentIndex]);
            if (heaterdriver){
                // в случае успеха
                ok = true;
            }
        }

        if (ok)
            dataInfo[currentIndex].channelInited = true;
        dataInfo[currentIndex].channelError = !ok;
        dataInfo[currentIndex].channelUpdated = ok;

        currentIndex++;
        update();
    }

    // следующий
    dataHarvesterTimer->start(1000);
}
