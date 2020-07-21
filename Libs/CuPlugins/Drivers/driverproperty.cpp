#include "driverproperty.h"
#include <QDebug>


DriverProperty_p::DriverProperty_p(AbstractDriver *parent, quint8 cmdGetter, quint8 cmdSetter)
    : mDriver(parent)
    , mCmdGetter(cmdGetter)
    , mCmdSetter(cmdSetter)
    , mBuffer(QByteArray())
    , mGettedSignal(new ServiceSignal())
    , mSettedSignal(new ServiceSignal())
{
    mDriver->appendProperty(this);
}

QByteArray DriverProperty_p::data() const
{
    return mBuffer;
}

void DriverProperty_p::readWrite(quint8 cmd, QByteArray data)
{
    qDebug()<<"cmd"<<cmd<<"data"<<data.toHex();
    mDriver->sendMsg(cmd, data);
    mLastSettedValue = data;
}

QByteArray DriverProperty_p::readWriteSync(quint8 cmd, QByteArray data, bool *ok, int tryCount)
{
    bool tmp = false;
    while (0 < tryCount--){
        readWrite(cmd, data);
        tmp = mDriver->waitingAnswer();
        if (tmp)
            break;
    }
    if (ok)
        *ok = tmp;
    return DriverProperty_p::data();
}

void DriverProperty_p::setBufferData(const QByteArray &buffer)
{
    mBuffer = buffer;
}

QByteArray DriverProperty_p::lastSettedData() const
{
    return mLastSettedValue;
}

ServiceSignal *DriverProperty_p::settedSignal()
{
    return mSettedSignal;
}

ServiceSignal *DriverProperty_p::gettedSignal()
{
    return mGettedSignal;
}

quint8 DriverProperty_p::cmdGetter() const
{
    return mCmdGetter;
}

void DriverProperty_p::setCmdGetter(const quint8 &cmdGetter)
{
    mCmdGetter = cmdGetter;
}

quint8 DriverProperty_p::cmdSetter() const
{
    return mCmdSetter;
}

void DriverProperty_p::setCmdSetter(const quint8 &cmdSetter)
{
    mCmdSetter = cmdSetter;
}

void DriverCommand::execute()
{
    readWrite(cmdSetter(), QByteArray());
}

void DriverCommand::executeSync(bool *ok, int tryCount)
{
    readWriteSync(cmdSetter(), QByteArray(), ok, tryCount);
}
