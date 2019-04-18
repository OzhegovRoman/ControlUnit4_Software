#ifndef CUIODEVICEIMPL_H
#define CUIODEVICEIMPL_H

#include <QObject>
#include "cuiodevice.h"

class cuIODeviceImpl : public cuIODevice
{
    Q_OBJECT
public:
    explicit cuIODeviceImpl(QObject *parent = nullptr);

    cuIOInterface *iOInterface() const;
    void setIOInterface(cuIOInterface *iOInterface);

    quint8 devAddress() const;
    void setDevAddress(const quint8 &devAddress);

public slots:
    void sendMsg(quint8 command, quint8 dataLength, quint8* data);
    void msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data);
protected:
    virtual bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data) = 0;
private:
    cuIOInterface *mIOInterface;
    quint8 mDevAddress;

};

#endif // CUIODEVICEIMPL_H
