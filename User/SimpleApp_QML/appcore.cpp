#include "appcore.h"
#include <QSettings>
#include <QGuiApplication>
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>
#include "devicelist.h"
#include "temperaturedata.h"
#include <QRandomGenerator>
#include "sspddata.h"
#include "Interfaces/cutcpsocketiointerface.h"
#include "../qCustomLib/qCustomLib.h"
#include "servercommands.h"

AppCore::AppCore(QObject *parent)
    : QObject(parent)
    , mLastIpAddress("127.0.0.1")
    , mDevList(nullptr)
    , mTempData(nullptr)
    , mSspdData(nullptr)
    , mInterface(new cuTcpSocketIOInterface(this))
    , mTempDriver(new TempDriverM0(this))
    , mSspdDriver(new SspdDriverM0(this))
{
    mControlUnitsList.append("test1");
    mControlUnitsList.append("test2");
    qDebug()<<mControlUnitsList;
}

static bool sReconnectEnableFlag;

bool AppCore::reconnectEnable()
{
    return sReconnectEnableFlag;
}

void AppCore::setReconnectEnable(bool reconnectEnable)
{
    sReconnectEnableFlag = reconnectEnable;
}

void AppCore::coreConnectToDefaultIpAddress()
{
    QSettings settings("Scontel", "cu-simpleapp");
    QString tmpStr = settings.value("TcpIpAddress","127.000.000.001").toString();
    coreConnectToIpAddress(tmpStr);
}

void AppCore::coreConnectToIpAddress(const QString& ipAddress)
{
    qDebug()<<"Core: connectToIpAddress"<<ipAddress;
    if (mLastIpAddress != ipAddress){
        mLastIpAddress = ipAddress;
        emit lastIpAddressChanged();
    }

    //подготовка интерфейса передачи данных
    mInterface->setAddress(convertToHostAddress(ipAddress));
    mInterface->setPort(SERVER_TCPIP_PORT);

    bool ok = true;
    qApp->processEvents();
    //Получение списка доступных устройств
    QString answer = mInterface->tcpIpQuery("SYST:DEVL?\r\n", 1000, &ok);
    if (ok){
        QSettings settings("Scontel", "cu-simpleapp");
        settings.setValue("TcpIpAddress", ipAddress);

        //записываем список устройств
        if (!mDevList)
            emit connectionReject();
        else {
            mDevList->removeAll();
            //ToDo: Разбор списка устройств
            answer.replace(QRegExp("\r?\n|\r"), "");
            QStringList strL = answer.split(";<br>");
            for (QString tmpStr : strL){
                QString driverType = "undefined";
                if (tmpStr.indexOf("CU4SD")> -1) driverType = "SSPD Driver";
                if (tmpStr.indexOf("CU4TD")> -1) driverType = "Temperature";
                int driverAddress = -1;
                QRegExp reg("address=[0-9]{1,2}:");

                int pos = reg.indexIn(tmpStr);
                if (pos>-1)
                    driverAddress = reg.cap().replace(QRegExp("address="), "").replace(":","").toInt();

                if (driverType != "undefined" && driverAddress>-1)
                    mDevList->appendItem(DeviceItem{driverType, driverAddress});
            }
            emit connectionApply();
        }
    }
    else {
        emit connectionReject();
    }
}

void AppCore::getTemperatureDriverData(quint8 address)
{
    mTempDriver->setDevAddress(address);
    mTempDriver->setIOInterface(mInterface);

    //Получаем данные и отсылаем их назад
    bool ok = false;
    auto data = mTempDriver->data()->getValueSync(&ok, 5);
    if (!mTempData || !ok)
        return;
    mTempData->setTemperature(static_cast<double>(data.Temperature));
    mTempData->setTemperatureSensorVoltage(static_cast<double>(data.TempSensorVoltage));
    mTempData->setPressure(static_cast<double>(data.Pressure));
    mTempData->setPressureSensorVoltage(static_cast<double>(data.PressSensorVoltageP - data.PressSensorVoltageN));
    mTempData->setConnected(data.CommutatorOn);
}

void AppCore::connectTemperatureSensor(quint8 address, bool state)
{
    mTempDriver->setDevAddress(address);
    mTempDriver->setIOInterface(mInterface);
    //connect - disconnect
    bool ok;
    mTempDriver->commutator()->setValueSync(state, &ok, 5);
    if (!mTempData || !ok)
        return;
    mTempData->setConnected(state);
}

void AppCore::getSspdDriverData(quint8 address)
{
    mSspdDriver->setDevAddress(address);
    setCurrentAddress(address);
    mSspdDriver->setIOInterface(mInterface);
    //реализация получения новых данных
    bool ok = false;
    auto sspdData = mSspdDriver->data()->getValueSync(&ok, 5);

    if (!mSspdData || !ok)
        return;

    mSspdData->setData(mSspdData->getIndexByName("current"), static_cast<double>(sspdData.Current) * 1e6);
    mSspdData->setData(mSspdData->getIndexByName("voltage"), static_cast<double>(sspdData.Voltage) * 1e3);
    mSspdData->setData(mSspdData->getIndexByName("short"), static_cast<bool>(sspdData.Status.stShorted));
    mSspdData->setData(mSspdData->getIndexByName("amplifier"), static_cast<bool>(sspdData.Status.stAmplifierOn));
    mSspdData->setData(mSspdData->getIndexByName("cmp_on"),static_cast<bool>(sspdData.Status.stComparatorOn &&
                                                                             sspdData.Status.stRfKeyToCmp &&
                                                                             sspdData.Status.stCounterOn));
    mSspdData->setData(mSspdData->getIndexByName("counter"), static_cast<double>(sspdData.Counts));
    mSspdData->setData(mSspdData->getIndexByName("autoreset_on"),static_cast<bool>(sspdData.Status.stAutoResetOn));
}

void AppCore::getSspdDriverParameters(quint8 address)
{
    mSspdDriver->setDevAddress(address);
    setCurrentAddress(address);
    mSspdDriver->setIOInterface(mInterface);
    //реализация получения новых данных
    bool ok = false;
    auto sspdParams = mSspdDriver->params()->getValueSync(&ok, 5);

    if (!mSspdData || !ok)
        return;

    mSspdData->setData(mSspdData->getIndexByName("cmp"), static_cast<double>(sspdParams.Cmp_Ref_Level) * 1000.0);
    mSspdData->setData(mSspdData->getIndexByName("counter_timeOut"), static_cast<double>(sspdParams.Time_Const));
    mSspdData->setData(mSspdData->getIndexByName("threshold"), static_cast<double>(sspdParams.AutoResetThreshold));
    mSspdData->setData(mSspdData->getIndexByName("timeOut"), static_cast<double>(sspdParams.AutoResetTimeOut));
}

void AppCore::updateControlUnitsList()
{
    mControlUnitsList.clear();
    mControlUnitsList = availableControlUnits();
    emit controlUnitsListChanged();
}

void AppCore::setNewData(int dataListIndex, double value)
{
    if (!mSspdData || dataListIndex < 0 || dataListIndex >= mSspdData->items().size())
        return;
    QString name = mSspdData->items()[dataListIndex].name;

    // будем считать что драйвер уже готов, иначе то как
    if (name == "current")
        mSspdDriver->current()->setValueSync(static_cast<float>(value * 1e-6), nullptr, 5);

    if (name == "short")
        mSspdDriver->shortEnable()->setValueSync(value > 0.01, nullptr, 5);

    if (name == "amplifier")
        mSspdDriver->amplifierEnable()->setValueSync(value>0.01, nullptr, 5);

    if (name == "cmp_on"){
        auto status = mSspdDriver->status()->currentValue();
        status.stCounterOn =
                status.stRfKeyToCmp =
                status.stCounterOn = value < 0.01 ? 0 : 1;
        mSspdDriver->status()->setValueSync(status, nullptr, 5);
    }

    if (name == "autoreset_on")
        mSspdDriver->autoResetEnable()->setValueSync(value>0.01, nullptr, 5);

    if (name == "cmp")
        mSspdDriver->cmpReferenceLevel()->setValueSync(static_cast<float>(value/1000), nullptr, 5);

    if (name == "counter_timeOut")
        mSspdDriver->timeConst()->setValueSync(static_cast<float>(value), nullptr, 5);
    if (name == "threshold")
        mSspdDriver->autoResetThreshold()->setValueSync(static_cast<float>(value), nullptr, 5);
    if (name == "timeOut")
        mSspdDriver->autoResetTimeOut()->setValueSync(static_cast<float>(value), nullptr, 5);
}

void AppCore::setControlUnits(const QStringList &controlUnitsList)
{
    qDebug()<<"setControlUnits";
    mControlUnitsList = controlUnitsList;
    emit controlUnitsListChanged();
}

int AppCore::getCurrentAddress() const
{
    return mCurrentAddress;
}

void AppCore::setCurrentAddress(int currentAddress)
{
    mCurrentAddress = currentAddress;
}

SspdData *AppCore::getSspdData() const
{
    return mSspdData;
}

void AppCore::setSspdData(SspdData *sspdData)
{
    if (mSspdData)
        mSspdData->disconnect(this);

    mSspdData = sspdData;

    if (mSspdData)
        connect(mSspdData, SIGNAL(newDataSetted(int, double)),
                this, SLOT(setNewData(int, double)));
}

TemperatureData *AppCore::getTempData() const
{
    return mTempData;
}

void AppCore::setTempData(TemperatureData *tempData)
{
    mTempData = tempData;
}

DeviceList *AppCore::devList() const
{
    return mDevList;
}

void AppCore::setDevList(DeviceList *devList)
{
    mDevList = devList;
}

QStringList AppCore::controlUnits() const
{
    return mControlUnitsList;
}

QString AppCore::lastIpAddress() const
{
    return mLastIpAddress;
}


