#include "cuiodeviceimpl.h"
#include <QDebug>

cuIODeviceImpl::cuIODeviceImpl(QObject *parent)
    : cuIODevice(parent)
{

}

cuIOInterface *cuIODeviceImpl::iOInterface() const
{
    return mIOInterface;
}

void cuIODeviceImpl::setIOInterface(cuIOInterface *iOInterface)
{
    if (iOInterface == nullptr) return;

    mIOInterface = iOInterface;
    connect(mIOInterface, SIGNAL(msgReceived(quint8,quint8,quint8,quint8*)),
            this, SLOT(msgReceived(quint8,quint8,quint8,quint8*)));
}

quint8 cuIODeviceImpl::devAddress() const
{
    return mDevAddress;
}

void cuIODeviceImpl::setDevAddress(const quint8 &devAddress)
{
    mDevAddress = devAddress;
}

void cuIODeviceImpl::sendMsg(quint8 command, quint8 dataLength, quint8 *data)
{
    if (mIOInterface == nullptr) return;
    mIOInterface->sendMsg(devAddress(), command, dataLength, data);
}

void cuIODeviceImpl::msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    pMsgReceived(address, command, dataLength, data);
}
