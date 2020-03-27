#include "mainwidget.h"
#include <QDebug>
#include "Drivers/sspddriverm0.h"
#include "Drivers/tempdriverm0.h"
#include <QThread>

MainWidget::MainWidget(FT801_SPI *ft801, DataHarvester *harvester)
    : FTWidget (nullptr)
    , mHarvester(harvester)
    , mUpdateFlag(true)
    , mTopIndex(0)
    , dataHarvesterTimer (new QTimer(this))
{
    setFt801(ft801);
    dataHarvesterTimer->setSingleShot(true);
    connect(dataHarvesterTimer, &QTimer::timeout, this, &MainWidget::dataHarvest);
}

void MainWidget::setup()
{
    mUpdateFlag = true;
    mTopIndex = 0;

    qDebug()<<"MainWidget setup";

    qDebug()<<"mHarvester"<<mHarvester;

    dataInfo.resize(mHarvester->drivers().size());
    for (int i = 0; i < dataInfo.size(); i++){
        dataInfo[i].channelInited = false;
        dataInfo[i].channelUpdated = false;
        dataInfo[i].channelError = false;
    }


    if (ft801() == nullptr){
        terminate();
        return;
    }

    ft801()->DLStart();
    ft801()->TagMask(1);
    ft801()->Tag(BT_Info);
    ft801()->Cmd_Button(430, 13, 44, 45, 27, 0, "");
    ft801()->TagMask(0);

    ft801()->Cmd_Gradient(464, 73, 0x3E3E3E, 464, 283, 0x000000);
    ft801()->ColorRGB(130, 130, 130);

    ft801()->LineWidth(16);
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
    ft801()->Cmd_Text(240, 24, 31, FT_OPT_CENTER, "Control Unit");
    ft801()->Cmd_Text(240, 56, 26, FT_OPT_CENTER, "Available units list:");

    ft801()->Begin(FT_RECTS);
    ft801()->LineWidth(200);
    ft801()->Vertex2ii(442, 26, 0, 0);
    ft801()->Vertex2ii(462, 46, 0, 0);
    ft801()->End();
    ft801()->ColorRGB(66, 66, 66);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(443, 27, 0, 0);
    ft801()->Vertex2ii(461, 45, 0, 0);
    ft801()->End();
    ft801()->ColorRGB(255, 255, 255);
    ft801()->Cmd_Text(452, 36, 30, FT_OPT_CENTER, "i");

    uint16_t right = mHarvester->drivers().size() > maxDeviceListSize ? 434: 470;

    ft801()->LineWidth(40);
    ft801()->ColorRGB(102, 85, 102);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(12, 84, 0, 0);
    ft801()->Vertex2ii(right, 256, 0, 0);
    ft801()->End();

    ft801()->ColorRGB(0, 0, 0);
    ft801()->Begin(FT_RECTS);
    ft801()->Vertex2ii(10, 82, 0, 0);
    ft801()->Vertex2ii(right - 2, 254, 0, 0);
    ft801()->End();

    ft801()->PointSize(100);

    dlOffset = ft801()->Read16(REG_CMD_DL);
    ft801()->Cmd_Memcpy(100000L, FT_RAM_DL, dlOffset);

    dataHarvesterTimer->start(100);
}

void MainWidget::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    uint8_t buttonTag = ft801()->Read(REG_TOUCH_TAG);
    if (buttonTag){
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
        uint32_t track = ft801()->Read32(REG_TRACKER);
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

    if (mUpdateFlag){
        mUpdateFlag = false;
        ft801()->DLStart();
        ft801()->Cmd_Append(100000L, dlOffset);
        //TODO: проверить на больших числах
        bool wideList = true;

        if (mHarvester->drivers().size() > maxDeviceListSize){
            ft801()->Cmd_FGColor(0xFF7514);
            ft801()->Cmd_BGColor(0x783508);

            ft801()->TagMask(1);
            ft801()->Tag(BT_SCROLLER);
            ft801()->Cmd_Scrollbar(444, 90, 26, 160, FT_OPT_FLAT, mTopIndex, maxDeviceListSize, static_cast<uint16_t>(mHarvester->drivers().size()));
            ft801()->Cmd_Track(441, 79, 33, 183, BT_SCROLLER);
            ft801()->TagMask(0);

            wideList = false;
        }


        ft801()->ColorRGB(255, 255, 255);
        ft801()->Cmd_FGColor(0);
        ft801()->Cmd_BGColor(0xFFFFFF);
        int size = qMin(mHarvester->drivers().size(), static_cast<int>(maxDeviceListSize));
        for (int16_t i = 0; i<size; i++){
            int16_t top = 84 + 34 * i;
            ft801()->TagMask(1);
            uint8_t buttonTag = BT_DriverButton + mTopIndex + static_cast<uint8_t>(i);
            ft801()->Tag(buttonTag);
            ft801()->Cmd_Button(11, top , wideList ? 456 : 420, 32, 30,
                                lastButtonPressedTag == buttonTag ? FT_OPT_FLAT : 0,
                                "");
            ft801()->TagMask(0);

            auto sspddriver = qobject_cast<SspdDriverM0*>(mHarvester->drivers()[i+mTopIndex]);
            if (sspddriver){
                // отрисовываем информацию по SSPD
                if (!dataInfo[i+mTopIndex].channelInited || sspddriver->status()->currentValue().stShorted)
                    // серый
                    ft801()->ColorRGB(135, 135, 135);
                else
                    if (qAbs(static_cast<double>(sspddriver->voltage()->currentValue()))<0.02){
                        // норм - белый
                        ft801()->ColorRGB(255,255,255);
                    }
                    else
                        // упал - оранжевый
                        ft801()->ColorRGB(255, 98, 46);

                ft801()->Cmd_Text(16, top + 16, 28, FT_OPT_CENTERY, QString("#%1 SSPD").arg(sspddriver->devAddress()).toLocal8Bit());

                if (dataInfo[i+mTopIndex].channelInited) {
                    ft801()->Cmd_Text(wideList ? 270 : 260, top + 16, 28, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                                      QString("%1 uA")
                                      .arg(static_cast<double>(sspddriver->current()->currentValue()) * 1e6, 6,'f', 1)
                                      .toLocal8Bit());
                    ft801()->Cmd_Text(wideList ? 434 : 398, top + 16, 28, FT_OPT_CENTERY | FT_OPT_RIGHTX,
                                      QString("%1 cps")
                                      .arg(static_cast<double>(sspddriver->counts()->currentValue() / sspddriver->params()->currentValue().Time_Const), 6, 'g', 4)
                                      .toLocal8Bit());
                }

                ft801()->LineWidth(16);
                ft801()->Begin(FT_LINES);
                ft801()->Vertex2ii(wideList ? 280: 270, static_cast<uint16_t>(top + 6), 0, 0);
                ft801()->Vertex2ii(wideList ? 280: 270, static_cast<uint16_t>(top + 26), 0, 0);
                ft801()->End();


            }
            //  а теперь попробуем скачстовать в температуру
            auto tempdriver =qobject_cast<TempDriverM0*>(mHarvester->drivers()[i+mTopIndex]);
            if (tempdriver){
                //удачно
                bool isConnected = qAbs(static_cast<double>(tempdriver->temperature()->currentValue())) > 1e-5;
                QString tempStr;

                if (dataInfo[i+mTopIndex].channelInited && isConnected){
                    ft801()->ColorRGB(255,255,255);
                    tempStr = QString("%1 K").
                            arg(static_cast<double>(tempdriver->temperature()->currentValue()), 4, 'f', 2);
                }
                else {
                    ft801()->ColorRGB(135, 135, 135);
                    tempStr = "Not connected";
                }
                ft801()->Cmd_Text(16, top + 16, 28, FT_OPT_CENTERY, QString("#%1 Temperature").arg(tempdriver->devAddress()).toLocal8Bit());

                ft801()->Cmd_Text(wideList ? 318 : 300, top + 16, 28, FT_OPT_CENTER,
                                  tempStr.toLocal8Bit());
            }

            // передняя задняя палочка

            ft801()->LineWidth(16);
            ft801()->Begin(FT_LINES);
            ft801()->Vertex2ii(174, static_cast<uint16_t>(top + 6), 0, 0);
            ft801()->Vertex2ii(174, static_cast<uint16_t>(top + 26), 0, 0);
            ft801()->Vertex2ii(wideList ? 444 : 408, static_cast<uint16_t>(top + 6), 0, 0);
            ft801()->Vertex2ii(wideList ? 444 : 408, static_cast<uint16_t>(top + 26), 0, 0);
            ft801()->End();

            // общая для всех точка моргания сетодиодом
            if (dataInfo[i + mTopIndex].channelUpdated){
                // зеленый
                ft801()->ColorRGB(29, 204, 55);
                dataInfo[i + mTopIndex].channelUpdated = false;
                update();
            }
            else  if (dataInfo[i + mTopIndex].channelError){
                // красный
                ft801()->ColorRGB(247, 38, 56);
                dataInfo[i + mTopIndex].channelError = false;
                update();
            }
            else
                //серый
                ft801()->ColorRGB(135, 135, 135);

            ft801()->Begin(FT_POINTS);
            ft801()->Vertex2ii(wideList ? 456: 420, static_cast<uint16_t>(top + 16), 0, 0);
            ft801()->End();
        }

        ft801()->DLEnd();
        ft801()->Finish();
    }

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

        auto tempdriver = qobject_cast<TempDriverM0*>(mHarvester->drivers()[currentIndex]);
        if (tempdriver){
            // в случае успеха
            // начинаем вычитывать данные? в случае если он не включен? отрубаем его
            tempdriver->data()->getValueSync(&ok);

            if (ok  && !tempdriver->commutator()->currentValue())
                tempdriver->commutator()->setValueSync(true);
        }
        auto sspddriver = qobject_cast<SspdDriverM0*>(mHarvester->drivers()[currentIndex]);
        if (sspddriver){
            // в случае успеха
            // начинаем вычитывать данные? в случае если он не включен? отрубаем его
            sspddriver->data()->getValueSync(&ok);
            if (ok)
                sspddriver->params()->getValueSync(&ok);
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
