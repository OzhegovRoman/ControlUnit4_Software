#include "mainwidget.h"
#include <QDebug>
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
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
    App_WrCoCmd_Buffer(host(), TAG(BT_Info));
    Gpu_CoCmd_Button(host(), 430, 13, 44, 45, 27, 0, "");
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
    Gpu_CoCmd_Text(host(), 240, 24, 31, OPT_CENTER, "Control Unit");
    Gpu_CoCmd_Text(host(), 240, 56, 26, OPT_CENTER, "Available units list:");


    App_WrCoCmd_Buffer(host(), LINE_WIDTH(200));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(442, 26, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(462, 46, 0, 0));
    App_WrCoCmd_Buffer(host(), END());
    App_WrCoCmd_Buffer(host(), COLOR_RGB(66, 66, 66));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(443, 27, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(461, 45, 0, 0));
    App_WrCoCmd_Buffer(host(), END());
    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(host(), 452, 36, 30, OPT_CENTER, "i");

    uint16_t right = mHarvester->drivers().size() > maxDeviceListSize ? 434: 470;

    App_WrCoCmd_Buffer(host(), LINE_WIDTH(40));
    App_WrCoCmd_Buffer(host(), COLOR_RGB(102, 85, 102));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(12, 84, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(right, 256, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(10, 82, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(right - 2, 254, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

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
            qDebug()<<"Button pressed" << buttonTag;
        }
    }
    else {
        switch (lastButtonPressedTag) {
        case BT_Info:{
            qDebug()<<"Button released" << lastButtonPressedTag;
            lastButtonPressedTag = 0;
            terminate();
            emit systemInfoClicked();
            return;
        }
        default: {
            if (lastButtonPressedTag >= BT_DriverButton){
                qDebug()<<"Button released" << lastButtonPressedTag;
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
        Gpu_CoCmd_FgColor(host(),0xFF7514);
        Gpu_CoCmd_BgColor(host(),0x783508);

        App_WrCoCmd_Buffer(host(), TAG_MASK(1));
        App_WrCoCmd_Buffer(host(), TAG(BT_SCROLLER));
        Gpu_CoCmd_Scrollbar(host(), 444, 90, 26, 160, OPT_FLAT, mTopIndex, maxDeviceListSize, static_cast<uint16_t>(mHarvester->drivers().size()));
        Gpu_CoCmd_Track(host(), 441, 79, 33, 183, BT_SCROLLER);
        App_WrCoCmd_Buffer(host(), TAG_MASK(0));

        wideList = false;
    }


    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_FgColor(host(),0);
    Gpu_CoCmd_BgColor(host(),0xFFFFFF);

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
                // отрисовываем информацию по SSPD
                if (!dataInfo[i+mTopIndex].channelInited || sspddriver->status()->currentValue().stShorted)
                    // серый
                    App_WrCoCmd_Buffer(host(), COLOR(Colors::Inactive));
                else
                    if (qAbs(static_cast<double>(sspddriver->voltage()->currentValue()))<0.02){
                        // норм - белый
                        App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
                    }
                    else
                        // упал - оранжевый
                        App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 98, 46));

                Gpu_CoCmd_Text(host(), 16, top + 16, 28, OPT_CENTERY, QString("#%1 SSPD").arg(sspddriver->devAddress()).toLocal8Bit());

                if (dataInfo[i+mTopIndex].channelInited) {
                    Gpu_CoCmd_Text(host(), wideList ? 270 : 260, top + 16, 28, OPT_CENTERY | OPT_RIGHTX,
                                   QString("%1 uA")
                                   .arg(static_cast<double>(sspddriver->current()->currentValue()) * 1e6, 6,'f', 1)
                                   .toLocal8Bit());
                    QString tmp = QString("%1 cps")
                            .arg(static_cast<double>(sspddriver->counts()->currentValue() / sspddriver->params()->currentValue().Time_Const), 6, 'g', 4);
                    tmp.replace("e+0","e");
                    Gpu_CoCmd_Text(host(), wideList ? 434 : 398, top + 16, 28, OPT_CENTERY | OPT_RIGHTX,
                                   tmp.toLocal8Bit());
                }

                App_WrCoCmd_Buffer(host(), LINE_WIDTH(16));
                App_WrCoCmd_Buffer(host(), BEGIN(LINES));
                App_WrCoCmd_Buffer(host(), VERTEX2II(wideList ? 280: 270, static_cast<uint16_t>(top + 6), 0, 0));
                App_WrCoCmd_Buffer(host(), VERTEX2II(wideList ? 280: 270, static_cast<uint16_t>(top + 26), 0, 0));
                App_WrCoCmd_Buffer(host(), END());

            }
        }
        //тоже самое для SSpdDriverM1
        {
            auto sspddriver = qobject_cast<SspdDriverM1*>(mHarvester->drivers()[i+mTopIndex]);
            if (sspddriver){
                // отрисовываем информацию по SSPD
                if (!dataInfo[i+mTopIndex].channelInited || sspddriver->status()->currentValue().stShorted)
                    // серый
                    App_WrCoCmd_Buffer(host(), COLOR(Colors::Inactive));
                else
                    if (qAbs(static_cast<double>(sspddriver->voltage()->currentValue()))<0.02){
                        // норм - белый
                        App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
                    }
                    else
                        // упал - оранжевый
                        App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 98, 46));

                Gpu_CoCmd_Text(host(), 16, top + 16, 28, OPT_CENTERY, QString("#%1 SSPD").arg(sspddriver->devAddress()).toLocal8Bit());

                if (dataInfo[i+mTopIndex].channelInited) {
                    Gpu_CoCmd_Text(host(), wideList ? 270 : 260, top + 16, 28, OPT_CENTERY | OPT_RIGHTX,
                                   QString("%1 uA")
                                   .arg(static_cast<double>(sspddriver->current()->currentValue()) * 1e6, 6,'f', 1)
                                   .toLocal8Bit());
                    QString tmp = QString("%1 cps")
                            .arg(static_cast<double>(sspddriver->counts()->currentValue() / sspddriver->params()->currentValue().Time_Const), 6, 'g', 4);
                    tmp.replace("e+0","e");
                    Gpu_CoCmd_Text(host(), wideList ? 434 : 398, top + 16, 28, OPT_CENTERY | OPT_RIGHTX,
                                   tmp.toLocal8Bit());
                }

                App_WrCoCmd_Buffer(host(), LINE_WIDTH(16));
                App_WrCoCmd_Buffer(host(), BEGIN(LINES));
                App_WrCoCmd_Buffer(host(), VERTEX2II(wideList ? 280: 270, static_cast<uint16_t>(top + 6), 0, 0));
                App_WrCoCmd_Buffer(host(), VERTEX2II(wideList ? 280: 270, static_cast<uint16_t>(top + 26), 0, 0));
                App_WrCoCmd_Buffer(host(), END());

            }
        }
        //  а теперь попробуем скастовать в температуру
        {
            auto tempdriver =qobject_cast<TempDriverM0*>(mHarvester->drivers()[i+mTopIndex]);
            if (tempdriver){
                //удачно
                bool isConnected = qAbs(static_cast<double>(tempdriver->temperature()->currentValue())) > 1e-5;
                QString tempStr;

                if (dataInfo[i+mTopIndex].channelInited && isConnected){
                    App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
                    tempStr = QString("%1 K").
                            arg(static_cast<double>(tempdriver->temperature()->currentValue()), 4, 'f', 2);
                }
                else {
                    App_WrCoCmd_Buffer(host(), COLOR(Colors::Inactive));
                    tempStr = "Not connected";
                }
                Gpu_CoCmd_Text(host(), 16, top + 16, 28, OPT_CENTERY,
                               QString("#%1 Temperature").arg(tempdriver->devAddress()).toLocal8Bit());

                Gpu_CoCmd_Text(host(), wideList ? 318 : 300, top + 16, 28, OPT_CENTER,
                               tempStr.toLocal8Bit());
            }
        }
        auto tempdriver =qobject_cast<TempDriverM1*>(mHarvester->drivers()[i+mTopIndex]);
        if (tempdriver){
            //удачно
            QString tempStr;
            static int tempStrPos = 0;

            App_WrCoCmd_Buffer(host(), COLOR_RGB(255, 255, 255));
            for (uint8_t i = 0; i < 4; ++i){
                if (tempdriver->defaultParam(i).enable)
                    tempStr += QString("T%1: %2 K; ")
                            .arg(i)
                            .arg(static_cast<double>(tempdriver->currentTemperature(i)), 4, 'f', 2);
            }
            tempStr.chop(2);
            int textLength = tempStr.size()*11;
            int16_t center = wideList ? 309 : 291;

            Gpu_CoCmd_Text(host(), 16, top + 16, 28, OPT_CENTERY,
                           QString("#%1 Temperature").arg(tempdriver->devAddress()).toLocal8Bit());

            App_WrCoCmd_Buffer(host(), SCISSOR_XY(180, top));
            App_WrCoCmd_Buffer(host(), SCISSOR_SIZE(2 * (center-180), 32));
            Gpu_CoCmd_Text(host(), 180 - animationProcess * textLength/animationPeriod, top + 16, 28, OPT_CENTERY,
                           tempStr.toLocal8Bit());
            Gpu_CoCmd_Text(host(), 180 + textLength - animationProcess * textLength/animationPeriod, top + 16, 28, OPT_CENTERY,
                           tempStr.toLocal8Bit());
            App_WrCoCmd_Buffer(host(), SCISSOR_XY(0, 0));
            App_WrCoCmd_Buffer(host(), SCISSOR_SIZE(512, 512));
        }

        // передняя задняя палочка

        App_WrCoCmd_Buffer(host(), LINE_WIDTH(16));
        App_WrCoCmd_Buffer(host(), BEGIN(LINES));
        App_WrCoCmd_Buffer(host(), VERTEX2II(174, static_cast<uint16_t>(top + 6), 0, 0));
        App_WrCoCmd_Buffer(host(), VERTEX2II(174, static_cast<uint16_t>(top + 26), 0, 0));
        App_WrCoCmd_Buffer(host(), VERTEX2II(wideList ? 444 : 408, static_cast<uint16_t>(top + 6), 0, 0));
        App_WrCoCmd_Buffer(host(), VERTEX2II(wideList ? 444 : 408, static_cast<uint16_t>(top + 26), 0, 0));
        App_WrCoCmd_Buffer(host(), END());

        // общая для всех точка моргания сетодиодом
        if (dataInfo[i + mTopIndex].channelUpdated){
            // зеленый
            App_WrCoCmd_Buffer(host(), COLOR_RGB(39, 204, 55));
            dataInfo[i + mTopIndex].channelUpdated = false;
            update();
        }
        else  if (dataInfo[i + mTopIndex].channelError){
            // красный
            App_WrCoCmd_Buffer(host(), COLOR_RGB(247, 38, 56));
            dataInfo[i + mTopIndex].channelError = false;
            update();
        }
        else
            //серый
            App_WrCoCmd_Buffer(host(), COLOR(Colors::Inactive));

        App_WrCoCmd_Buffer(host(), BEGIN(POINTS));
        App_WrCoCmd_Buffer(host(), VERTEX2II(wideList ? 456: 420, static_cast<uint16_t>(top + 16), 0, 0));
        App_WrCoCmd_Buffer(host(), END());
    }

    App_WrCoCmd_Buffer(host(), DISPLAY());
    Gpu_CoCmd_Swap(host());
    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());
    //    }
    animationProcess = (animationProcess + 1) % animationPeriod;
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
