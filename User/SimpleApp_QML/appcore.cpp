#include "appcore.h"
#include <QDebug>
#include <QGuiApplication>
#include "QElapsedTimer"
#include <QSettings>
#include "../qCustomLib/qCustomLib.h"
#include "Server/servercommands.h"

AppCore::AppCore(QObject *parent) :
    QObject(parent),
    mInterface(new cuTcpSocketIOInterface(this)),
    mTempDriver(new cCu4TdM0Driver(this)),
    mSspdDriver(new cCu4SdM0Driver(this))
{

}

void AppCore::setSspdCurrent(const float &value)
{
    last_SspdData.Current = value;
    mSspdDriver->current()->setValueSequence(value, nullptr, 5);
}

void AppCore::setSspdShorted(const bool &value)
{
    last_SspdData.Status.stShorted = value;
    mSspdDriver->setShortEnable(value);
}

void AppCore::setSspdAmplifierTurnedOn(const bool &value)
{
    last_SspdData.Status.stAmplifierOn = value;
    mSspdDriver->setAmpEnable(value);
}

void AppCore::setSspdComparatorTurnedOn(const bool &value)
{
    last_SspdData.Status.stComparatorOn =
            last_SspdData.Status.stRfKeyToCmp =
            last_SspdData.Status.stCounterOn = value;
    mSspdDriver->deviceStatus()->setValueSequence(last_SspdData.Status, nullptr, 5);

}

void AppCore::setSspdAutoResetTurnedOn(const bool &value)
{
    last_SspdData.Status.stAutoResetOn = value; // сделать сразе же установку в устройство
    mSspdDriver->setAutoResetEnable(value);
}

void AppCore::setSspdCmpRefLevel(const float &value)
{
    last_SspdParams.Cmp_Ref_Level = value;
    mSspdDriver->cmpReferenceLevel()->setValueSequence(value, nullptr, 5);
}

void AppCore::setSspdCounterTimeConst(const float &value)
{
    last_SspdParams.Time_Const = value;
    mSspdDriver->timeConst()->setValueSequence(value, nullptr, 5);
}

void AppCore::setSspdAutoResetThreshold(const float &value)
{
    last_SspdParams.AutoResetThreshold = value;
    mSspdDriver->autoResetThreshold()->setValueSequence(value, nullptr, 5);
}

void AppCore::setSspdAutoResetTimeOut(const float &value)
{
    last_SspdParams.AutoResetTimeOut = value;
    mSspdDriver->autoResetTimeOut()->setValueSequence(value, nullptr, 5);
}

void AppCore::coreConnectToDefaultIpAddress()
{
    QSettings settings("Scontel", "cu-simpleapp");
    QString tmpStr = settings.value("TcpIpAddress","127.000.000.001").toString();
    coreConnectToIpAddress(tmpStr);
}

void AppCore::coreConnectToIpAddress(const QString& ipAddress)
{
    mInterface->setAddress(convertToHostAddress(ipAddress));
    mInterface->setPort(SERVER_TCPIP_PORT);

    bool ok;
    qApp->processEvents();
    QString answer = mInterface->tcpIpQuery("SYST:DEVL?\r\n", 1000, &ok);
    if (ok){
        mCoreMessage = answer;
        emit connectionApply();
    }
    else{
        mCoreMessage = ipAddress;
        emit connectionReject();
    }
}

void AppCore::connectToDevice()
{

}

void AppCore::getTemperatureDriverData(quint8 address)
{
    mTempDriver->setDevAddress(address);
    mTempDriver->setIOInterface(mInterface);

    last_TempData = mTempDriver->deviceData()->getValueSequence(nullptr, 5);
    emit temperatureDataDone();
}

void AppCore::connectTemperatureSensor(quint8 address)
{
    mTempDriver->setDevAddress(address);
    mTempDriver->setIOInterface(mInterface);
    mTempDriver->commutatorOn()->setValueSequence(true, nullptr, 5);
}

void AppCore::disConnectTemperatureSensor(quint8 address)
{
    mTempDriver->setDevAddress(address);
    mTempDriver->setIOInterface(mInterface);
    mTempDriver->commutatorOn()->setValueSequence(false, nullptr, 5);
}

void AppCore::initializeSspdDriver(quint8 address)
{
    qDebug()<<"initializeSspdDriver";
    mSspdDriver->setDevAddress(address);
    mSspdDriver->setIOInterface(mInterface);
    qDebug()<<"getDeviceData";
    last_SspdData = mSspdDriver->deviceData()->getValueSequence(nullptr, 5);
    qDebug()<<"getDeviceParams";
    last_SspdParams = mSspdDriver->deviceParams()->getValueSequence(nullptr, 5);
    qDebug()<<"emit signal";
    emit sspdDriverInitialized();
}

void AppCore::getSspdData(quint8 address)
{
    mSspdDriver->setDevAddress(address);
    mSspdDriver->setIOInterface(mInterface);
    last_SspdData = mSspdDriver->deviceData()->getValueSequence(nullptr, 5);
    emit sspdDataUpdated();
}

