#include "heaterwidget.h"
#include "../compactdraw.h"

#include <QElapsedTimer>
#include <QDebug>
#include "inputwidget.h"

HeaterWidget::HeaterWidget(Gpu_Hal_Context_t *host)
    : FTWidget (nullptr)
    , mDriver (nullptr)
{
    setHost(host);
}

void HeaterWidget::setup()
{
    if (host() == nullptr || mDriver == nullptr){
        terminate();
        return;
    }

    Gpu_CoCmd_Dlstart(host());
    App_WrCoCmd_Buffer(host(), TAG_MASK(1));
    CD::dummyButton(BT_Dummy);
    App_WrCoCmd_Buffer(host(), TAG(BT_Back));
    Gpu_CoCmd_Button(host(), 10, 10, 50, 50, 27, 0, "");

    App_WrCoCmd_Buffer(host(), TAG(BT_MaxCurrent));
    Gpu_CoCmd_Button(host(), 16, 85, 442, 28, 29, 0, "");
    App_WrCoCmd_Buffer(host(), TAG(BT_FrontEdge));
    Gpu_CoCmd_Button(host(), 16, 119, 442, 28, 29, 0, "");
    App_WrCoCmd_Buffer(host(), TAG(BT_HoldTime));
    Gpu_CoCmd_Button(host(), 16, 153, 442, 28, 29, 0, "");
    App_WrCoCmd_Buffer(host(), TAG(BT_RearEdge));
    Gpu_CoCmd_Button(host(), 16, 187, 442, 28, 29, 0, "");
    App_WrCoCmd_Buffer(host(), TAG(BT_Heat));
    Gpu_CoCmd_Button(host(), 18, 225, 460, 30, 29, 0, "");

    App_WrCoCmd_Buffer(host(), TAG_MASK(0));

    Gpu_CoCmd_FgColor(host(),CD::themeColor(Grad_Buttons));
    Gpu_CoCmd_GradColor(host(),CD::themeColor(Grad_Top));

    CD::headPanel("Heater", QString("Address: %1").arg(mDriver->devAddress()));
    CD::buttonBack();
    CD::mainBackground();
    CD::mainArea(470, 218);

    QStringList rows;
    rows<<
           "Max Current (mA):" <<
           "Front edge (sec):" <<
           "Hold Time (sec):" <<
           "Rear edge (sec):";

    uint8_t increment = 34;
    App_WrCoCmd_Buffer(host(), LINE_WIDTH(16));
    App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextInactive)));
    App_WrCoCmd_Buffer(host(), BEGIN(LINES));
    for (int i = 0; i < rows.count()-1; ++i) {
        App_WrCoCmd_Buffer(host(), VERTEX2II(18,  82 + ((i + 1)*increment), 0, 0));
        App_WrCoCmd_Buffer(host(), VERTEX2II(462, 82 + ((i + 1)*increment), 0, 0));
    }
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));

    for (int i = 0; i < rows.count(); ++i) {
        Gpu_CoCmd_Text(host(), 18, 82 + (i*increment) + increment / 2, 29, OPT_CENTERY, rows.at(i).toLocal8Bit());
    }

    App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextInactive)));
    App_WrCoCmd_Buffer(host(), LINE_WIDTH(16));
    App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
    App_WrCoCmd_Buffer(host(), VERTEX2II(10,  225, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(470, 270, 0, 0));
    App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(Grad_Top)));
    App_WrCoCmd_Buffer(host(), VERTEX2II(12,  227, 0, 0));
    App_WrCoCmd_Buffer(host(), VERTEX2II(468, 268, 0, 0));
    App_WrCoCmd_Buffer(host(), END());

    App_WrCoCmd_Buffer(host(), POINT_SIZE(100));

    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    dlOffset = Gpu_Hal_Rd16(host(), REG_CMD_DL);
    Gpu_CoCmd_Memcpy(host(), 100000L, RAM_DL, dlOffset);

    App_WrCoCmd_Buffer(host(), DISPLAY());
    Gpu_CoCmd_Swap(host());
    App_Flush_Co_Buffer(host());
    Gpu_Hal_WaitCmdfifo_empty(host());

    bool ok = false;
    mDriver->eepromConst()->getValueSync(&ok, 5);
    update();
}

void HeaterWidget::loop()
{
    static uint32_t lastButtonPressedTag = 0;
    static QElapsedTimer buttonTimer;
    static QElapsedTimer heatingTimer;

    static int timeToValueChange = 0;
    static int timerCounts = 0;

    static unsigned int progressPressing = 0;
    static bool heating = false;

    static int waitingtime = 5000;
    static int stoptime = 500;

    uint8_t buttonTag = Gpu_Hal_Rd8(host(), REG_TOUCH_TAG);
    if (buttonTag && buttonTag != 255){

        if (lastButtonPressedTag == 0){
            qDebug()<<"start ticking";
            buttonTimer.start();
            timeToValueChange = 0;
            timerCounts = 0;
        }

        lastButtonPressedTag = buttonTag;

        switch (buttonTag) {
        case BT_Heat: {
            if (buttonTimer.elapsed() <= 1000){
                progressPressing = static_cast<unsigned int>(buttonTimer.elapsed() / 10);
                if (progressPressing > 100) progressPressing = 100;
                update();
            }
            else {
                if (progressPressing != 0){
                    progressPressing = 0;
                    if (!heating ){
                        //startHeating
                        heatingTimer.start();
                        auto data = mDriver->eepromConst()->currentValue();
                        waitingtime = static_cast<int>((data.holdTime+ data.rearEdgeTime + data.frontEdgeTime)*1000);
                        stoptime = static_cast<int>(data.rearEdgeTime * 1000);
                        mDriver->eepromConst()->setValueSync(data, nullptr, 5);
                        mDriver->startHeating()->executeSync(nullptr, 5);
                        heating = true;
                    }
                    else {
                        //emergencyStop
                        if (waitingtime > heatingTimer.elapsed() + stoptime){
                            waitingtime = heatingTimer.elapsed() + stoptime;
                            mDriver->emergencyStop()->executeSync(nullptr, 5);
                        }
                    }
                    update();
                }
            }
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
        case BT_MaxCurrent:{
            double temp = InputWidget::getDouble(host(), "Max current, mA");
            if (!qIsNaN(temp) && (temp >=0)){
                auto data = mDriver->eepromConst()->currentValue();
                data.maximumCurrent = static_cast<float>(temp)/1000.0;
                mDriver->eepromConst()->setCurrentValue(data);
            }
            lastButtonPressedTag = 0;
            update();
            break;
        }
        case BT_FrontEdge:{
            double temp = InputWidget::getDouble(host(), "Front Edge, sec");
            if (!qIsNaN(temp) && (temp >=0)){
                auto data = mDriver->eepromConst()->currentValue();
                data.frontEdgeTime = static_cast<float>(temp);
                mDriver->eepromConst()->setCurrentValue(data);
            }
            lastButtonPressedTag = 0;
            update();
            break;
        }
        case BT_HoldTime:{
            double temp = InputWidget::getDouble(host(), "Hold Time, sec");
            if (!qIsNaN(temp) && (temp >=0)){
                auto data = mDriver->eepromConst()->currentValue();
                data.holdTime = static_cast<float>(temp);
                mDriver->eepromConst()->setCurrentValue(data);
            }
            lastButtonPressedTag = 0;
            update();
            break;
        }
        case BT_RearEdge:{
            double temp = InputWidget::getDouble(host(), "Rear edge, sec");
            if (!qIsNaN(temp) && (temp >=0)){
                auto data = mDriver->eepromConst()->currentValue();
                data.rearEdgeTime = static_cast<float>(temp);
                mDriver->eepromConst()->setCurrentValue(data);
            }
            lastButtonPressedTag = 0;
            update();
            break;
        }
        case BT_Heat:{
            progressPressing = 0;
            lastButtonPressedTag = 0;
            update();
            break;
        }
        }
    }

    if (heating){
        update();
        if (heatingTimer.elapsed()>waitingtime){
            //stopHeating
            heating = false;
        }

    }

    if (mUpdateFlag){
        mUpdateFlag = false;

        if (mDriver == nullptr)
            return;

        Gpu_CoCmd_Dlstart(host());
        Gpu_CoCmd_Append(host(), 100000L, dlOffset);

        if (progressPressing != 0){
            App_WrCoCmd_Buffer(host(), COLOR(0xFF8411));
            App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
            App_WrCoCmd_Buffer(host(), VERTEX2II(12,  227, 0, 0));
            App_WrCoCmd_Buffer(host(), VERTEX2II(static_cast<uint32_t>(12.0 + 456.0 * progressPressing / 100.0) , 268, 0, 0));
            App_WrCoCmd_Buffer(host(), END());
            App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));
            Gpu_CoCmd_Text(host(), 236, 248, 30, OPT_CENTER, heating? "Emergency stop" : "Start Heating");
        }
        else if (heating){
            App_WrCoCmd_Buffer(host(), COLOR(0x39BFEF));
            App_WrCoCmd_Buffer(host(), BEGIN(RECTS));
            App_WrCoCmd_Buffer(host(), VERTEX2II(12,  227, 0, 0));
            App_WrCoCmd_Buffer(host(), VERTEX2II(static_cast<uint32_t>(12.0 + 456.0 * heatingTimer.elapsed()/ waitingtime) , 268, 0, 0));
            App_WrCoCmd_Buffer(host(), END());
            App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));
            Gpu_CoCmd_Text(host(), 236, 248, 30, OPT_CENTER, (heatingTimer.elapsed()+ stoptime< waitingtime) ? "Heating. Press for stop" : "Stoping");
        } else {
            App_WrCoCmd_Buffer(host(), COLOR(CD::themeColor(TextNormal)));
            Gpu_CoCmd_Text(host(), 236, 248, 30, OPT_CENTER, "Hold for start Heating");
        }

        auto data = mDriver->eepromConst()->currentValue();

        Gpu_CoCmd_Text(host(), 450, 99, 29, OPT_CENTERY | OPT_RIGHTX,
                       QString("%1").arg(static_cast<double>(data.maximumCurrent) * 1000.0, 6, 'f', 1).toLocal8Bit());
        Gpu_CoCmd_Text(host(), 450, 133, 29, OPT_CENTERY | OPT_RIGHTX,
                       QString("%1").arg(static_cast<double>(data.frontEdgeTime), 6, 'f', 1).toLocal8Bit());
        Gpu_CoCmd_Text(host(), 450, 167, 29, OPT_CENTERY | OPT_RIGHTX,
                       QString("%1").arg(static_cast<double>(data.holdTime), 6, 'f', 1).toLocal8Bit());
        Gpu_CoCmd_Text(host(), 450, 201, 29, OPT_CENTERY | OPT_RIGHTX,
                       QString("%1").arg(static_cast<double>(data.rearEdgeTime), 6, 'f', 1).toLocal8Bit());




        App_WrCoCmd_Buffer(host(), DISPLAY());
        Gpu_CoCmd_Swap(host());
        App_Flush_Co_Buffer(host());
        Gpu_Hal_WaitCmdfifo_empty(host());


    }

    FTWidget::loop();
}

void HeaterWidget::setDriver(HeaterDriverM0 *driver)
{
    mDriver = driver;
}

void HeaterWidget::update()
{
    mUpdateFlag = true;
}
