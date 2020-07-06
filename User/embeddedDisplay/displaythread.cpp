#include "displaythread.h"
#include <unistd.h>
#include <QDebug>

#include "Interfaces/curs485iointerface.h"
#include "servercommands.h"
#include "../qCustomLib/qCustomLib.h"

#include <QSettings>

const uint8_t arrow_left[]={
    #include "Design/ESE/images/arrow_left_2.binh"
};
const uint8_t arrow_right[]={
    #include "Design/ESE/images/arrow_right_2.binh"
};
const uint8_t welcome_screen[]={
    #include "Design/ESE/images/LOGO.binh"
};

#define ARROW_LEFT_ADDRESS   0
#define ARROW_RIGHT_ADDRESS  8192
#define WELCOME_SCREEN_ADDRESS  0


cDisplay::cDisplay(QObject *parent):
    QObject(parent),
    mPowerMode(pmNormal),
    mButtonTag(0),
    mLastButtonTag(-1),
    mCurrentIndex(-1),
    mInterface(nullptr),
    mWorkerTimer(new QTimer(this)),
    mDisplayTimer(new QTimer(this)),
    mFinishAll(false),
    mFinished(false),
    mSspdButtonIndex(0),
    mSspdData(true),
    mWDGTimer(new QTimer(this))
{
}

void cDisplay::initialize()
{
    // инициализация дисплея
    FTImpl.Init(FT_DISPLAY_RESOLUTION);
    delay(20);

    if (!isFT801founded())
        return;

    displaySetUp();

    welcomeScreen();

    QThread::msleep(WelcomeScreenTime);

    uploadArrows();
    showSpinner();

    initializeInterface();

    connect(mWorkerTimer, SIGNAL(timeout()), SLOT(workerTimeOut()));
    mWorkerTimer->setSingleShot(true);
    mWorkerTimer->start(500);

    connect(mDisplayTimer, SIGNAL(timeout()), SLOT(displayTimeOut()));
    mDisplayTimer->setSingleShot(true);
    mDisplayTimer->start(50);

    connect(this, SIGNAL(quitSignal()), SLOT(finish()));

    connect(mWDGTimer, SIGNAL(timeout()), SLOT(watchDogTimeOut()));
    mWDGTimer->setSingleShot(true);

    mWDGTimer->start(WatchDogTimeOut);
}

void cDisplay::finish()
{
    qDebug()<<"Finished";
    mFinishAll = true;
    doFinish();
}


bool cDisplay::isTcpIpProtocol() const
{
    return mTcpIpProtocol;
}

void cDisplay::setTcpIpProtocolEnabled(bool value)
{
    mTcpIpProtocol = value;
}

QString cDisplay::tcpIpAddress() const
{
    return mTcpIpAddress;
}

void cDisplay::setTcpIpAddress(const QString &value)
{
    mTcpIpAddress = value;
}

QString cDisplay::rs485PortName() const
{
    return mRs485PortName;
}

void cDisplay::setRs485PortName(const QString &rs485PortName)
{
    mRs485PortName = rs485PortName;
}

bool cDisplay::isFinished() const
{
    return mFinished;
}

bool cDisplay::isFT801founded()
{
    //Read Register ID to check if FT800 is ready.
    unsigned int tmp = 0;
    int tryCount = 0;
    while ((FT801_CHIPID != tmp) && (tryCount++ < 10)){
        tmp = FTImpl.Read32(FT_ROM_CHIPID);
        qDebug()<<tmp;
    }

    if (tmp != FT801_CHIPID){
        qDebug()<<"Critical error!!! There is no any FT801 based display";
        exit(1);
    }
    return true;
}

void cDisplay::displaySetUp()
{
    FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
    FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN);
    FTImpl.DisplayOn();
    FTImpl.AudioOff();

    QSettings settings("Scontel", "embeddedDisplay");

    FTImpl.Write32(REG_CTOUCH_TRANSFORM_A,settings.value("TransformA", 4294942054).toUInt());
    FTImpl.Write32(REG_CTOUCH_TRANSFORM_B,settings.value("TransformB", 520).toUInt());
    FTImpl.Write32(REG_CTOUCH_TRANSFORM_C,settings.value("TransformC", 31452914).toUInt());
    FTImpl.Write32(REG_CTOUCH_TRANSFORM_D,settings.value("TransformD", 405).toUInt());
    FTImpl.Write32(REG_CTOUCH_TRANSFORM_E,settings.value("TransformE", 4294941440).toUInt());
    FTImpl.Write32(REG_CTOUCH_TRANSFORM_F,settings.value("TransformF", 18322989).toUInt());

    FTImpl.DLStart();
    FTImpl.Finish();
    FTImpl.DLStart();
    FTImpl.DLEnd();
    FTImpl.Finish();

    FTImpl.DLStart();
    FTImpl.ClearColorRGB(0,0,0);
    FTImpl.Clear(1, 1, 1);
    FTImpl.DLEnd();
    FTImpl.Finish();
}

void cDisplay::welcomeScreen()
{
    FTImpl.WriteCmd(CMD_INFLATE);
    FTImpl.WriteCmd(WELCOME_SCREEN_ADDRESS);
    FTImpl.WriteCmdfromflash(welcome_screen, sizeof(welcome_screen));

    FTImpl.DLStart();

    FTImpl.Cmd_Gradient(240, 79, 0xFEFFFF, 240, 240, 0xA5A5A5);

    FTImpl.BitmapHandle(0);
    FTImpl.BitmapSource(WELCOME_SCREEN_ADDRESS);//?
    FTImpl.BitmapLayout(FT_ARGB1555,960,272);
    FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 480, 272);

    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2ii(0, 0, 0, 0);
    FTImpl.End();

    FTImpl.DLEnd();
    FTImpl.Finish();
}

void cDisplay::uploadArrows()
{
    FTImpl.WriteCmd(CMD_INFLATE);
    FTImpl.WriteCmd(ARROW_LEFT_ADDRESS);
    FTImpl.WriteCmdfromflash(arrow_left, sizeof(arrow_left));

    FTImpl.WriteCmd(CMD_INFLATE);
    FTImpl.WriteCmd(ARROW_RIGHT_ADDRESS);
    FTImpl.WriteCmdfromflash(arrow_right, sizeof(arrow_right));
}

void cDisplay::showSpinner()
{
    FTImpl.DLStart();
    FTImpl.Clear(1, 1, 1);
    FTImpl.Cmd_Spinner(FT_DISPLAYWIDTH/2, FT_DISPLAYHEIGHT/2, 0, 1);

    FTImpl.DLEnd();
    FTImpl.Finish();
}

void cDisplay::initializeInterface()
{
    bool ok;
    QString answer;

    if (isTcpIpProtocol()){
        cuTcpSocketIOInterface* interface = new cuTcpSocketIOInterface(this);

        interface->setAddress(convertToHostAddress(tcpIpAddress()));
        interface->setPort(SERVER_TCPIP_PORT);

        answer = interface->tcpIpQuery("SYST:DEVL?\r\n", 500, &ok);
        mInterface = interface;

    }
    else {
        cuRs485IOInterface* interface = new cuRs485IOInterface(this);
        interface->setPortName(rs485PortName());

        QSettings settings("Scontel", "RaspPi Server");
        int size = settings.beginReadArray("devices");

        answer.append(QString("DevCount: %1\r\n").arg(size));

        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);

            answer.append(";<br>");
            answer.append(QString("Dev%1: address=%2: type=%3\r\n")
                          .arg(i)
                          .arg(settings.value("devAddress", 255).toInt())
                          .arg(settings.value("devType","None").toString()));
        }

        settings.endArray();
        mInterface = interface;
    }

    createUi(answer);
}

void cDisplay::createUi(QString deviceList)
{
    QStringList list = deviceList.split("<br>");
    // составляем список всех устройств, каждый из них инициализируем  и т. д.
    foreach (QString str, list) {
        QStringList lList = str.split(':');
        if (lList.size() == 3){ //должно быть описание устройства
            //первое значение DevX - X порядковый номер
            int add = lList[1].split('=')[1].toInt();
            QString type = lList[2].split('=')[1];
            if (type.contains("CU4SD")){
                //данное устройство - SspdDriver
                mDrivers.append(new SspdDriverM0(this));
            }
            else if (type.contains("CU4TD"))
                //данное устройство - TempDriver
                mDrivers.append(new TempDriverM0(this));
            int i = mDrivers.size()-1;
            mDrivers[i]->setDevAddress(static_cast<quint8>(add));
            mDrivers[i]->setIOInterface(mInterface);
            mDrivers[i]->deviceType()->getValueSync(nullptr, 5);
        }
    }
    nextPrevModule(true);
}

void cDisplay::show_TempWidget()
{
    auto *tmpDriver = static_cast<TempDriverM0*>(mDrivers[mCurrentIndex]);

    showTitle("Temperature",tmpDriver->devAddress());

    FTImpl.ColorRGB(86,86,86);
    FTImpl.LineWidth(60);
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(2080,1840);
    FTImpl.Vertex2f(5632,3312);
    FTImpl.End();
    FTImpl.ColorRGB(0,0,0);
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(2080,1840);
    FTImpl.Vertex2f(5600,3280);
    FTImpl.End();

    FTImpl.ColorRGB(255, 255, 255);
    if (qAbs(static_cast<double>(tmpDriver->temperature()->currentValue())) > 1e-5)
        FTImpl.Cmd_Text(240,160,31, 1536, QString("T: %1 K").
                        arg(static_cast<double>(tmpDriver->temperature()->currentValue()), 4, 'f', 2).
                        toLatin1().data());
    else
        FTImpl.Cmd_Text(240,160,31, 1536, QString("N/C").toLatin1().data());


    FTImpl.Cmd_FGColor(0x2b2b2b);
    FTImpl.Cmd_GradColor(0xffffff);

    showPreviouseButton(mButtonTag == BT_Prev);
    showNextButton(mButtonTag == BT_Next);

}

void cDisplay::show_SspdWidget()
{
    auto *tmpDriver = static_cast<SspdDriverM0*>(mDrivers[mCurrentIndex]);
    auto data = tmpDriver->data()->currentValue();

    FTImpl.TagMask(1);
    FTImpl.Tag(BT_SspdData);
    FTImpl.Cmd_Button(11,75, 214,115, 31,0, "Button");
    FTImpl.Tag(BT_SspdCmp);
    FTImpl.Cmd_Button(9,201, 221,64, 31,0, "Button");
    FTImpl.TagMask(0);

    showTitle("SSPD", tmpDriver->devAddress());

    FTImpl.Cmd_FGColor(0x2b2b2b);
    FTImpl.Cmd_GradColor(0xffffff);

    showPreviouseButton(mButtonTag == BT_Prev);
    showNextButton(mButtonTag == BT_Next);

    bool tmpBool = data.Status.stShorted;
    FTImpl.TagMask(1);
    FTImpl.Tag(BT_Short);
    FTImpl.ColorRGB(255,255,255);
    FTImpl.Cmd_FGColor(0x525252);
    FTImpl.Cmd_BGColor(tmpBool ? 0x525252 : 0x009000);
    FTImpl.Cmd_Toggle(264,88,58,28,0,tmpBool ? 65535 : 0 ,"Open""\xFF""Short");

    FTImpl.Tag(BT_Amp);
    tmpBool = !data.Status.stAmplifierOn;
    FTImpl.Cmd_BGColor(tmpBool ? 0x525252 : 0x009000);
    FTImpl.Cmd_Toggle(389,88,59,28,0,tmpBool ? 65535 : 0 ,"Amp""\xFF""Amp");
    FTImpl.TagMask(0);

    FTImpl.ColorRGB(82,82,82);
    FTImpl.LineWidth(60);
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(144,1200);
    FTImpl.Vertex2f(3696,3072);
    FTImpl.End();

    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(144,3216);
    FTImpl.Vertex2f(3696,4240);
    FTImpl.End();


    FTImpl.ColorRGB(0,mSspdData ? 0x90 : 0, 0);
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(144,1200);
    FTImpl.Vertex2f(3664,3040);
    FTImpl.End();

    FTImpl.ColorRGB(0,!mSspdData ? 0x90 : 0, 0);
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(144,3216);
    FTImpl.Vertex2f(3664,4208);
    FTImpl.End();


    FTImpl.ColorRGB(255, 255, 255);
    FTImpl.Cmd_Text(18, 89, 30, 0, "I:");
    FTImpl.Cmd_Text(176, 89, 30, 0, "uA");
    FTImpl.Cmd_Text(64, 89, 30, 0, QString("%1").arg(static_cast<double>(data.Current) * 1e6, 6,'f', 1).toLatin1().data());

    FTImpl.Cmd_Text(18, 144, 30, 0, "U:");
    FTImpl.Cmd_Text(176,144, 30, 0, "mV");
    FTImpl.Cmd_Text(64, 144, 30, 0, QString("%1").arg(static_cast<double>(data.Voltage) * 1e3, 6,'f', 1).toLatin1().data());

    tmpBool = !data.Status.stComparatorOn;
    FTImpl.Cmd_FGColor(0x525252);
    FTImpl.Cmd_BGColor(tmpBool ? 0x525252 : 0x009000);

    FTImpl.TagMask(1);
    FTImpl.Tag(BT_Cmp);
    FTImpl.Cmd_Toggle(30,222,56,29,0,tmpBool ? 65535 : 0,"Cmp""\xFF""Cmp");
    FTImpl.TagMask(0);

    FTImpl.Cmd_Text(189,231,29, 1024, "mV");
    double tmpF = static_cast<double>(tmpDriver->params()->currentValue().Cmp_Ref_Level) * 1e3;
    FTImpl.Cmd_Text(179,231,29, 3072, QString("%1").arg(tmpF, 3,'f', 0).toLatin1().data());

    FTImpl.Cmd_FGColor(0x2b2b2b);
    FTImpl.Cmd_GradColor(0xffffff);
    FTImpl.TagMask(1);
    FTImpl.Tag(BT_Plus);
    FTImpl.Cmd_Button(251,128, 65,60, 31, mButtonTag == BT_Plus ? FT_OPT_FLAT : 0, "+");
    FTImpl.Tag(BT_Minus);
    FTImpl.Cmd_Button(252,200, 65,60, 31, mButtonTag == BT_Minus ? FT_OPT_FLAT : 0, "-");

    FTImpl.Tag(BT_1);
    if (mSspdButtonIndex == 1) FTImpl.ColorRGB(0, 0xe0, 0);
    else FTImpl.ColorRGB(255, 255, 255);
    FTImpl.Cmd_Button(329,199, 65,60, 29, mButtonTag == BT_1 ? FT_OPT_FLAT : 0, "x1");

    FTImpl.Tag(BT_10);
    if (mSspdButtonIndex == 2) FTImpl.ColorRGB(0, 0xe0, 0);
    else FTImpl.ColorRGB(255, 255, 255);
    FTImpl.Cmd_Button(405,129, 65,60, 29, mButtonTag == BT_10 ? FT_OPT_FLAT : 0, "x10");

    if (mSspdData)
        if (mSspdButtonIndex == 0) FTImpl.ColorRGB(0, 0xe0, 0);
        else FTImpl.ColorRGB(255, 255, 255);
    else
        FTImpl.ColorRGB(89,89,89);
    FTImpl.Tag(BT_0_1);
    FTImpl.Cmd_Button(328,128, 65,60, 29, (mButtonTag == BT_0_1) && (mSspdData) ? FT_OPT_FLAT : 0, "x0.1");

    if (!mSspdData)
        if (mSspdButtonIndex == 3) FTImpl.ColorRGB(0, 0xe0, 0);
        else FTImpl.ColorRGB(255, 255, 255);
    else
        FTImpl.ColorRGB(89,89,89);
    FTImpl.Tag(BT_100);
    FTImpl.Cmd_Button(406,198, 65,60, 29, (mButtonTag == BT_100) && (!mSspdData) ? FT_OPT_FLAT : 0, "x100");
    FTImpl.TagMask(0);
}

void cDisplay::doFinish()
{
    qDebug()<<"Finish start!";
    FTImpl.DLStart();

    FTImpl.ClearColorRGB(0,0,0);
    FTImpl.Clear(1,1,1);

    FTImpl.Cmd_Gradient(464,73, 0x3e3e3e, 464,283, 0x000000);
    FTImpl.LineWidth(16);

    FTImpl.ColorRGB(255,255,255);
    FTImpl.Cmd_Text(240, 34, 29, FT_OPT_CENTER, "PC will be turned off or rebooted.");
    FTImpl.Cmd_Text(240, 62, 29, FT_OPT_CENTER, "Please wait until operation is done.");
    FTImpl.Cmd_Spinner(240, 164, 0, 1);

    FTImpl.Cmd_Swap();

    FTImpl.DLEnd();
    FTImpl.Finish();
    mFinished = true;
    qDebug()<<"Done!!!! Good bye!";
}

void cDisplay::nextPrevModule(bool next)
{
    if (mDrivers.size() == 0) return;
    if (mCurrentIndex!=-1){
        //если термометр то выключить
        if (mDrivers[mCurrentIndex]->deviceType()->currentValue().contains("CU4TD")){
            static_cast<TempDriverM0*>(mDrivers[mCurrentIndex])->commutator()->setValueSync(false, nullptr, 5);
        }

    }
    mCurrentIndex = (mCurrentIndex + (next ? (1): mDrivers.size()-1)) % mDrivers.size();

    //если термометр то включить
    if (mDrivers[mCurrentIndex]->deviceType()->currentValue().contains("CU4TD")){
        static_cast<TempDriverM0*>(mDrivers[mCurrentIndex])->commutator()->setValueSync(true, nullptr, 5);
    }
    mInited = false;
    showSpinner();
}

void cDisplay::showPreviouseButton(bool pressed)
{
    FTImpl.TagMask(1);
    FTImpl.Tag(BT_Prev);
    FTImpl.Cmd_Button(2, 2, 85, 60, 31, pressed ? FT_OPT_FLAT : 0, "");
    FTImpl.TagMask(0);

    FTImpl.BitmapHandle(0);
    FTImpl.BitmapSource(ARROW_LEFT_ADDRESS);
    FTImpl.BitmapLayout(FT_ARGB1555,128,64);
    FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 64, 64);
    FTImpl.Cmd_LoadIdentity();
    FTImpl.Cmd_Scale(65536, 65536);
    FTImpl.Cmd_SetMatrix();
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2f(176,0);
    FTImpl.End();
}

void cDisplay::showNextButton(bool pressed)
{
    FTImpl.TagMask(1);
    FTImpl.Tag(BT_Next);
    FTImpl.Cmd_Button(391,2, 85,60, 31, pressed ? FT_OPT_FLAT : 0, "");
    FTImpl.TagMask(0);

    FTImpl.BitmapHandle(0);
    FTImpl.BitmapSource(ARROW_RIGHT_ADDRESS);
    FTImpl.BitmapLayout(FT_ARGB1555,128,64);
    FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 64, 64);
    FTImpl.Cmd_LoadIdentity();
    FTImpl.Cmd_Scale(65536, 65536);
    FTImpl.Cmd_SetMatrix();
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2f(6480,0);
    FTImpl.End();
}

void cDisplay::showTitle(QString title, int address)
{
    FTImpl.ClearColorRGB(0,0,0);
    FTImpl.Clear(1,1,1);
    FTImpl.Cmd_Gradient(464,73, 0x3e3e3e, 464,283, 0x000000);
    FTImpl.LineWidth(16);
    FTImpl.ColorRGB(130,130,130);
    FTImpl.Begin(FT_LINES);
    FTImpl.Vertex2f(0,1088);
    FTImpl.Vertex2f(7680,1088);
    FTImpl.End();
    FTImpl.ColorRGB(0,0,0);
    FTImpl.Begin(FT_LINES);
    FTImpl.Vertex2f(0,1056);
    FTImpl.Vertex2f(7680,1056);
    FTImpl.End();

    FTImpl.ColorRGB(255, 255, 255);
    FTImpl.Cmd_Text(240,24,31, 1536, title.toLatin1().data());
    FTImpl.Cmd_Text(240,57,26, 1536, QString("Address: %1").arg(address).toLatin1().data());
}

void cDisplay::buttonProcess()
{
    if ((!mLastButtonTag) && (mButtonTag)){
        // кнопка нажата
        displayUpdate();
    }

    if ((mLastButtonTag) && (!mButtonTag)){
        // кнопка отжата
        switch (mLastButtonTag) {
        case BT_Next:
        case BT_Prev:{
            nextPrevModule(mLastButtonTag == BT_Next);
            break;
        }
        default:{
            QString str = mDrivers[mCurrentIndex]->deviceType()->currentValue();
            if (str.contains("CU4SD"))
                sspdButtonProcess();
            break;
        }
        }
    }
}

void cDisplay::sspdButtonProcess()
{
    auto *tmpDriver = static_cast<SspdDriverM0*>(mDrivers[mCurrentIndex]);
    tmpDriver->setTimeOut(100);
    auto data = tmpDriver->data()->currentValue();
    bool ok = true;
    switch (mLastButtonTag) {

    case BT_Short:{
        tmpDriver->shortEnable()->setValueSync(!data.Status.stShorted, nullptr, 5);
        tmpDriver->data()->getValueSync(&ok, 5);
        break;
    }
    case BT_Amp:{
        tmpDriver->amplifierEnable()->setValueSync(!data.Status.stAmplifierOn, nullptr, 5);
        tmpDriver->data()->getValueSync(&ok, 5);
        break;
    }
    case BT_Cmp:{
        data.Status.stComparatorOn = data.Status.stRfKeyToCmp = data.Status.stCounterOn = !data.Status.stComparatorOn;
        tmpDriver->status()->setValueSync(data.Status, nullptr, 5);
        tmpDriver->data()->getValueSync(&ok, 5);
        break;
    }
    case BT_SspdData:
        mSspdData = true;
        mSspdButtonIndex = 0;
        break;
    case BT_SspdCmp:
        mSspdData = false;
        mSspdButtonIndex = 1;
        break;
    case BT_0_1:
        if (mSspdData) mSspdButtonIndex = 0;
        break;
    case BT_100:
        if (!mSspdData) mSspdButtonIndex = 3;
        break;
    case BT_1:
    case BT_10:
        mSspdButtonIndex = mLastButtonTag - BT_0_1;
        break;
    case BT_Plus:{
        if (mSspdData){
            float tmp = tmpDriver->current()->currentValue();
            tmp += static_cast<float>(1E-7 * pow(10, mSspdButtonIndex));
            tmpDriver->current()->setValueSync(tmp, nullptr, 5);
            tmpDriver->current()->setCurrentValue(tmp);
        }
        else {
            float tmp = tmpDriver->cmpReferenceLevel()->currentValue();
            tmp += static_cast<float>(1e-4 * pow(10, mSspdButtonIndex));
            tmpDriver->cmpReferenceLevel()->setValueSync(tmp, nullptr, 5);
            tmpDriver->cmpReferenceLevel()->setCurrentValue(tmp);
        }
        break;
    }
    case BT_Minus:{
        if (mSspdData){
            float tmp = tmpDriver->current()->currentValue();
            tmp -= static_cast<float>(1E-7 * pow(10, mSspdButtonIndex));
            tmpDriver->current()->setValueSync(tmp, nullptr, 5);
            tmpDriver->current()->setCurrentValue(tmp);
        }
        else {
            float tmp = tmpDriver->cmpReferenceLevel()->currentValue();
            tmp -= static_cast<float>(1e-4 * pow(10,mSspdButtonIndex));
            tmpDriver->cmpReferenceLevel()->setValueSync(tmp, nullptr, 5);
            tmpDriver->cmpReferenceLevel()->setCurrentValue(tmp);
        }
        break;
    }
    default:
        ok = false;
        break;
    }
    if (ok)
        displayUpdate();
}

void cDisplay::displayUpdate()
{
    FTImpl.DLStart();

    QString str = mDrivers[mCurrentIndex]->deviceType()->currentValue();
    if (str.contains("CU4TD")){
        show_TempWidget();
    }
    if (str.contains("CU4SD")){
        show_SspdWidget();
    }

    FTImpl.Cmd_Swap();
    FTImpl.DLEnd();
    FTImpl.Finish();
}

void cDisplay::wakeUp()
{
    mPowerMode = pmNormal;
    qDebug()<<"wakeUp";
    FTImpl.Write(REG_PWM_DUTY, 128);
    mWDGTimer->start(WatchDogTimeOut);
}

void cDisplay::workerTimeOut()
{
    if (mFinishAll)
        return;

    if ((mCurrentIndex<0) || (mCurrentIndex>=mDrivers.size())) return;

    if (mPowerMode == pmNormal) {

        bool ok = true;
        QString str = mDrivers[mCurrentIndex]->deviceType()->currentValue();
        mDrivers[mCurrentIndex]->setTimeOut(100);
        if (str.contains("CU4TD")){
            auto* mDriver = static_cast<TempDriverM0*>(mDrivers[mCurrentIndex]);
            auto data = mDriver->data()->getValueSync(&ok, 5);
            if ((ok) && (!data.CommutatorOn))
                mDriver->commutator()->setValueSync(true, nullptr, 5);
        }
        if (str.contains("CU4SD")){
            static_cast<SspdDriverM0*>(mDrivers[mCurrentIndex])->data()->getValueSync(&ok, 5);
            if (ok)
                static_cast<SspdDriverM0*>(mDrivers[mCurrentIndex])->params()->getValueSync(&ok, 5);
        }

        if (ok)
            displayUpdate();
    }

    mInited = true;
    mWorkerTimer->start(WorkerTimeOut);
}

void cDisplay::displayTimeOut()
{
    if (mFinishAll) {
        doFinish();
        return;
    }

    if (mInited){

        if (mPowerMode == pmPreSleep){
            if (FTImpl.Read32(REG_TOUCH_SCREEN_XY) != 0x80008000)
                wakeUp();
        }
        else {
            mButtonTag = FTImpl.Read(REG_TOUCH_TAG);

            if ((mButtonTag!=mLastButtonTag)){

                buttonProcess();
                mLastButtonTag=mButtonTag;
                mPowerMode = pmNormal;
                mWDGTimer->start(WatchDogTimeOut);
            }
        }
    }

    if (mFinishAll) {
        doFinish();
        return;
    }

    mDisplayTimer->start(mPowerMode == pmNormal ? WorkDisplayTimeOut : SleepModeDisplayTimeOut);
}

void cDisplay::watchDogTimeOut()
{
    mPowerMode = pmPreSleep;
    qDebug()<<"watch dog timer";

    FTImpl.Write16(REG_PWM_HZ, 2000);

    const int n = 20;
    for (int i = 0; i < n; i++){
        FTImpl.Write32(REG_PWM_DUTY, 128-(i+1)*128/n);
        QThread::msleep(100);
    }

}

