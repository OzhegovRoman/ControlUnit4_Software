#include "abstractdriver_v2.h"
#include <QCoreApplication>
#include "driverproperty.h"

AbstractDriver_V2::AbstractDriver_V2(QObject *parent)
    : cuIODeviceImpl(parent)
    , mTimer(new QTimer(this))
{
    mTimer->setSingleShot(true);
    connect(mTimer, &QTimer::timeout, this, [=](){mTimeOut = true;});
}

bool AbstractDriver_V2::waitingAnswer()
{
    while (!(mAnswerReceive || mTimeOut)){
        qApp->processEvents();
    }
    return (!mTimeOut);
}

int AbstractDriver_V2::timeOut() const
{
    return mDriverTimeOut;
}

void AbstractDriver_V2::setTimeOut(int value)
{
    mDriverTimeOut = value;
}

void AbstractDriver_V2::appendProperty(DriverProperty_p *property)
{
    QMutableVectorIterator<DriverProperty_p*> iter(mProperties);
    while (iter.hasNext()){
        DriverProperty_p* tmpProperty = iter.next();
        if (((tmpProperty->cmdGetter() == property->cmdGetter()) && (property->cmdGetter() != 0xFF)) ||
                ((tmpProperty->cmdSetter() == property->cmdSetter()) && (property->cmdSetter() != 0xFF)))
            iter.remove();
    }
    mProperties.push_back(property);
}

void AbstractDriver_V2::sendMsg(quint8 command, QByteArray data)
{
    mTimer->start(mDriverTimeOut);
    mTimeOut = false;
    mAnswerReceive = false;
    cuIODeviceImpl::sendMsg(command,
                            static_cast<quint8>(data.size()),
                            reinterpret_cast<quint8*>(data.data()));
}

bool AbstractDriver_V2::pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    if (address != devAddress()) return false;
    mTimer->stop();
    mAnswerReceive = true;

    for (auto property : mProperties)
    {
        if (command == property->cmdGetter())
        {
            //TODO: check dataLength

            property->setBufferData(QByteArray(reinterpret_cast<char*>(data), dataLength));
            property->gettedSignal()->emit_signal();
            return true;
        }

        if (command == property->cmdSetter())
        {
            property->settedSignal()->emit_signal();
            return true;
        }
    }
    return false;
}
