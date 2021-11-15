#include "appcore.h"
#include <QSettings>
#include <QGuiApplication>
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>
#include "devicelist.h"
#include <QRandomGenerator>
#include "unitdata.h"
#include "Interfaces/cutcpsocketiointerface.h"
#include "../qCustomLib/qCustomLib.h"
#include "servercommands.h"

#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
#include "Drivers/heaterdriverm0.h"

AppCore::AppCore(QObject *parent)
    : QObject(parent)
    , mLastIpAddress("127.0.0.1")
    , mDevList(nullptr)
    , mUnitData(nullptr)
    , mInterface(new cuTcpSocketIOInterface(this))
    , mDriver(nullptr)
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
                if (tmpStr.contains("CU4SDM0")) driverType = "SSPD Driver M0";
                if (tmpStr.contains("CU4SDM1")) driverType = "SSPD Driver M1";
                if (tmpStr.contains("CU4TDM0")) driverType = "Temperature M0";
                if (tmpStr.contains("CU4TDM1")) driverType = "Temperature M1";
                if (tmpStr.contains("CU4HTM"))  driverType = "Heater";
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

void AppCore::updateDriverData(quint8 address, QString type)
{
    assert(mUnitData);

    if (type == "SSPD Driver M0"){

        auto *driver = qobject_cast<SspdDriverM0*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new SspdDriverM0(this);
            mDriver = driver;
        }

        driver->setDevAddress(address);
        mUnitData->setCurrentAddress(address);
        driver->setIOInterface(mInterface);

        //реализация получения новых данных
        bool ok = false;
        auto data = driver->data()->getValueSync(&ok, 5);


        if (!ok) return;

        mUnitData->setData("current",       static_cast<double>(data.Current) * 1e6);
        mUnitData->setData("voltage",       static_cast<double>(data.Voltage) * 1e3);
        mUnitData->setData("short",         static_cast<bool>(data.Status.stShorted));
        mUnitData->setData("amplifier",     static_cast<bool>(data.Status.stAmplifierOn));
        mUnitData->setData("cmp_on",        static_cast<bool>(data.Status.stComparatorOn &&
                                                              data.Status.stRfKeyToCmp &&
                                                              data.Status.stCounterOn));
        mUnitData->setData("counter",       static_cast<double>(data.Counts));
        mUnitData->setData("autoreset_on",  static_cast<bool>(data.Status.stAutoResetOn));
    }
    else if (type == "SSPD Driver M1"){
        auto *driver = qobject_cast<SspdDriverM1*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new SspdDriverM1(this);
            mDriver = driver;
        }
        driver->setDevAddress(address);
        mUnitData->setCurrentAddress(address);
        driver->setIOInterface(mInterface);
        //реализация получения новых данных
        bool ok = false;
        auto data = driver->data()->getValueSync(&ok, 5);
        if (!ok) return;

        mUnitData->setData("current",       static_cast<double>(data.Current) * 1e6);
        mUnitData->setData("current_mon",   static_cast<double>(data.CurrentMonitor) * 1e6);
        mUnitData->setData("voltage",       static_cast<double>(data.Voltage) * 1e3);
        mUnitData->setData("short",         static_cast<bool>(data.Status.stShorted));
        mUnitData->setData("amplifier",     static_cast<bool>(data.Status.stAmplifierOn));
        mUnitData->setData("counter_on",    static_cast<bool>(data.Status.stComparatorOn &&
                                                              data.Status.stCounterOn));
        mUnitData->setData("hf_on",         static_cast<bool>(data.Status.stHFModeOn));
        mUnitData->setData("counter",       static_cast<double>(data.Counts));
        mUnitData->setData("autoreset_on",  static_cast<bool>(data.Status.stAutoResetOn));
    }
    else if (type == "Temperature M0"){
        auto *driver = qobject_cast<TempDriverM0*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new TempDriverM0(this);
            mDriver = driver;
        }
        driver->setDevAddress(address);
        mUnitData->setCurrentAddress(address);
        driver->setIOInterface(mInterface);
        bool ok = false;
        auto data = driver->data()->getValueSync(&ok, 5);
        qDebug()<<ok;
        if (!ok) return;

        mUnitData->setData("temperature",   static_cast<double>(data.Temperature));
        mUnitData->setData("commutator",    static_cast<bool>(data.CommutatorOn));
        mUnitData->setData("voltage",       static_cast<double>(data.TempSensorVoltage));
    }
    else if (type == "Temperature M1"){
        auto *driver = qobject_cast<TempDriverM1*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new TempDriverM1(this);
            mDriver = driver;
        }
        driver->setDevAddress(address);
        mUnitData->setCurrentAddress(address);
        driver->setIOInterface(mInterface);
        bool ok = driver->updateTemperature();
        if (!ok) return;
        ok = driver->updateVoltage();
        if (!ok) return;
        auto relays = driver->relaysStatus()->getValueSync(&ok, 5);
        if (!ok) return;

        for (int i = 0; i<4; ++i){
            mUnitData->setData(QString("temperature_%1").arg(i+1),
                               static_cast<double>(driver->currentTemperature(i)));
            mUnitData->setData(QString("voltage_%1").arg(i+1),
                               static_cast<double>(driver->currentVoltage(i)));
        }

        mUnitData->setData("relay_5",   static_cast<bool>(relays.status() & cRelaysStatus::ri5V));
        mUnitData->setData("relay_25",  static_cast<bool>(relays.status() & cRelaysStatus::ri25V));
    }
    else if (type == "Heater"){
        HeaterDriverM0 * driver;
        if (mDriver != nullptr){
            driver = qobject_cast<HeaterDriverM0 *>(mDriver);
            if (!driver){
                mDriver->deleteLater();
                driver = new HeaterDriverM0(this);
            }
        }
        else driver = new HeaterDriverM0(this);
        mDriver = driver;

        driver->setDevAddress(address);
        mUnitData->setCurrentAddress(address);
        driver->setIOInterface(mInterface);
        bool ok = false;
        auto data = driver->eepromConst()->getValueSync(&ok, 5);
        qDebug()<<ok;
        if (!ok) return;

        mUnitData->setData("maxcurrent",   static_cast<double>(data.maximumCurrent));
        mUnitData->setData("frontedge",    static_cast<double>(data.frontEdgeTime));
        mUnitData->setData("holdtime",     static_cast<double>(data.holdTime));
        mUnitData->setData("rearedge",     static_cast<double>(data.rearEdgeTime));
    }
}

void AppCore::updateDriverParameters(quint8 address, QString type)
{
    assert(mUnitData);

    if (type == "SSPD Driver M0"){
        auto *driver = qobject_cast<SspdDriverM0*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new SspdDriverM0(this);
            mDriver = driver;
        }
        driver->setDevAddress(address);
        mUnitData->setCurrentAddress(address);
        driver->setIOInterface(mInterface);
        //реализация получения новых данных
        bool ok = false;
        auto sspdParams = driver->params()->getValueSync(&ok, 5);

        if (!ok) return;

        mUnitData->setData(mUnitData->getIndexByName("cmp"), static_cast<double>(sspdParams.Cmp_Ref_Level) * 1000.0);
        mUnitData->setData(mUnitData->getIndexByName("counter_timeOut"), static_cast<double>(sspdParams.Time_Const));
        mUnitData->setData(mUnitData->getIndexByName("threshold"), static_cast<double>(sspdParams.AutoResetThreshold));
        mUnitData->setData(mUnitData->getIndexByName("timeOut"), static_cast<double>(sspdParams.AutoResetTimeOut));
    }
    else if (type == "SSPD Driver M1"){
        auto *driver = qobject_cast<SspdDriverM1*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new SspdDriverM1(this);
            mDriver = driver;
        }
        driver->setDevAddress(address);
        mUnitData->setCurrentAddress(address);
        driver->setIOInterface(mInterface);
        //реализация получения новых данных
        bool ok = false;
        auto sspdParams = driver->params()->getValueSync(&ok, 5);

        if (!ok) return;

        mUnitData->setData(mUnitData->getIndexByName("cmp"), static_cast<double>(sspdParams.Cmp_Ref_Level) * 1000.0);
        mUnitData->setData(mUnitData->getIndexByName("counter_timeOut"), static_cast<double>(sspdParams.Time_Const));
        mUnitData->setData(mUnitData->getIndexByName("threshold"), static_cast<double>(sspdParams.AutoResetThreshold));
        mUnitData->setData(mUnitData->getIndexByName("timeOut"), static_cast<double>(sspdParams.AutoResetTimeOut));
    }
    else if (type == "Temperature M0"){

    }
    else if (type == "Temperature M1"){

    }
}

void AppCore::updateControlUnitsList()
{
    mControlUnitsList.clear();
    mControlUnitsList = availableControlUnits();
    emit controlUnitsListChanged();
}

void AppCore::prepareUnitData(quint8 address, QString type)
{
    qDebug()<<"prepareUnitData. Address: "<<address<<". Type: "<<type;
    assert(mUnitData);
    mUnitData->items()->clear();
    mUnitData->setCurrentType(type);
    mUnitData->setCurrentAddress(address);

    if (type == "SSPD Driver M0"){
        mUnitData->items()->append({"current", "Current (uA)", 0, "changable", 1, "Bias", 0.1});
        mUnitData->items()->append({"voltage", "Voltage (mV)", 0, "unchangable", 2, "Bias", 0});
        mUnitData->items()->append({"short", "Shorted", 0, "switch", 0, "Bias", 0});
        mUnitData->items()->append({"amplifier", "Amplifier", 1, "switch", 0, "Signal", 0});
        mUnitData->items()->append({"cmp", "Trigger (mV)", 0, "changable", 0, "Signal", 10});
        mUnitData->items()->append({"cmp_on", "Comparator", 0, "switch", 0, "Signal", 0});
        mUnitData->items()->append({"counter", "Counts", 0, "unchangable", 0, "Counter", 0});
        mUnitData->items()->append({"counter_timeOut", "TimeOut (sec)", 0, "changable", 2, "Counter", 0.1});
        mUnitData->items()->append({"autoreset_on", "Autoreset", 0, "switch", 0, "Autoreset", 0});
        mUnitData->items()->append({"threshold", "Threshold (V)", 0, "changable", 2, "Autoreset", 0.1});
        mUnitData->items()->append({"timeOut", "TimeOut (sec)", 0, "changable", 2, "Autoreset", 0.1});
    }
    else if (type == "SSPD Driver M1"){
        mUnitData->items()->append({"current", "Current (uA)", 0, "changable", 1, "Bias", 0.1});
        mUnitData->items()->append({"current_mon", "Current mon (uA)", 0, "unchangable", 1, "Bias", 0.1});
        mUnitData->items()->append({"voltage", "Voltage (mV)", 0, "unchangable", 2, "Bias", 0});
        mUnitData->items()->append({"short", "Shorted", 0, "switch", 0, "Bias", 0});
        mUnitData->items()->append({"amplifier", "Amplifier", 1, "switch", 0, "Signal", 0});
        mUnitData->items()->append({"cmp", "Trigger (mV)", 0, "changable", 0, "Signal", 10});
        mUnitData->items()->append({"counter_on", "Counter", 0, "switch", 0, "Signal", 0});
        mUnitData->items()->append({"hf_on", "HF Mode", 0, "switch", 0, "Signal", 0});
        mUnitData->items()->append({"counter", "Counts", 0, "unchangable", 0, "Counter", 0});
        mUnitData->items()->append({"counter_timeOut", "TimeOut (sec)", 0, "changable", 2, "Counter", 0.1});
        mUnitData->items()->append({"autoreset_on", "Autoreset", 0, "switch", 0, "Autoreset", 0});
        mUnitData->items()->append({"threshold", "Threshold (V)", 0, "changable", 2, "Autoreset", 0.1});
        mUnitData->items()->append({"timeOut", "TimeOut (sec)", 0, "changable", 2, "Autoreset", 0.1});
    }
    else if (type == "Temperature M0"){
        mUnitData->items()->append({"temperature", "Temperature (K)", 0, "unchangable", 1, "Temperature", 0});
        mUnitData->items()->append({"commutator", "Commutator", 0, "switch", 0, "Temperature", 0});
        mUnitData->items()->append({"voltage", "Voltage (V)", 0, "unchangable", 3, "Other", 0});
    }
    else if (type == "Temperature M1"){
        auto *driver = qobject_cast<TempDriverM1*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new TempDriverM1(this);
            mDriver = driver;
        }
        driver->setDevAddress(mUnitData->currentAddress());
        driver->setIOInterface(mInterface);

        driver->readDefaultParams();

        for (int i = 0; i < 4; ++i)
            if (driver->defaultParam(i).enable){
                mUnitData->items()->append({QString("temperature_%1").arg(i+1),
                                            QString("T%1 (K)").arg(i+1),
                                            0, "unchangable", 1, "Temperature", 0});
            }
        mUnitData->items()->append({"relay_5", "Relay (5V)", 0, "switch", 0, "Relays", 0});
        mUnitData->items()->append({"relay_25", "Relay (25V)", 0, "switch", 0, "Relays", 0});
        for (int i = 0; i < 4; ++i)
            if (driver->defaultParam(i).enable){
                mUnitData->items()->append({QString("voltage_%1").arg(i+1),
                                            QString("V%1 (K)").arg(i+1),
                                            0, "unchangable", 1, "Temperature", 0});
            }
    }

    else if (type == "Heater"){
        mUnitData->items()->append({"maxcurrent",   "Max Current (mA)", 0, "changable", 1, "Current",   0.1});
        mUnitData->items()->append({"frontedge",    "Front Edge (sec)", 0, "changable", 1, "Time",      0.1});
        mUnitData->items()->append({"holdtime",     "Hold Time (sec)" , 0, "changable", 1, "Time",      0.1});
        mUnitData->items()->append({"rearedge",     "Rear Edge (sec)" , 0, "changable", 1, "Time",      0.1});
        qDebug()<<"prepared";
    }

}

void AppCore::startHeating(quint8 address, QString type)
{
    assert(mUnitData);

    if (type != "Heater") return;

    auto * driver = qobject_cast<HeaterDriverM0 *>(mDriver);
    if (!driver) return;

    // получаем данные
    bool ok = false;
    auto data = driver->eepromConst()->getValueSync(&ok, 5);
    if (!ok) return;

    int index = mUnitData->getIndexByName("maxcurrent");
    data.maximumCurrent = mUnitData->items()->at(index).value;
    if (data.maximumCurrent < 0){
        data.maximumCurrent = 0;
        mUnitData->setData(index, 0.0);
    }
    qDebug()<<"current"<<data.maximumCurrent;

    index = mUnitData->getIndexByName("frontedge");
    data.frontEdgeTime = mUnitData->items()->at(index).value;
    if (data.frontEdgeTime < 0){
        data.frontEdgeTime = 0;
        mUnitData->setData(index, 0.0);
    }
    qDebug()<<"frontedge"<<data.frontEdgeTime;

    index = mUnitData->getIndexByName("holdtime");
    data.holdTime = mUnitData->items()->at(index).value;
    if (data.holdTime < 0){
        data.holdTime = 0;
        mUnitData->setData(index, 0.0);
    }
    qDebug()<<"holdtime"<<data.holdTime;

    index = mUnitData->getIndexByName("rearedge");
    data.rearEdgeTime = mUnitData->items()->at(index).value;
    if (data.rearEdgeTime < 0){
        data.rearEdgeTime = 0;
        mUnitData->setData(index, 0.0);
    }
    qDebug()<<"rearedge"<<data.rearEdgeTime;

    // отправляем Новые данные
    driver->eepromConst()->setValueSync(data, &ok, 5);
    if (!ok) return;

    driver->startHeating()->executeSync(&ok, 5);
}

void AppCore::emergencyStop(quint8 address, QString type)
{
    qDebug()<<"emergency stop";
    if (type != "Heater") return;

    auto * driver = qobject_cast<HeaterDriverM0 *>(mDriver);
    if (!driver) return;

    // получаем данные
    bool ok = false;
    driver->emergencyStop()->executeSync(&ok, 5);

}

void AppCore::setNewData(int dataListIndex, double value)
{
    assert(mUnitData);

    if (dataListIndex < 0 || dataListIndex >= mUnitData->items()->size())
        return;
    QString name = mUnitData->items()->at(dataListIndex).name;

    if (mUnitData->currentType() == "SSPD Driver M0"){
        auto *driver = qobject_cast<SspdDriverM0*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new SspdDriverM0(this);
            mDriver = driver;
        }

        driver->setDevAddress(mUnitData->currentAddress());
        driver->setIOInterface(mInterface);

        if (name == "current")
            driver->current()->setValueSync(static_cast<float>(value * 1e-6), nullptr, 5);

        if (name == "short")
            driver->shortEnable()->setValueSync(value > 0.01, nullptr, 5);

        if (name == "amplifier")
            driver->amplifierEnable()->setValueSync(value>0.01, nullptr, 5);

        if (name == "cmp_on"){
            driver->counterEnable()->setValueSync(value > 0.01, nullptr, 5);
            driver->rfKeyEnable()->setValueSync(value > 0.01, nullptr, 5);
            driver->cmpLatchEnable()->setValueSync(value > 0.01, nullptr, 5);
        }

        if (name == "autoreset_on")
            driver->autoResetEnable()->setValueSync(value>0.01, nullptr, 5);

        if (name == "cmp")
            driver->cmpReferenceLevel()->setValueSync(static_cast<float>(value/1000), nullptr, 5);

        if (name == "counter_timeOut")
            driver->timeConst()->setValueSync(static_cast<float>(value), nullptr, 5);
        if (name == "threshold")
            driver->autoResetThreshold()->setValueSync(static_cast<float>(value), nullptr, 5);
        if (name == "timeOut")
            driver->autoResetTimeOut()->setValueSync(static_cast<float>(value), nullptr, 5);
    }
    else if (mUnitData->currentType() == "SSPD Driver M1"){
        auto *driver = qobject_cast<SspdDriverM1*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new SspdDriverM1(this);
            mDriver = driver;
        }
        driver->setDevAddress(mUnitData->currentAddress());
        driver->setIOInterface(mInterface);

        if (name == "current")
            driver->current()->setValueSync(static_cast<float>(value * 1e-6), nullptr, 5);

        if (name == "short")
            driver->shortEnable()->setValueSync(value > 0.01, nullptr, 5);

        if (name == "amplifier")
            driver->amplifierEnable()->setValueSync(value>0.01, nullptr, 5);

        if (name == "counter_on"){
            driver->counterEnable()->setValueSync(value > 0.01, nullptr, 5);
            driver->cmpLatchEnable()->setValueSync(value > 0.01, nullptr, 5);
        }

        if (name == "hf_on")
            driver->highFrequencyModeEnable()->setValueSync(value>0.01, nullptr, 5);

        if (name == "autoreset_on")
            driver->autoResetEnable()->setValueSync(value>0.01, nullptr, 5);

        if (name == "cmp")
            driver->cmpReferenceLevel()->setValueSync(static_cast<float>(value/1000), nullptr, 5);

        if (name == "counter_timeOut")
            driver->timeConst()->setValueSync(static_cast<float>(value), nullptr, 5);
        if (name == "threshold")
            driver->autoResetThreshold()->setValueSync(static_cast<float>(value), nullptr, 5);
        if (name == "timeOut")
            driver->autoResetTimeOut()->setValueSync(static_cast<float>(value), nullptr, 5);
    }
    else if (mUnitData->currentType() == "Temperature M0"){
        auto *driver = qobject_cast<TempDriverM0*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new TempDriverM0(this);
            mDriver = driver;
        }
        driver->setDevAddress(mUnitData->currentAddress());
        driver->setIOInterface(mInterface);
        if (name == "commutator")
            driver->commutator()->setValueSync(value>0.01, nullptr, 5);
    }

    else if (mUnitData->currentType() == "Temperature M1"){
        auto *driver = qobject_cast<TempDriverM1*>(mDriver);
        if (!driver){
            mDriver->deleteLater();
            driver = new TempDriverM1(this);
            mDriver = driver;
        }
        driver->setDevAddress(mUnitData->currentAddress());
        driver->setIOInterface(mInterface);
        auto relayStatus = driver->relaysStatus()->currentValue();
        if (name == "relay_5"){
            relayStatus.setStatus(cRelaysStatus::ri5V, (relayStatus.status() & cRelaysStatus::ri5V) ? 0 : cRelaysStatus::ri5V);
            driver->relaysStatus()->setValueSync(relayStatus, nullptr, 5);
        }
        if (name == "relay_5"){
            relayStatus.setStatus(cRelaysStatus::ri25V, (relayStatus.status() & cRelaysStatus::ri25V) ? 0 : cRelaysStatus::ri25V);
            driver->relaysStatus()->setValueSync(relayStatus, nullptr, 5);
        }
    }
}

void AppCore::setControlUnits(const QStringList &controlUnitsList)
{
    qDebug()<<"setControlUnits";
    mControlUnitsList = controlUnitsList;
    emit controlUnitsListChanged();
}

UnitData *AppCore::getUnitData() const
{
    return mUnitData;
}

void AppCore::setUnitData(UnitData *unitData)
{
    if (mUnitData)
        mUnitData->disconnect(this);

    mUnitData = unitData;

    if (mUnitData)
        connect(mUnitData, SIGNAL(newDataSetted(int, double)),
                this, SLOT(setNewData(int, double)));
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
