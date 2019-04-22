#include "adriver.h"
#include <QCoreApplication>
#include "star_prc_commands.h"

using namespace cmd;

AbstractDriver::AbstractDriver(QObject *parent):
    cuIODeviceImpl(parent),
    mTimer(new QTimer(this)),
    mDeviceType(new cuDeviceParam<QString>(this, G_GetDeviceType)),
    mModificationVersion(new cuDeviceParam<QString>(this, G_GetModVersion)),
    mHardwareVersion(new cuDeviceParam<QString>(this, G_GetHwVersion)),
    mFirmwareVersion(new cuDeviceParam<QString>(this, G_GetFwVersion)),
    mDeviceDescription(new cuDeviceParam<QString>(this, G_GetDeviceDescription)),
    mUDID(new cuDeviceParam<cUDID>(this, G_GetDeviceId))
{
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(waitingTimerTimeOut()));
}

AbstractDriver::~AbstractDriver()
{
    delete mHardwareVersion;
    delete mModificationVersion;
    delete mDeviceType;
}

void AbstractDriver::init()
{
    sendMsg(G_Init, 0, nullptr);
}

void AbstractDriver::goToPark()
{
    sendMsg(G_GoToPark, 0, nullptr);
}

void AbstractDriver::listeningOff()
{
    sendMsg(G_ListeningOff, 0, nullptr);
}

void AbstractDriver::getStatus()
{
    sendMsg(G_GetStatus, 0, nullptr);
}

void AbstractDriver::getLastError()
{
    sendMsg(G_GetLastError, 0, nullptr);
}

void AbstractDriver::rebootDevice()
{
    sendMsg(G_Reboot, 0, nullptr);
}

void AbstractDriver::writeEeprom()
{
    sendMsg(G_WriteEeprom, 0, nullptr);
}

bool AbstractDriver::waitingAnswer()
{
    while (!(mAnswerReceive || mTimeOut)){
        qApp->processEvents();
    }
    return (!mTimeOut);
}

bool AbstractDriver::getDeviceInfo()
{
    bool ok;
    mDeviceType->getValueSequence(&ok);
    if (!ok) return false;
    mModificationVersion->getValue();
    if (!waitingAnswer()) return false;
    mHardwareVersion->getValue();
    if (!waitingAnswer()) return false;
    mFirmwareVersion->getValue();
    if (!waitingAnswer()) return false;
    mDeviceDescription->getValue();
    if (!waitingAnswer()) return false;
    mUDID->getValue();
    return (waitingAnswer());
}

void AbstractDriver::sendMsg(quint8 command, quint8 dataLength, quint8 *data)
{
    mTimer->start(getDriverTimeOut());
    mTimeOut = false;
    mAnswerReceive = false;
    cuIODeviceImpl::sendMsg(command, dataLength, data);
}

void AbstractDriver::waitingTimerTimeOut()
{
    mTimeOut = true;
}

bool AbstractDriver::pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    Q_UNUSED(dataLength)
    Q_UNUSED(data)
    if (address != devAddress()) return false;

    mTimer->stop();
    mAnswerReceive = true;

    switch (command){
    case G_Init:
        break;
    case G_GoToPark:
        break;
    case G_GetLastError:
        break;
    case G_GetDeviceId:
        if (dataLength==12){
            cUDID tmpUDID;
            tmpUDID.setUDID(data);
            mUDID->setCurrentValue(tmpUDID);
        }
        break;
    case G_GetDeviceType:
        mDeviceType->setCurrentValue(QString(QByteArray((const char*)data, dataLength)));
        break;
    case G_GetModVersion:
        mModificationVersion->setCurrentValue(QString(QByteArray((const char*)data, dataLength)));
        break;
    case G_GetHwVersion:
        mHardwareVersion->setCurrentValue(QString(QByteArray((const char*)data, dataLength)));
        break;
    case G_GetFwVersion:
        mFirmwareVersion->setCurrentValue(QString(QByteArray((const char*)data, dataLength)));
        break;
    case G_GetDeviceDescription:
        mDeviceDescription->setCurrentValue(QString(QByteArray((const char*)data, dataLength)));
        break;
    case G_WriteEeprom:
        emit eepromConstWrited();
    default:
        return false;
    }
    return true;
}

int AbstractDriver::getDriverTimeOut() const
{
    return mDriverTimeOut;
}

void AbstractDriver::setDriverTimeOut(int driverTimeOut)
{
    mDriverTimeOut = driverTimeOut;
}

cuDeviceParam<cUDID> *AbstractDriver::getUDID() const
{
    return mUDID;
}

cuDeviceParam<QString> *AbstractDriver::getDeviceDescription() const
{
    return mDeviceDescription;
}

cuDeviceParam<QString> *AbstractDriver::getFirmwareVersion() const
{
    return mFirmwareVersion;
}

cuDeviceParam<QString> *AbstractDriver::getHardwareVersion() const
{
    return mHardwareVersion;
}

cuDeviceParam<QString> *AbstractDriver::getModificationVersion() const
{
    return mModificationVersion;
}

cuDeviceParam<QString> *AbstractDriver::getDeviceType() const
{
    return mDeviceType;
}
