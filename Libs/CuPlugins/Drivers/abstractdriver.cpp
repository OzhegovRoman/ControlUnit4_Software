#include "abstractdriver.h"
#include <QCoreApplication>
#include "driverproperty.h"

AbstractDriver::AbstractDriver(QObject *parent)
    : cuIODeviceImpl(parent)
    , mTimer(new QTimer(this))
{
    mTimer->setSingleShot(true);
    connect(mTimer, &QTimer::timeout, this, [=](){mTimeOut = true;});
}

bool AbstractDriver::waitingAnswer()
{
    while (!(mAnswerReceive || mTimeOut)){
        qApp->processEvents();
    }
    return (!mTimeOut);
}

int AbstractDriver::timeOut() const
{
    return mDriverTimeOut;
}

void AbstractDriver::setTimeOut(int value)
{
    mDriverTimeOut = value;
}

void AbstractDriver::appendProperty(DriverProperty_p *property)
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

void AbstractDriver::sendMsg(quint8 command, QByteArray data)
{
    mTimer->start(mDriverTimeOut);
    mTimeOut = false;
    mAnswerReceive = false;
    cuIODeviceImpl::sendMsg(command,
                            static_cast<quint8>(data.size()),
                            reinterpret_cast<quint8*>(data.data()));
}

bool AbstractDriver::pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
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
